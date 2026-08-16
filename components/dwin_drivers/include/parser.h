#pragma once


#include "dwin_uart.h"
#include "freertos/task.h"
#include "esp_netif.h"

#include "dwin_common.h"

uint16_t dwin_help_get_color_temp(float temp);
const char *parser_get_rssi_description(int8_t rssi);
const char *parser_get_auth_mode(int authmode);
picture_t get_pic(const uint weather_id, const bool night);
