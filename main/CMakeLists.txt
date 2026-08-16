idf_component_register(
                SRC_DIRS "." 
                INCLUDE_DIRS "include"
                PRIV_REQUIRES common bme280 weather_client wifi ap_server dwin_drivers esp_http_client esp_http_server json esp_wifi spi_flash nvs_flash esp_netif driver esp_driver_i2c esp_timer)

