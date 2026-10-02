#pragma once

#include "dwin_config.h"
#include "esp_event_base.h"
#include "screen_keys.h"
#include <stdint.h>
#include <sys/queue.h>
#include <time.h>


typedef struct {
    int32_t command;
    uint8_t data[64];
} show_queue_data_t;

typedef enum { NO_CHANGE, GO_UP, GO_DOWN } currency_state_t;

/* time function */
enum mode_time_func {
    ONLY_ONCE,
    RELOAD_COUNT,
};

enum {
    SOUNDS_ALLOW,
    UNUSED_WIFI_PEER_FLAG,
    SNTP_ALLOW,
    SECURITY,
    WEATHER_OK,
    CON_STA_OK,
    UNUSED_SENSOR_1_FLAG,
    UNUSED_SENSOR_2_FLAG,
    RESPONSE_400_SERVER,
    SNTP_WORK,
    PROCESS_DWIN,
    UNUSED_WIFI_PEER_CONNECTED,
    IS_TIME,
    RESPONSE_OK,
    RESPONSE_UPDATE,
    WORK_AP,
    SSID_FOUND,
    UNUSED_WIFI_PEER_RUNNING,
    IS_NIGHT,
    UNUSED_LOCAL_SENSOR_FLAG,
    WRONG_API_KEY
};

/*events bit*/
#define BIT_WRONG_API_KEY (1 << WRONG_API_KEY)
#define BIT_RESPONSE_400_SERVER (1 << RESPONSE_400_SERVER)
#define BIT_SOUNDS_ALLOW (1 << SOUNDS_ALLOW)
#define BIT_SNTP_ALLOW (1 << SNTP_ALLOW)
#define BIT_NIGHT (1 << IS_NIGHT)
#define BIT_SECURITY (1 << SECURITY)
#define BIT_CON_STA_OK (1 << CON_STA_OK)
#define BIT_SSID_FOUND (1 << SSID_FOUND)
#define BIT_DENIED_STA (1 << WORK_AP)
#define BIT_SNTP_WORK (1 << SNTP_WORK)
#define BIT_TIMER_RUN (1 << TIMER_RUN)
#define BIT_WEATHER_OK (1 << WEATHER_OK)
#define BIT_IS_TIME (1 << IS_TIME)
#define BIT_DWIN_RESPONSE_OK (1 << RESPONSE_OK)
#define BIT_PROCESS (1 << PROCESS_DWIN)
#define STORED_FLAGS (BIT_SOUNDS_ALLOW | BIT_SNTP_ALLOW | BIT_SECURITY)

typedef enum {
    INDEX_HOUR_T,
    INDEX_MIN_T,
    INDEX_SEC_T,
} index_timer_t;

typedef enum {
    DATA_PWD,
    DATA_API,
    DATA_COLOR,
    DATA_CITY,
    DATA_SSID,
    DATA_NOTIF,
    DATA_FLAGS,
    END_DATA_IDENTEFIER_FOR_CICLE,
} data_identification_t;

typedef struct {
    uint8_t weather_pic;
    uint8_t sunrise_hour;
    uint8_t sunrise_min;
    uint8_t sunset_hour;
    uint8_t sunset_min;
    /*receive time*/
    uint8_t dt_tx;
    float outdoor;
    char description[LEN_BUF_DESCRIPTION];
    uint8_t pop[NUMBER_DATA_WEATHER];
    float feels_like[NUMBER_DATA_WEATHER];
    int64_t updated_at_us;
} weather_data_t;

/* data struct timer func*/
typedef struct periodic_event {
    size_t time;
    size_t time_init;
    uint16_t command;
    int mode;
} periodic_event_t;

typedef struct timer_data {
    uint8_t mac[SIZE_MAC];
    char *name;
    int8_t *time_buf;
    uint8_t num_week;
} timer_data_t;

typedef struct {
    float usd_bay;
    float usd_sale;
    float eur_bay;
    float eur_sale;
    int64_t updated_at_us;
    currency_state_t usd_state;
    currency_state_t eur_state;
} currency_t;

typedef struct {
    uint8_t area;
    char pwd_wifi[SIZE_PWD];
    char ssid_name[SIZE_BUF];
    char city_name[SIZE_BUF];
    char buf_api[SIZE_BUF];
    uint8_t colors_interface[COLOR_INTERFACE_NUMBER];
    uint8_t notif_data[SIZE_NOTIFICATION];
    int8_t timer_data[SIZE_TIMER];
    timer_data_t *timers;
    weather_data_t weather_data;
    currency_t cur_value;
} dwin_data_t;

typedef void (*dwin_screen_handler_t)(dwin_data_t *, uint8_t, char);
typedef void (*dwin_show_handler_t)(const dwin_data_t *, int32_t, void *);

typedef struct {
    TaskFunction_t pTask;
    UBaseType_t priority;
    uint32_t stack;
} task_dwin_t;

typedef struct {
    dwin_screen_handler_t main_handler;
    dwin_show_handler_t show_handler;
} handlers_dwin_t;
