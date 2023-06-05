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
#include "dwin_weather.h"
#include "parser.h"
#include "dwin_timer.h"
#include "sensor.h"
#include "show_screen_handlers.h"

ESP_EVENT_DECLARE_BASE(WIFI_SET); 
ESP_EVENT_DECLARE_BASE(ESPNOW_SET); 
ESP_EVENT_DECLARE_BASE(EVENTS_SERVICE); 
ESP_EVENT_DECLARE_BASE(EVENTS_MANAGER); 
ESP_EVENT_DECLARE_BASE(EVENTS_DIRECTION); 
ESP_EVENT_DECLARE_BASE(EVENTS_SHOW);


extern char *show_buf;
extern EventGroupHandle_t dwin_event_group;
extern esp_event_loop_handle_t 
                direct_loop, 
                show_loop, 
                fast_service_loop,
                slow_service_loop;
extern QueueHandle_t dwin_uart_events_queue, queue_espnow_tx, queue_espnow_rx;
extern char *buf_format_time;
extern TaskHandle_t rx_espnow, tx_espnow;