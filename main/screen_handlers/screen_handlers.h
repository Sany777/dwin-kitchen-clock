#pragma once


#include "dwin_config.h"
#include "dwin_data_types.h"
#include "screen_keys.h"
#include "dwin_defines.h"

#include "dwin_uart.h"
#include "dwin_drivers.h"
#include "screen_keys.h"
#include "dwin_events.h"
#include "screen_handlers.h"
#include "wifi_dwin.h"

/*screen handlers*/

void search_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void ap_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void setting_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void main_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void clock_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void state_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void notifications_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void timer_screen_handler(void* main_data, esp_event_base_t base, int32_t key, void* event_data);
void timer_run_handler(void* data, esp_event_base_t base, int32_t key, void* event_data);