#pragma once

#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <ctype.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "cJSON.h"
#include "esp_http_client.h"
#include "dwin_common.h"



void get_currency(dwin_data_t *main_data);
void get_weather(dwin_data_t *main_data, uint8_t key);
