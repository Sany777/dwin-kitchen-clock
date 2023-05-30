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



void get_weather(void *pv);
char** find_str_key(char *buf, const size_t buf_len, const char *key);