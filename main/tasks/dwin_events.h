#pragma once

#include "dwin_common.h"



#define PRIORITY_DIRECTION      5
#define PRIORITY_FAST_SERVICE   (PRIORITY_DIRECTION-1)
#define PRIORITY_SHOW           (PRIORITY_DIRECTION-2)
#define PRIORITY_SLOW_SERVICE   (PRIORITY_DIRECTION-3)


#define sleep_dwin(time_us)                                                                     \
    do{                                                             \
        ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(time_us));    \
        uart_wait_tx_idle_polling(UART_DWIN);                       \
        ESP_ERROR_CHECK(esp_light_sleep_start());                   \
    }while(0)
                                    //stop_event_timer();                                     

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

/* events structure */
#define SIZE_LIST_TASKS       (END_KEY_SCREEN_TASK-1)

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
        .stack = 2000
    },
    {
        .pTask = direction_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 2000
    },
    {
        .pTask = service_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 10000
    },
    {
        .pTask = uart_event_task,
        .priority = 10,
        .stack = 4048
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
};


void vApplicationIdleHook(void);
