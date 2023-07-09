#pragma once


#include "assert.h"
#include "esp_now.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "nvs.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "bmx280.h"
#include "dwin_memory.h"
#include "dwin_config.h"
#include "dwin_defines.h"
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
#include "bmp280_main.h"

ESP_EVENT_DECLARE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DECLARE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DECLARE_BASE(SNTP_EVENTS);

extern char *send_buf;
extern EventGroupHandle_t dwin_event_group;
extern QueueHandle_t dwin_uart_events_queue, 
                        queue_espnow_tx, 
                        queue_espnow_rx, 
                        queue_direct, 
                        queue_service,
                        queue_show;
extern TaskHandle_t rx_espnow, tx_espnow;
extern uint8_t cur_screen_id;
extern const uint16_t USED_COLORS[];
