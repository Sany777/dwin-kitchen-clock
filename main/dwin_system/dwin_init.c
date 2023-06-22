#include "dwin_init.h"

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
            vTaskDelay(2000);
            esp_restart();
        };
    }
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
    } else {
        vTaskDelay(1000);
        dwin_clock_get();
    }
    set_new_event(MAIN_SCREEN);
}

void set_power_mode_eco(const bool run)
{
    esp_pm_impl_init();
    esp_pm_config_esp32c3_t pv = {
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




void i2c_master_init()
{ 
	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA_PIN,
		.scl_io_num = SCL_PIN,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = 1000000
	};
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_config));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
}

int8_t BME280_I2C_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int32_t iError = BME280_INIT_VALUE;

	esp_err_t espRc;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, reg_addr, true);
	i2c_master_write(cmd, reg_data, cnt, true);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		iError = SUCCESS;
	} else {
		iError = ERROR;
	}
	i2c_cmd_link_delete(cmd);

	return (int8_t)iError;
}

int8_t BME280_I2C_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int32_t iError = BME280_INIT_VALUE;
	esp_err_t espRc;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

	if (cnt > 1) {
		i2c_master_read(cmd, reg_data, cnt-1, I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, reg_data+cnt-1, I2C_MASTER_NACK);
	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
	if (espRc == ESP_OK) {
		iError = SUCCESS;
	} else {
		iError = ERROR;
	}

	i2c_cmd_link_delete(cmd);

	return (int8_t)iError;
}

void BME280_delay_msek(uint32_t msek)
{
	vTaskDelay(msek/portTICK_PERIOD_MS);
}

void init_bme280(void)
{
	struct bme280_t bme280 = {
		.bus_write = BME280_I2C_bus_write,
		.bus_read = BME280_I2C_bus_read,
		.dev_addr = BME280_I2C_ADDRESS1,
		.delay_msec = BME280_delay_msek
	};
	int32_t com_rslt;
    com_rslt = bme280_init(&bme280);
	com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
	com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_2X);
	com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
	com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
	com_rslt += bme280_set_filter(BME280_FILTER_COEFF_16);
	com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);
	if (com_rslt != SUCCESS) {
		ESP_LOGE(TAG_BME280, "init BM280 or setting error.");
	}
}

void read_sensor_handler(main_data_t* main_data)
{

    bme280_read_uncomp_temperature(&temp_BM280);
    temp_BM280 = bme280_compensate_temperature_int32(temp_BM280);

    ESP_LOGI(TAG_BME280, "%3d degC", (int)temp_BM280);

}