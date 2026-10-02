idf_component_register(SRCS "math_utils.c" "string_utils.c" "notification_utils.c"
                             "freshness_utils.c"
                             "dwin_timer.c"
                       INCLUDE_DIRS "includes"
                       REQUIRES esp_event esp_netif nvs_flash esp_timer driver esp_driver_uart
                                esp_system spi_flash esp_hw_support)
