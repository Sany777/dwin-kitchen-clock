#pragma once

#include "device_config.h"
#include "limits.h"
#include "stdint.h"
#include <freertos/FreeRTOS.h>

#define MAX_SSID_PEER_SCREEN 9
#define MAX_SCAN_LIST_SIZE (MAX_SSID_PEER_SCREEN * 2)

/* for notify */
#define NO_CLEAR_ENTRY (0UL)
#define CLEAR_EXIT (ULONG_MAX)

#define DWIN_NO_DATA (-100)
#define myEOF '\r'
/****************************/

#define SIZE_WEEK 7
#define MAX_HOUR 23
#define MAX_MIN_SEC 59
#define MAX_MONTH 12
#define MAX_DAY 31
#define MAX_YEAR 99
#define MONTH_NUMBER 12

#define MAX_dwin_data_t 249

#define BACKGROUND_COLOR WHITE

/* size buffer */
#define MAX_OTA_SIZE 1500000
/* Scratch server_buffer size */
#define SCRATCH_SIZE 5500 // >5 KB
#define MAX_URI_LEN 50
#define MAX_LEN_CHUNC_IMG 1000

#define SIZE_API 32
#define MAX_STR_LEN SIZE_API
#define SIZE_BUF (MAX_STR_LEN + 1)
#define SIZE_PWD 64
#define SIZE_BRIGHT 1
#define COLOR_INTERFACE_NUMBER 3

#define NOTIF_PER_DAY 4
#define SIZE_NOTIFICATION (SIZE_WEEK * NOTIF_PER_DAY * 2)
#define SIZE_TIMER 3
#define SIZE_BUF_TIME 7
#define MAX_SLAVE_DEVICES 4

/* UART */
#define UART_WAKEUP_THRESHOLD 3

/* buzzer */
#define NUMBER_SIG_BUZ 10
#define LOUD_BUZZER DEVICE_BUZZER_LOUD_LEVEL
#define NORMAL_BUZZER DEVICE_BUZZER_NORMAL_LEVEL

/* delays show data */
#define DELAY_SHOW_ITEM (25 / portTICK_PERIOD_MS)
#define DELAY_SHOW_MESSAGE (2000 / portTICK_PERIOD_MS)
#define DELAY_CHANGE_PIC (200 / portTICK_PERIOD_MS)

/* delay process */
#define DELAY_SCAN_SSID (1500 / portTICK_PERIOD_MS)
#define WAIT_SERVICE (7000 / portTICK_PERIOD_MS)

/* events delay*/
#define WAIT_WIFI_EVENT (1000 / portTICK_PERIOD_MS)
#define TIMEOUT_SEND_EVENTS (20 / portTICK_PERIOD_MS)
#define TICKS_TO_RUN_LOOP 100
#define TASK_EVENT_SIZE 3
#define DELAY_SLOW_LOOP (1000 / portTICK_PERIOD_MS)
#define DELAY_FAST_LOOP (50 / portTICK_PERIOD_MS)
#define DEALAY_START_TASK (1000 / portTICK_PERIOD_MS)
#define SIZE_EVENTS_UART 10
#define TIMEOUT_PUSH_KEY (100 / portTICK_PERIOD_MS)
#define DELAY_FAST_CHANGE_PIC 40
#define CLIENT_BUF_LEN 4000
#define SIZE_IP 4
#define SIZE_MAC 6
#define START_FLAG_TIMERS TIMER_DEVICE_1


#define VARIABLE_VALUE 0

#define RESIZE_KEYS_STEP 5
#define SHIFT_DT_TX 12
#define STEP_DT_TX 3
#define WAIT_PROCEES (pdMS_TO_TICKS(5000))
#define FIRST_WAIT_WIFI_BIT (pdMS_TO_TICKS(1000))
#define DELAI_UPDATE_WEATHER 900
#define DELAI_FIRST_UPDATE_WEATHER 10
#define DELAI_UPDATE_WEATHER_FAIL 300
#define ATEMPT_GET_WEATHER 4
/* Clock */
#define DELAI_UPDATE_TIME_ON_SCREEN 60
#define TIME_ZONA_FORMAT "EET+2EEST,M3.5.0/3,M10.5.0/4"
#define SYNCH_10_HOUR 36000000
#define SYNC_15_MIN 900000
#define MIN_BEFORE_NOTIFICATION 5

#define SIZE_UART_EVENTS 10
#define SIZE_SHOW_BUF 250
#define MAX_DATA_LEN (SIZE_SHOW_BUF - 1)
/* events timer */
#define DELAY_AUTOCLOSE 120
#define WAIT_SHOW 100
#define WAIT_set_new_command 100
#define MAX_AP_PER_PAGE 9

#define SIZE_QUEUE_DIRECT 10

static const uint8_t MONTH_DAY[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* weather service*/
#define NUMBER_DATA_WEATHER 5
#define MAX_LEN_DESCRIPTION 20
#define NUMBER_ITEM_WEATHER 5
#define LEN_BUF_DESCRIPTION (MAX_LEN_DESCRIPTION + 1)
#define FIRST_URL "https://api.openweathermap.org/data/2.5/forecast?q="
#define SECOND_URL "&units=metric&cnt=5&appid="
#define SIZE_URL_BUF (sizeof(FIRST_URL) + sizeof(SECOND_URL) + MAX_STR_LEN + MAX_STR_LEN + 1)

#define SIMPLE_PRIVAT_API "https://api.privatbank.ua/p24api/pubinfo?exchange&coursid=5"
#define SIZE_SIMPLE_PRIVAT_API sizeof(SIMPLE_PRIVAT_API)
#define GET_CURRENCY_HOUR 9
#define CACHED_SCREEN_DATA_MAX_AGE_US (15LL * 60 * 60 * 1000000)
