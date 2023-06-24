#pragma once

#include "dwin_config.h"

/*COLORS*/
#define GREEN  0x076E
#define AQUA   0x74FF
#define YELLOW 0xFFF0
#define ORANGE 0xFD8F
#define WHITE  0xFFFF
#define BLACK  0x0000
#define CAEN   0x0410
#define BLUE   0x001F
#define GREY   0xC618
#define VIOLET 0x841F
#define LEMON  0x07E0
#define RED    0xF800

#define NUMBER_COLORS END_AREA_COLORS

#define SIZE_USED_COLORS 10
static const uint16_t USED_COLORS[SIZE_USED_COLORS] = {
    WHITE, 
    CAEN, 
    BLUE, 
    GREY, 
    VIOLET, 
    LEMON, 
    RED, 
    GREEN, 
    AQUA, 
    ORANGE
};
static const char* COLOR_NAME[] = { 
    "WHITE", 
    "CAEN",
    "BLUE", 
    "GREY",
    "VIOLET", 
    "LEMON", 
    "RED", 
    "GREEN",
    "AQUA",
    "ORANGE"
};
#define COLOR_ENABLE  RED
#define COLOR_DISABLE WHITE


/*AREAS ON THE SCREEN*/
typedef enum area_clock{
    AREA_YEAR,
    AREA_MONTH,
    AREA_DAY,
    AREA_HOUR,
    AREA_MIN,
    AREA_SEC,
    END_AREA_CLOCK,
} area_clock_t;

typedef enum area_timer{
    AREA_TIMER_HOUR,
    AREA_TIMER_MIN,
    AREA_TIMER_SEC,
    END_AREA_TIMERS
} area_stop_timer_t;

typedef enum area_notifications{
    AREA_HOUR_1,
    AREA_MIN_1,
    AREA_HOUR_2,
    AREA_MIN_2,
    AREA_HOUR_3,
    AREA_MIN_3,
    AREA_HOUR_4,
    AREA_MIN_4,
    END_AREA_NOTIFICATIONS
} area_notifications_t;

typedef enum area_custom{
    AREA_CLOCK_COLOR,
    AREA_DESC_COLOR,
    AREA_INFO_COLOR,
    END_AREA_COLORS
} area_custom_t;

typedef enum {
    KEY_NOT_1_TOGGLE,
    KEY_NOT_2_TOGGLE,
    KEY_NOT_3_TOGGLE,
    KEY_NOT_4_TOGGLE,
    KEY_NOT_CURRENT_DAY_TOGGLE,
    END_AREA_TOGGLE_NOTIF_STATE
} area_toggle_notify_t;

typedef enum area_settings{
    AREA_SSID,
    AREA_PASSWORD,
    AREA_CITY,
    AREA_API,
    END_AREA_SETTINGS
} area_settings_t;


/* screen keys are linked with screens_handlers list */
typedef enum Main_buttons{
    MAIN_SCREEN = 1,
    CLOCK_SCREEN,
    SEARCH_SSID_SCREEN,
    SETTING_SCREEN,
    SET_COLOR_SCREEN,
    NOTIFICATION_SCREEN,
    SERVER_SCREEN,
    STATE_DEVICE_SCREEN,
    TIMER_SCREEN,
    INFO_SCREEN,
    ESPNOW_DEVICES_SCREEN,
    END_KEY_SCREEN_TASK
}task_key_t;

#define START_SCREEN_TASK MAIN_SCREEN

/* AREA KEYS*/
#define KEY_START_AREA                  0X40
#define MAX_AREA_UNIT_ON_SCREEN         20
#define END_KEY_AREA                    (KEY_START_AREA+MAX_AREA_UNIT_ON_SCREEN)
/* DAY KEYS */
#define KEY_DAY_1                       0X60
#define END_KEY_DAY                     (KEY_DAY_1+SIZE_WEEK)
/* TOGGLE DAY KEYS */
#define KEY_TOGGLE_DAY_1                0X70
#define END_KEY_TOGGLE_DAY              (KEY_TOGGLE_DAY_1+SIZE_WEEK)  
/* TOGGLE KEYS */
#define KEY_TOGGLE_1                    0X20
#define MAX_TOGGLE_UNIT_ON_SCREEN       20
#define END_KEY_TOGGLES                 (KEY_TOGGLE_1+MAX_TOGGLE_UNIT_ON_SCREEN)

typedef enum {
    KEY_ESPNOW_TOGGLE = KEY_TOGGLE_1,
    KEY_SECURITY_TOGGLE,
    KEY_SOUND_TOGGLE ,
    KEY_SNTP_TOGGLE,
    END_AREA_STATE
} area_toggle_state_t;

/* KEY BUTTON */
enum buttons_and_events{
    KEY_DELETE = 0x80,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_SYNC,
    KEY_DEINIT,
    KEY_INIT,
    KEY_NEXT,
    KEY_STOP,
    KEY_PAUSA,
    KEY_CLOSE,
    KEY_INCREMENT,
    KEY_DECREMENT,
    KEY_DETAILS_SCREEN,
    KEY_SETTING_SCREEN_LOW,
    KEY_SETTING_SCREEN_SYMB,
    KEY_MENU_SCREEN,
    KEY_SETTING_SCREEN_UP,
    UPDATE_WEATHER_COMPLETE,
    UPDATE_TIME_COMPLETE,
    UPDATE_DATA_COMPLETE,
    STATION_JOINE,
    KEY_END_COMMON_DWIN_BUTTONS,
    START_SERVICE_EVENTS,
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
    STOP_SNTP,
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
    GET_TEMPERATURE,
    UPDATE_TIME_FROM_UART,
    UPDATE_TIME_FROM_ETHER,
    UPDATE_TIME_FROM_MS,
    SHOW_TIME,
    GET_DATA_FROM_DEVICE,
    DATA_SHOW,
    TIMER_RUN,
    STATION_LEAVE,
    DETAILS_SCREEN,
    NORMAL_SCREEN,
    IS_NOTIFICATION,
    GET_REMOTE_SENSOR,
    CHECK_NET_DATA,
    IS_NOTIFY, 
    NOT_NOTIFY,
    END_SERVICE_EVENTS
};

/* PIC INDEX */
typedef enum{
    NO_WEATHER_PIC,
    SETTING_LOW_LETTER_PIC,
    SETTING_UP_LETTER_PIC,
    SETTING_SYMBOL_PIC,
    CLOCK_PIC,
    TIMER_STOP_PIC,
    TIMER_RUN_PIC,
    WEAT_CLOUD_MEDIUM_PIC,
    WEAT_CLOUD_SMALL_NIGHT_PIC,
    WEAT_CLOUD_SMALL_DAY_PIC,
    WEAT_CLOUD_HEAVY_PIC,
    WEAT_FOG_PIC,
    WEAT_RAIN_PIC,
    WEAT_RAIN_MOON_PIC,
    WEAT_RAIN_SUN_PIC,
    WEAT_SNOW_PIC,
    WEAT_THUNDER_PIC,
    WEAT_CLEAR_NIGHT_PIC,
    WEAT_CLEAR_DAY_PIC,
    SEARCH_PIC,
    NOTIF_PIC,
    CUSTOM_COLORS_PIC,
    MENU_PIC,
    INFO_PIC,
    END_LIST_PIC
} picture_t;

