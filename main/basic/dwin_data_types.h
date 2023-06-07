#pragma once

#include <stdint.h>
#include <sys/queue.h>
#include "esp_event_base.h"

#include "screen_keys.h"
#include "dwin_config.h"


typedef void dwin_handler_t (void*, esp_event_base_t, int32_t, void*);

typedef int temperature_t;

typedef enum espnow_action{
    TRY_AGAIN,
    GIVE_NOT,
    GIVE_TIME,
    GIVE_TIMERS,
    GIVE_DIVICE_INFO,
    GIVE_NETWORK,
    RESPONSE_ADD_NEW,
    REQUEST_ADD_NEW,
    NEED_TIME,
    NEED_TEMP,
    NEED_NETWORK,
    NEED_DEVICE_INFO,
} espnow_action_t;

typedef enum type_devic{
    SENSOR_TEMP_DEVICE,
    TIMER_DEVICE,
    DWIN_SCREEN_DEVICE,
}type_device_t;

/* time function */
typedef enum mode_time_fun{
    ONLY_ONCE,
    RELOAD_COUNT,
} mode_time_func_t;


enum events{
    START_STA,
    INIT_AP,
    CLOSE_CUR_CON,
    INIT_SCAN_NETWORKS,
    DEINIT_SCAN_NETWORKS,
    START_SCAN_NETWORKS,
    START_ESPNOW,
    PAUSE_ESPNOW,
    STOP_ESPNOW,
    UPDATE_TIME,
    INIT_SNTP,
    INIT_ESPNOW,
    STOP_WIFI,
    SEND_RESULT,
    SET_CONFIG,
    SET_MODE_STA,
    GET_WEATHER,
    SHOW_SCREEN,
    PUSH_KEY,
    SELECT_TASK,
    START_SERVICE,
    UPDATE_TIME_FROM_UART,
    UPDATE_TIME_FROM_ETHER,
    UPDATE_TIME_FROM_MS,
    SHOW_TIME,
    GET_DATA_FROM_DEVICE,
    DATA_SHOW,
    TIMER_SHOW,
    UPDATE_DATA_COMPLETE,
    STATION_JOINE,
    STATION_LEAVE,
};

typedef enum flag_state_device{
    SOUNDS_ALLOW,
    ESPNOW_ALLOW,
    SNTP_ALLOW,
    SECURITY,
    WEATHER_OK,
    CON_STA_OK,
    SENSOR_1_OK,
    SENSOR_2_OK,

    SNTP_WORK,
    PROCESS_DWIN,
    ESPNOW_CONECT,
    IS_TIME,
    TIMER_RUN,
    RESPONSE_OK,
    RESPONSE_UPDATE,
    WORK_AP,
    SSID_FOUND,
    ESPNOW_RUN,
}flag_state_device_t;

/*events bit*/
#define BIT_SOUNDS_ALLOW        ( 1 << SOUNDS_ALLOW )
#define BIT_ESPNOW_ALLOW        ( 1 << ESPNOW_ALLOW )
#define BIT_SYNC_TIME_ALLOW     ( 1 << SNTP_ALLOW )
#define BIT_SECURITY            ( 1 << SECURITY )
#define BIT_CON_STA_OK          ( 1 << CON_STA_OK )
#define BIT_ESPNOW_RUN          ( 1 << ESPNOW_RUN )
#define BIT_SSID_FOUND          ( 1 << SSID_FOUND  )
#define BIT_WORK_AP             ( 1 << WORK_AP )
#define BIT_SNTP_WORK           ( 1 << SNTP_WORK )
#define BIT_TIMER_RUN           ( 1 << TIMER_RUN )
#define BIT_WEATHER_OK          ( 1 << WEATHER_OK )
#define BIT_IS_TIME             ( 1 << IS_TIME )

#define BIT_ESPNOW_CONECT       ( 1 << ESPNOW_CONECT )
#define BIT_DWIN_RESPONSE_OK    ( 1 << RESPONSE_OK )
#define BIT_PROCESS             ( 1 << PROCESS_DWIN )

