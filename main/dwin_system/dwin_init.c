#include "dwin_init.h"

EventGroupHandle_t dwin_event_group;
TaskHandle_t rx_espnow = NULL, tx_espnow = NULL;
QueueHandle_t 
          dwin_uart_events_queue = NULL, 
          queue_espnow_tx = NULL, 
          queue_espnow_rx = NULL, 
          queue_direct = NULL, 
          queue_service = NULL, 
          queue_show = NULL;
#define SIZE_QUEUE_DIRECT 10



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
    main_data->notif_data = malloc(SIZE_NOTIFICATION);
    assert(main_data->notif_data);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ESP_ERROR_CHECK(nvs_flash_init());
    }
    int offset;
    read_offset(&offset);
    set_timezone(offset);
    init_uart();
    // welcome();
    read_all_memory(main_data);
    wifi_init();
        
    // cancel_text_box();
    // set_color(GREEN,BLACK);
    // clear_screen();

    init_dwin_events(main_data);
}


void wifi_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());
    const wifi_init_config_t cfg = (const wifi_init_config_t) WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}
