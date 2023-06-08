#include "dwin_init.h"

EventGroupHandle_t dwin_event_group;

void esp_init(void)
{
    main_data_t *main_data = (main_data_t *) calloc(1, sizeof(main_data_t));
    assert(main_data);
    show_buf = malloc(SIZE_SHOW_BUF);
    assert(show_buf);
    main_data->weather_data = calloc(1, sizeof(weather_data_t));
    assert(main_data->weather_data);
    dwin_event_group = xEventGroupCreate();
    assert(dwin_event_group);
    main_data->notif_data = calloc(1,SIZE_BUF_NOTIFICATION);
    assert(main_data->notif_data);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ESP_ERROR_CHECK(nvs_flash_init());
    }
    int offset;
    read_memory(&offset, DATA_OFFSET);
    temp_INDOOR = 100;
    set_timezone(offset);
    init_uart();
    read_all_memory(main_data);
    wifi_init();
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
