#pragma once

#include "assert.h"
#include "dwin_config.h"
#include "dwin_data_types.h"
#include "dwin_defines.h"
#include "esp_chip_info.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs.h"
#include "sdkconfig.h"
#include "string.h"
#include <freertos/FreeRTOS.h>

ESP_EVENT_DECLARE_BASE(WIFI_SET_EVENTS);
ESP_EVENT_DECLARE_BASE(SNTP_EVENTS);

extern char *send_buf;
extern EventGroupHandle_t dwin_event_group;
extern QueueHandle_t dwin_uart_events_queue, direct_queue, service_queue, show_queue;
extern SemaphoreHandle_t dwin_display_mutex;
extern uint8_t cur_screen_id;
extern uint32_t cur_view_generation;
extern const uint16_t USED_COLORS[];
