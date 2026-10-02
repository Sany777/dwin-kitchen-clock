#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "screen_keys.h"

uint16_t dwin_help_get_color_temp(float temp);
const char *parser_get_rssi_description(int8_t rssi);
const char *parser_get_auth_mode(int authmode);
picture_t get_pic(unsigned int weather_id, bool night);
