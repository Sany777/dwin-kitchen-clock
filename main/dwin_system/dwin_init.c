#include "dwin_init.h"

bmx280_t* bmx280;

ESP_EVENT_DEFINE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DEFINE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DEFINE_BASE(SNTP_EVENTS);

static const task_dwin_t sevice_tasks[SIZE_SERVICE_TASK] = {
    {
        .pTask = show_task,
        .priority = PRIORITY_SHOW,
        .stack = 4000
    },
    {
        .pTask = direction_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 8000
    },
    {
        .pTask = service_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 8000
    },
    {
        .pTask = uart_event_task,
        .priority = PRIORITY_UART,
        .stack = 4000
    }
};

EventGroupHandle_t dwin_event_group;
TaskHandle_t rx_espnow = NULL, tx_espnow = NULL;
QueueHandle_t 
          dwin_uart_events_queue = NULL, 
          queue_espnow_tx = NULL, 
          queue_espnow_rx = NULL, 
          queue_direct = NULL, 
          queue_service = NULL, 
          queue_show = NULL;

void esp_init(void)
{
    main_data_t *main_data = (main_data_t *) calloc(1, sizeof(main_data_t));
    assert(main_data);
    buf_operation = malloc(SIZE_SHOW_BUF);
    assert(buf_operation);
    dwin_event_group = xEventGroupCreate();
    queue_direct = xQueueCreate(SIZE_QUEUE_DIRECT, sizeof(uint16_t));
    queue_show = xQueueCreate(3, sizeof(show_queue_data_t));
    queue_service = xQueueCreate(SIZE_QUEUE_DIRECT, sizeof(uint8_t));
    assert(queue_show);
    assert(queue_service);
    assert(dwin_event_group);
    assert(queue_direct);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ESP_ERROR_CHECK(nvs_flash_init());
    }
    read_all_memory(main_data);
    int offset;
    read_offset(&offset);
    set_timezone(offset);
    init_uart();
    wifi_init();
    vTaskDelay(100);
    set_new_event(START_STA);
    for(int i=0; i<SIZE_SERVICE_TASK; i++){
        if(xTaskCreate(
            sevice_tasks[i].pTask, 
            "service",
            sevice_tasks[i].stack, 
            (void * const)main_data, 
            sevice_tasks[i].priority,
            NULL
        ) != pdTRUE)
        {
            send_message_dwin("No enough memory. Task no create");
            vTaskDelay(10000);
            esp_restart();
        };
    }
    vTaskDelay(1000);
    // esp_pm_impl_init();
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(xEventGroup&BIT_SNTP_ALLOW){
        xEventGroupSetBits(dwin_event_group, BIT_PROCESS);
        set_new_event(START_STA);
        set_new_event(INIT_SNTP);
    }
    do{
        send_hello();
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_DWIN_RESPONSE_OK, false, false, 1000);
    }while(!(xEventGroup&BIT_DWIN_RESPONSE_OK));
    dwin_set_pic(NO_WEATHER_PIC);
    welcome();
    if(xEventGroup&BIT_SNTP_ALLOW){
        xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, true, true, 3000);
    }
    if(!(xEventGroup&BIT_CON_STA_OK)){
        vTaskDelay(1000);
        dwin_clock_get();
    }
    temp_BM280 = NO_TEMP;
    set_new_event(MAIN_SCREEN);
    vTaskDelay(1000);
    if(init_bmp280() == ESP_OK){
        set_new_event(GET_TEMPERATURE);
        set_periodic_event(GET_TEMPERATURE, 35, RELOAD_COUNT);
    }
}

void set_power_mode_eco(const bool run)
{
    esp_pm_config_esp32_t pv = {
        .max_freq_mhz = 160,
        .min_freq_mhz = 40,
        .light_sleep_enable = run
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pv));
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    const wifi_init_config_t cfg = (const wifi_init_config_t) WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

esp_err_t init_bmp280(void)
{
    i2c_config_t i2c_cfg = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .sda_pullup_en = false,
        .scl_pullup_en = false,
        .master = {
            .clk_speed = 100000
        }
    };
    DWIN_CHECK_AND_RETURN(i2c_param_config(I2C_NUM_0, &i2c_cfg));
    DWIN_CHECK_AND_RETURN(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    bmx280 = bmx280_create(I2C_NUM_0);
    if (!bmx280) { 
        return ESP_FAIL;
    }
    DWIN_CHECK_AND_RETURN(bmx280_init(bmx280));
    bmx280_config_t bmx_cfg = BMX280_DEFAULT_CONFIG;
    DWIN_CHECK_AND_RETURN(bmx280_configure(bmx280, &bmx_cfg));
    return ESP_OK;
}

void read_sensor_handler(main_data_t* main_data)
{
    bmx280_setMode(bmx280, BMX280_MODE_FORCE);
    do {
        vTaskDelay(pdMS_TO_TICKS(10));
    } while(bmx280_isSampling(bmx280));
    bmx280_readoutFloat(bmx280, &temp_BM280, &pressure_BM280, 0);
}