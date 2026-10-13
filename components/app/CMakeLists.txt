idf_component_register(
    SRCS
        "app_init.c"
        "dwin_events.c"
        "dwin_uart.c"
        "screen_handlers.c"
        "show_screen_handlers.c"
    INCLUDE_DIRS "includes"
    REQUIRES core wifi net dwin_screen driver esp_wifi esp_hw_support nvs_flash esp_netif
            esp_system spi_flash esp_timer esp_app_format
)