#define BIT_INTERNET            ( 1 << INTERNET_OK )
#define BIT_SEN_1               ( 1 << SENSOR_1_OK )
#define BIT_SEN_2               ( 1 << SENSOR_2_OK )
#define STORED_FLAGS            (1UL|BIT_SOUNDS_ALLOW|BIT_SYNC_TIME_ALLOW|BIT_ESPNOW_ALLOW|BIT_SECURITY )

#define NUMBER_STORED_FLAGS 4

typedef enum index_timer {
    INDEX_HOUR_T,
    INDEX_MIN_T,
    INDEX_SEC_T, 
}index_timer_t;

typedef enum data_identification{
	DATA_PWD,
	DATA_API,
	DATA_COLOUR,
	DATA_CITY,
	DATA_SSID,
	DATA_NOTIF,
	DATA_FLAGS,
	END_DATA_IDENTEFIER_FOR_CICLE,
    DATA_OFFSET,
} data_identification_t;

/*package espnow*/
typedef struct sensor_package{
    uint16_t crc;
    temperature_t temperature;
    int humidity;
    time_t wakeup;
}__attribute__((packed))sensor_package_t;

typedef struct{
    uint16_t crc;
    uint8_t timers[0];
}__attribute__((packed))timer_package_t;

typedef struct network_package{
    uint16_t crc;
    char ssid[SIZE_BUF];
    char pwd[SIZE_BUF];
} __attribute__((packed))network_package_t;

typedef struct time_package{
    uint16_t crc;
    uint16_t time;
} __attribute__((packed))time_package_t;

typedef struct  action_packag{
    uint16_t crc;
    uint8_t action;  
    uint8_t mac[8];           
} __attribute__((packed)) action_package_t;

typedef struct hello_package{
    uint8_t action;
} __attribute__((packed)) hello_package_t;

typedef struct device_espnow {
    uint16_t crc;
    uint8_t type;
    char name[MAX_NAME_DEVICE+1];
} __attribute__((packed)) device_info_package_t;



typedef struct {
    uint8_t weather_pic;
    uint8_t sunrise_hour;
    uint8_t sunrise_min;
    uint8_t sunset_hour;
    uint8_t sunset_min;
    uint8_t timezone;
    char description[LEN_BUF_DESCRIPTION];
    uint8_t pop[NUMBER_DATA_WEATHER];
    uint8_t dt_tx;
    temperature_t indoor;
    temperature_t feels_like[NUMBER_DATA_WEATHER];
    temperature_t outdoor[NUMBER_DATA_WEATHER];
} weather_data_t;

/* data struct timer func*/
typedef struct periodic_event {
    size_t time;
    size_t time_init;
    uint32_t event_id;
    mode_time_func_t mode;
    esp_event_base_t base;
    esp_event_loop_handle_t event_loop;
}periodic_event_t;

typedef struct {
    uint8_t mac[SIZE_MAC];
    bool is_brodcast_dst;
    uint8_t rssi;
    uint8_t *parcel;
    uint32_t parcel_len;
}espnow_rx_data_t;


typedef struct espnow_send{
    uint8_t mac[SIZE_MAC];
    uint8_t action;
} espnow_send_t;

typedef struct cur_device_espnow {
    uint8_t mac[SIZE_MAC];
    char *name;
    type_device_t type;
}cur_device_inf_t;

typedef struct device_inf {
    SLIST_ENTRY(device_inf)next;
    uint8_t mac[SIZE_MAC];
    uint8_t type;
    char *name;
} device_inf_t;

typedef struct sensor_data{
    temperature_t temperature;
    int humidity;
    uint8_t mac[SIZE_MAC];
    char name[0];
} __attribute__((packed))sensor_data_t;



typedef struct {
    uint8_t area;
    char pwd_wifi[SIZE_BUF];
    char ssid_name[SIZE_BUF];
    char city_name[SIZE_BUF];
    char buf_api[SIZE_BUF];
    uint8_t colors_interface[SIZE_COLOURS_INTERFACE];
    uint8_t *notif_data;
    weather_data_t *weather_data;
    sensor_data_t *sensor_data[NUMBER_SENSOR];
    struct tm *time;
} main_data_t;
