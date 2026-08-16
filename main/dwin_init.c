#include "dwin_init.h"
#include "dwin_events.h"
#include "dwin_help.h"
#include "bmp280_main.h"
#include "show_screen_handlers.h"
#include "dwin_uart.h"
#include "nvs_flash.h"
#include "dwin_memory.h"
#include "wifi_dwin.h"
#include "dwin_timer.h"
#include "dwin_drivers.h"
#include "esp_wifi.h"
#include "show_data.h"

ESP_EVENT_DEFINE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DEFINE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DEFINE_BASE(SNTP_EVENTS);

const task_dwin_t service_tasks[SIZE_SERVICE_TASK] = {
    {.pTask = task_show, .priority = PRIORITY_SHOW, .stack = 4000},
    {.pTask = task_direction, .priority = PRIORITY_FAST_SERVICE, .stack = 8000},
    {.pTask = task_service, .priority = PRIORITY_FAST_SERVICE, .stack = 8000},
    {.pTask = task_uart_event, .priority = PRIORITY_UART, .stack = 4000}};

EventGroupHandle_t dwin_event_group;
TaskHandle_t rx_espnow = NULL, tx_espnow = NULL;
QueueHandle_t dwin_uart_events_queue = NULL, espnow_tx_queue = NULL, espnow_rx_queue = NULL, direct_queue = NULL,
              service_queue = NULL, show_queue = NULL;

void
kitchen_clock_init(void)
{
    static dwin_data_t main_data_inst;
    dwin_data_t *main_data = &main_data_inst;
    memset(main_data, 0, sizeof(dwin_data_t));
    dwin_event_group = xEventGroupCreate();
    direct_queue     = xQueueCreate(SIZE_QUEUE_DIRECT, sizeof(uint16_t));
    show_queue       = xQueueCreate(3, sizeof(show_queue_data_t));
    service_queue    = xQueueCreate(SIZE_QUEUE_DIRECT, sizeof(uint8_t));
    assert(show_queue);
    assert(service_queue);
    assert(dwin_event_group);
    assert(direct_queue);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    dwin_memory_read_all(main_data);
    int32_t offset;
    dwin_memory_read_offset(&offset);
    wifi_dwin_set_timezone(offset);
    init_uart();
    wifi_init();
    vTaskDelay(300);
    dwin_timer_set_new_command(START_STA);
    for (int i = 0; i < SIZE_SERVICE_TASK; i++) {
        if (xTaskCreate(service_tasks[i].pTask, "service", service_tasks[i].stack, (void *const)main_data,
                        service_tasks[i].priority, NULL) != pdTRUE) {
            show_data_send_message("No enough memory. Task no create");
            vTaskDelay(10000);
            esp_restart();
        };
    }
    temp_BM280 = DWIN_NO_DATA;
    dwin_help_init_currency_val(main_data);
    vTaskDelay(1000);
    dwin_timer_set_new_command(START_STA);
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    xEventGroupSetBits(dwin_event_group, BIT_PROCESS);
    do {
        dwin_send_hello();
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_DWIN_RESPONSE_OK, false, false, 1000);
    } while (!(xEventGroup & BIT_DWIN_RESPONSE_OK));
    dwin_set_picture(NO_WEATHER_PIC);
    welcome();
    if (xEventGroup & BIT_SNTP_ALLOW) {
        dwin_timer_set_new_command(INIT_SNTP);
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_PROCESS, true, true, 3000);
    }
    if (!(xEventGroup & BIT_CON_STA_OK) || !(xEventGroup & BIT_SNTP_ALLOW)) {
        vTaskDelay(1000);
        dwin_clock_get();
    }

    dwin_timer_set_new_command(MAIN_SCREEN);
    vTaskDelay(200);
    if (bme280_init() == ESP_OK) {
        vTaskDelay(200);
        dwin_timer_set_new_command(GET_TEMPERATURE);
    }
    dwin_timer_set_new_command(UPDATE_CURRENCY);
}

void
wifi_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    const wifi_init_config_t cfg = (const wifi_init_config_t)WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}
