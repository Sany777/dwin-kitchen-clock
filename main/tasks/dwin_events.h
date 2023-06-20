#pragma once

#include "dwin_common.h"

#define PRIORITY_UART           6
#define PRIORITY_DIRECTION      (PRIORITY_UART-1)
#define PRIORITY_FAST_SERVICE   (PRIORITY_DIRECTION-1)
#define PRIORITY_SHOW           (PRIORITY_DIRECTION-2)

extern  dwin_screen_handler_t  wifi_set_mode_handler;
extern  dwin_screen_handler_t  search_screen_handler;
extern  dwin_screen_handler_t  ap_screen_handler;
extern  dwin_screen_handler_t  setting_screen_handler;
extern  dwin_screen_handler_t  main_screen_handler;
extern  dwin_screen_handler_t  clock_handler;
extern  dwin_screen_handler_t  state_screen_handler;
extern  dwin_screen_handler_t  notifications_screen_handler;
extern  dwin_screen_handler_t  timer_screen_handler;
extern  dwin_screen_handler_t  set_color_screen_handler;
extern  dwin_screen_handler_t  device_screen_handler;
extern  dwin_screen_handler_t  info_screen_handler;
extern  dwin_show_handler_t  show_timer_handler;
extern  dwin_show_handler_t  show_main_handler;
extern  dwin_show_handler_t  show_clock_handler;
extern  dwin_show_handler_t  show_ssid_handler;
extern  dwin_show_handler_t  show_settings_handler;
extern  dwin_show_handler_t  show_color_screen_handler;
extern  dwin_show_handler_t  show_notify_handler;
extern  dwin_show_handler_t  show_ap_handler;
extern  dwin_show_handler_t  show_state_handler;
extern  dwin_show_handler_t  show_timer_stop_handler;
extern  dwin_show_handler_t  show_info_handler;
extern  dwin_show_handler_t  show_device_handler;

/* events structure */
#define SIZE_LIST_TASKS       (END_KEY_SCREEN_TASK-START_SCREEN_TASK)

void init_dwin_events(main_data_t*);
void show_task(void*);
void service_task(void *);
void direction_task(void *);
void uart_event_task(void *);
void check_net_data(main_data_t*);


#define screen_handler(main_data, command, char_in)  screens_handlers[cur_screen_id-START_SCREEN_TASK].main_handler(main_data, command, char_in)
#define show_handler(main_data, key, data)    screens_handlers[cur_screen_id-START_SCREEN_TASK].show_handler(main_data, key, data)


#define SIZE_SERVICE_TASK 4

static task_dwin_t const sevice_tasks[SIZE_SERVICE_TASK] = {

    {
        .pTask = show_task,
        .priority = PRIORITY_SHOW,
        .stack = 4000
    },
    {
        .pTask = direction_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 8000
    },
    {
        .pTask = service_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 8000
    },
    {
        .pTask = uart_event_task,
        .priority = PRIORITY_UART,
        .stack = 4000
    },
};


static const handlers_dwin_t screens_handlers[SIZE_LIST_TASKS] = {
    {
        .main_handler    = main_screen_handler,
        .show_handler    = show_main_handler,
    },
    {
        .main_handler    = clock_handler,
        .show_handler    = show_clock_handler,
    },
    {
        .main_handler    = search_screen_handler,
        .show_handler    = show_ssid_handler,
    },
    {
        .main_handler    = setting_screen_handler,
        .show_handler    = show_settings_handler,
    },
    {
        .main_handler    = set_color_screen_handler,
        .show_handler    = show_color_screen_handler,
    },
    {
        .main_handler    = notifications_screen_handler,
        .show_handler    = show_notify_handler,
    },
    {
        .main_handler    = ap_screen_handler,
        .show_handler    = show_ap_handler,
    },
    {
        .main_handler    = state_screen_handler,
        .show_handler    = show_state_handler,
    },
    {
        .main_handler    = timer_screen_handler,
        .show_handler    = show_timer_handler
    },
    {
        .main_handler    = info_screen_handler,
        .show_handler    = show_info_handler
    },
    {
        .main_handler    = device_screen_handler,
        .show_handler    = show_device_handler
    },
};


void vApplicationIdleHook(void);


