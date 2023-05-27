#pragma once


#include "dwin_uart.h"
#include "freertos/task.h"
#include "esp_netif.h"

#include "dwin_common.h"


const char *get_rssi_description(uint8_t rssi);
picture_t get_pic(const uint weather_id, const bool night);
const char *get_auth_mode(int authmode);

