#pragma once

#include "dwin_common.h"



#define PRIORITY_DIRECTION 5
#define PRIORITY_SHOW    (PRIORITY_DIRECTION-1)
#define PRIORITY_SERVICE (PRIORITY_DIRECTION-1)
#define PRIORITY_MANAGER (PRIORITY_DIRECTION-2)

#define sleep_dwin(time_us)                                                                 \
                                do{ stop_event_timer();                                     \
                                    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(time_us));\
                                    uart_wait_tx_idle_polling(UART_DWIN);                  \
                                    ESP_ERROR_CHECK(esp_light_sleep_start());               \
                                }while(0)


enum {
    CPU_0,
    CPU_1
};

typedef enum
{
    PUSH_KEY,
    SELECT_TASK,
    START_SERVICE,
    UPDATE_TIME_FROM_UART,
    UPDATE_TIME_FROM_SNTP,
    UPDATE_TIME_FROM_MS,
    DATA_SHOW
} command_id_t;

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
    HELP_TASK,
    SHOW_TASK,
    UART_TASK,
    SERVICE_CONECTION,
    END_LIST_TASKS
} identefier_system_task_t;

#define SIZE_LIST_TASKS    MAIN_HANDLERS_LIST_END
#define SIZE_LIST_SERVICES 2

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
extern  dwin_handler_t server_handler;
extern  dwin_handler_t device_screen_handler;
extern  dwin_handler_t main_screen_handler;
extern  dwin_handler_t clock_handler;
extern  dwin_handler_t set_color_screen_handler;
extern  dwin_handler_t notification_screen_handler;
extern  dwin_handler_t timer_screen_handler;
extern  dwin_handler_t set_time_handler;

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
 static handlers_dwin_t links_handlers_list[SIZE_LIST_TASKS] = {
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
    //     .main_handler    = server_handler,
    //     .show_handler    = NULL,
    // },
};


/* loop task */
void show_task(void*);
void direction_task(void *);
void services_task(void*);
void uart_event_task(void *);

void init_dwin_events(main_data_t*);


 static task_dwin_t list_services[SIZE_LIST_SERVICES] = 
{
    // {
    //     .pTask = direction_task,
    //     .priority = 5,
    //     .stack = 7000
    // },
    {
        .pTask = services_task,
        .priority = 5,
        .stack = 12000
    },
    // {
    //     .pTask = show_task,
    //     .priority = 3,
    //     .stack = 2048
    // },
    {
        .pTask = uart_event_task,
        .priority = 5,
        .stack = 2048
    },
};

 static esp_event_loop_handle_t 
                loop_direct, 
                loop_show, 
                loop_service;

// ESP_EVENT_DECLARE_BASE(EVENTS_SHOW);

static ESP_EVENT_DEFINE_BASE(EVENTS_SHOW);


static ESP_EVENT_DEFINE_BASE(EVENTS_SERVICE);
// ESP_EVENT_DECLARE_BASE(EVENTS_SERVICE); 

static ESP_EVENT_DEFINE_BASE(EVENTS_DIRECTION);
ESP_EVENT_DECLARE_BASE(EVENTS_DIRECTION); 

static ESP_EVENT_DEFINE_BASE(EVENTS_MANAGER);
ESP_EVENT_DECLARE_BASE(EVENTS_MANAGER); 

static ESP_EVENT_DEFINE_BASE(EVENTS_SET_TIME);
ESP_EVENT_DECLARE_BASE(EVENTS_SET_TIME); 


static ESP_EVENT_DEFINE_BASE(WIFI_SET);


void vApplicationIdleHook(void );