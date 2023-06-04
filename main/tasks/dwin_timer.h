#pragma once

#include "esp_timer.h"
#include <sys/queue.h>
#include "esp_err.h"
#include "esp_event.h"

#include "dwin_config.h"
#include "dwin_defines.h"
#include "dwin_data_types.h"
#include "dwin_events.h"

esp_err_t set_periodic_event(esp_event_loop_handle_t event_loop, 
                                esp_event_base_t basa, 
                                int32_t event_id, 
                                size_t sec, 
                                mode_time_func_t mode);
void periodic_timer_cb(void* arg);
esp_err_t init_event_timer(void);
void remove_event_timer(void);
void stop_event_timer(void);
void remove_periodic_event(esp_event_base_t base, int32_t event_id);