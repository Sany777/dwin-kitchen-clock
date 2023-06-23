#include "dwin_init.h"

ESP_EVENT_DEFINE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DEFINE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DEFINE_BASE(SNTP_EVENTS);

const task_dwin_t sevice_tasks[SIZE_SERVICE_TASK] = {
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
    vTaskDelay(300);
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
    set_new_event(START_STA);
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    xEventGroupSetBits(dwin_event_group, BIT_PROCESS);
    do{
        send_hello();
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_DWIN_RESPONSE_OK, false, false, 1000);
    }while(!(xEventGroup&BIT_DWIN_RESPONSE_OK));
    dwin_set_pic(NO_WEATHER_PIC);
    welcome();
    if(xEventGroup&BIT_SNTP_ALLOW){
        set_new_event(INIT_SNTP);
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, true, true, 3000);
    }
    if(!(xEventGroup&BIT_CON_STA_OK) 
        || !(xEventGroup&BIT_SNTP_ALLOW))
    {
        vTaskDelay(1000);
        dwin_clock_get();
    }
    temp_BM280 = NO_TEMP;
    set_new_event(MAIN_SCREEN);
    vTaskDelay(200);
    if(init_bmp280() == ESP_OK){
        vTaskDelay(200);
        set_new_event(GET_TEMPERATURE);
        set_periodic_event(GET_TEMPERATURE, 35, RELOAD_COUNT);
    }
}

void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    const wifi_init_config_t cfg = (const wifi_init_config_t) WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}
