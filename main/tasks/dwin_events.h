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

typedef struct {
    TaskFunction_t pTask;
    UBaseType_t priority;
    uint32_t stack;
} task_dwin_t;

typedef struct {
    dwin_screen_handler_t *main_handler;
    dwin_show_handler_t *show_handler;
} handlers_dwin_t;


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

void show_task(void*);
void direction_task(void *);
void uart_event_task(void *);
void slow_service_task(void *pv);
void fast_service_task(void *pv);

#define screen_handler(main_data, command, char_in)  screens_handlers[cur_screen_id].main_handler(main_data, command, char_in)
#define show_handler(main_data, key, data)    screens_handlers[cur_screen_id].show_handler(main_data, key, data)


#define show_screen(key, data)                              \
    do{                                                     \
        show_queue_data_t to_send = {                       \
            .command = key,                                 \
            .data = data                                    \
        };                                                  \
        xQueueSend(queue_show, to_send, WAIT_SHOW);         \
    }while(0)

    
/* loop task */
#define start_ap()                                                                 \
    do{                                                                            \
        esp_event_post_to(fast_service_loop, WIFI_SET_EVENTS, INIT_AP, NULL, 0, 200);     \
    }while(0)    

#define get_weather()                                                                       \
    do{                                                                                     \
        esp_event_post_to(slow_service_loop, WIFI_SET_EVENTS, GET_WEATHER, NULL, 0, WAIT_SERVICE); \
    }while(0)                                                                               \

#define start_sta()                                                                         \
    do{                                                                                     \
        esp_event_post_to(fast_service_loop, WIFI_SET_EVENTS, START_STA, NULL, 0, 200);            \
    }while(0) 

#define stop_espnow()                                                                           \
    do{                                                                                         \
        esp_event_post_to(slow_service_loop, ESPNOW_EVENTS, STOP_ESPNOW, NULL, 0, WAIT_SERVICE);   \
    }while(0)

#define start_espnow()                                                                          \
    do{                                                                                         \
        esp_event_post_to(slow_service_loop, ESPNOW_EVENTS, START_ESPNOW, NULL, 0, 200);           \
    }while(0) 
#define start_sntp()                                                                            \
    do{                                                                                         \
        esp_event_post_to(slow_service_loop, WIFI_SET_EVENTS, INIT_SNTP, NULL, 0, WAIT_SERVICE);       \
    }while(0) 

#define stop_sntp()                                                                            \
    do{                                                                                        \
        esp_event_post_to(slow_service_loop, SNTP_EVENTS, STOP_SNTP, NULL, 0, WAIT_SERVICE);      \
    }while(0) 

void init_dwin_events(main_data_t*);
void check_net_data_handler(main_data_t* main_data);

#define SIZE_SERVICE_TASK 5

 static task_dwin_t sevice_tasks[SIZE_SERVICE_TASK] = {

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
        .pTask = fast_service_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 5000
    },
    {
        .pTask = slow_service_task,
        .priority = PRIORITY_SLOW_SERVICE,
        .stack = 5000
    },
    {
        .pTask = uart_event_task,
        .priority = 10,
        .stack = 4048
    },
};




void vApplicationIdleHook(void );
