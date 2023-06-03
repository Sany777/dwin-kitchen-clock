#pragma once

#include "esp_timer.h"
#include <sys/queue.h>
#include "esp_err.h"
#include "esp_event.h"

#include "dwin_config.h"
#include "dwin_defines.h"
#include "dwin_data_types.h"
#include "dwin_events.h"

ESP_EVENT_DECLARE_BASE(TIMER_TURNED_ON);


esp_err_t add_periodic_event(esp_event_base_t basa, int32_t event_id, size_t sec, mode_time_func_t mode);
void periodic_timer_callback(void* arg);
esp_err_t set_mode_timer_event(esp_event_base_t basa, int32_t event_id, action_timer_func_t mode);
esp_err_t init_event_timer(main_data_t *main_data);
void remove_event_timer(void);
void stop_event_timer(void);
