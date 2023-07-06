#pragma once

#include "dwin_common.h"
#include "show_screen_handlers.h"
#include "screen_handlers.h"

#define PRIORITY_UART           6
#define PRIORITY_DIRECTION      (PRIORITY_UART-1)
#define PRIORITY_FAST_SERVICE   (PRIORITY_DIRECTION-1)
#define PRIORITY_SHOW           (PRIORITY_DIRECTION-2)

/* events structure */
#define SIZE_LIST_TASKS         (END_KEY_SCREEN_TASK-START_SCREEN_TASK)

void init_dwin_events(main_data_t*);
void show_task(void*);
void service_task(void *);
void direction_task(void *);
void uart_event_task(void *);
void check_net_data(main_data_t*);

/* main screen func*/
#define screen_handler(main_data, command, char_in) \
    screens_handlers[cur_screen_id-START_SCREEN_TASK].main_handler(main_data, command, char_in)

/* show func */
#define show_handler(main_data, key, data) \
    screens_handlers[cur_screen_id-START_SCREEN_TASK].show_handler(main_data, key, data)

void vApplicationIdleHook(void);


