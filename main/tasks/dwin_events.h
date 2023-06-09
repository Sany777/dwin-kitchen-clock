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


enum {
    CPU_0,
    CPU_1
};


/*screen tasks*/
typedef enum {
    MAIN_SCREEN,
    CLOCK_SCREEN,
    DEVICE_SCREEN,
    TIMER_SCREEN,
    SETTING_SCREEN,
    SEARCH_SSID_SCREEN,
    CUSTOMIZATION_SCREEN,
    NOTIFICATION_SCREEN,
    SERVER_SCREEN,
    MAIN_HANDLERS_LIST_END
} identefier_handler_screen_t;

typedef enum {
    MAIN_TASK,
    SHOW_TASK,
    FAST_TASK,
    SLOW_TASK,
    UART_TASK,
    END_LIST_TASKS
} identefier_system_task_t;

#define SIZE_LIST_HANDLERS    MAIN_HANDLERS_LIST_END
#define SIZE_LIST_TASKS    END_LIST_TASKS

typedef struct {
    TaskFunction_t pTask;
    UBaseType_t priority;
    uint32_t stack;
} task_dwin_t;

typedef struct {
    dwin_handler_t *main_handler;
    dwin_handler_t *show_handler;
    dwin_handler_t *service_handler;
} handlers_dwin_t;


extern  dwin_handler_t screen_change_handler;
extern  dwin_handler_t setting_handler;
extern  dwin_handler_t search_screen_handler;
extern  dwin_handler_t ap_handler;
extern  dwin_handler_t device_screen_handler;
extern  dwin_handler_t main_screen_handler;
extern  dwin_handler_t clock_handler;
extern  dwin_handler_t set_color_screen_handler;
extern  dwin_handler_t notification_screen_handler;
extern  dwin_handler_t timer_screen_handler;


extern  dwin_handler_t show_ssid_handler;
extern  dwin_handler_t show_device_handler;
extern  dwin_handler_t show_rename_device_handler;
extern  dwin_handler_t show_notify_handler;
extern  dwin_handler_t show_custom_handler;
extern  dwin_handler_t show_setting_handler;
extern  dwin_handler_t show_clock_handler;
extern  dwin_handler_t show_main_handler;
extern  dwin_handler_t show_timer_handler;

extern  dwin_handler_t wifi_set_mode_handler;

/* events structure */
 static handlers_dwin_t links_handlers_list[SIZE_LIST_HANDLERS] = {
    // {
    //     .main_handler    = main_screen_handler,
    //     .show_handler    = show_main_handler,
    // },
    // {
    //     .main_handler    = clock_handler,
    //     .show_handler    = show_clock_handler,
    // },    
    // {
    //     .main_handler    = device_screen_handler,
    //     .show_handler    = show_device_handler,
    // },
    // {
    //     .main_handler    = timer_screen_handler,
    //     .show_handler    = show_timer_handler,
    // },
    // {
    //     .main_handler    = search_screen_handler,
    //     .show_handler    = show_ssid_handler,
    // },

    // {
    //     .main_handler    = set_color_screen_handler,
    //     .show_handler    = show_custom_handler,
    // },
    // {
    //     .main_handler    = notification_screen_handler,
    //     .show_handler    = show_notify_handler,
    // },
    // {
    //     .main_handler    = ap_handler,
    //     .show_handler    = NULL,
    // },
};


/* loop task */
void show_task(void*);
void direction_task(void *);
void slow_services_task(void *pv);
void fast_services_task(void *pv);
void uart_event_task(void *);

#define show_screen(dt)                                                                                             \
                        do{                                                                                         \
                            esp_event_post_to(show_loop, EVENTS_SHOW, SHOW_SCREEN, dt, sizeof(dt), WAIT_SERVICE);   \
                        }while(0) 

#define start_ap()                                                                                              \
                        do{                                                                                     \
                            esp_event_post_to(fast_service_loop, WIFI_SET, INIT_AP, NULL, 0, WAIT_SERVICE);     \
                        }while(0)    

#define get_weather()                                                                                           \
                        do{                                                                                     \
                            esp_event_post_to(slow_service_loop, WIFI_SET, GET_WEATHER, NULL, 0, WAIT_SERVICE); \
                        }while(0)                                                                               \

#define start_sta()                                                                                             \
                        do{                                                                                     \
                            esp_event_post_to(fast_service_loop, WIFI_SET, START_STA, NULL, 0, WAIT_SERVICE);   \
                        }while(0) 

#define stop_espnow()                                                                                               \
                        do{                                                                                         \
                            esp_event_post_to(slow_service_loop, ESPNOW_SET, STOP_ESPNOW, NULL, 0, WAIT_SERVICE);   \
                        }while(0)

#define start_espnow()                                                                                              \
                        do{                                                                                         \
                            esp_event_post_to(slow_service_loop, ESPNOW_SET, START_ESPNOW, NULL, 0, WAIT_SERVICE);    \
                        }while(0) 
#define start_sntp()                                                                                                \
                        do{                                                                                         \
                            esp_event_post_to(slow_service_loop, WIFI_SET, INIT_SNTP, NULL, 0, WAIT_SERVICE);       \
                        }while(0) 

#define stop_sntp()                                                                                                \
                        do{                                                                                        \
                            esp_event_post_to(slow_service_loop, WIFI_SET, STOP_SNTP, NULL, 0, WAIT_SERVICE);      \
                        }while(0) 
void init_dwin_events(main_data_t*);
void check_net_data_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data);
void timer_run_handler(void* data, esp_event_base_t base, int32_t key, void* event_data);

 static task_dwin_t list_services[SIZE_LIST_TASKS] = 
{
    {
        .pTask = direction_task,
        .priority = PRIORITY_DIRECTION,
        .stack = 2048
    },
    {
        .pTask = show_task,
        .priority = PRIORITY_SHOW,
        .stack = 2048
    },
    {
        .pTask = fast_services_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 12000
    },
    {
        .pTask = slow_services_task,
        .priority = PRIORITY_SLOW_SERVICE,
        .stack = 12000
    },
    {
        .pTask = uart_event_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 1024
    },
};




void vApplicationIdleHook(void );
