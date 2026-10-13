idf_component_register(SRCS "wifi_dwin.c" "wifi_init.c" "espnow_devices.c"
                       INCLUDE_DIRS "includes"
                       REQUIRES core esp_wifi esp_event esp_netif dwin_screen esp_timer)
