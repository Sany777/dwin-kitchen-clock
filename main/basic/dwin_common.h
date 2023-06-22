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
#include "esp_task_wdt.h"


ESP_EVENT_DECLARE_BASE(ESPNOW_EVENTS);
ESP_EVENT_DECLARE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DECLARE_BASE(SNTP_EVENTS);

extern char *buf_operation;
extern EventGroupHandle_t dwin_event_group;
extern QueueHandle_t dwin_uart_events_queue, 
                        queue_espnow_tx, 
                        queue_espnow_rx, 
                        queue_direct, 
                        queue_service,
                        queue_show;
extern TaskHandle_t rx_espnow, tx_espnow;
extern uint8_t cur_screen_id;
static char * TAG = "dwin";


#include "pm_impl.h"

typedef struct {
    int max_freq_mhz;   /*!< Maximum CPU frequency, in MHz */
    int min_freq_mhz;   /*!< Minimum CPU frequency to use when no locks are taken, in MHz */
    bool light_sleep_enable;        /*!< Enter light sleep when no locks are taken */
} esp_pm_config_esp32_t;