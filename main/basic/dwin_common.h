#pragma once


#include "assert.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "nvs.h"

#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "dwin_memory.h"
#include "dwin_config.h"
#include "dwin_defines.h"
#include "dwin_data_types.h"
#include "screen_keys.h"
#include "screen_handlers.h"
#include "wifi_dwin.h"
#include "device_info.h"
#include "dwin_init.h"
#include "espnow_dwin.h"
#include "dwin_config.h"
#include "dwin_data_types.h"
#include "screen_keys.h"
#include "dwin_defines.h"
#include "show_data.h"
#include "dwin_timer.h"
#include "dwin_help.h"
#include "dwin_drivers.h"


static EventGroupHandle_t dwin_event_group;
// static QueueHandle_t dwin_uart_events_queue;