#pragma once

#include "driver/uart.h"
#include "limits.h"


#define MAX_SCAN_LIST_SIZE   20
#define MAX_SSID_PEER_SCREEN 10

/* for notify */
#define NO_CLEAR_ENTRY     (0UL)
#define CLEAR_EXIT         (ULONG_MAX)

#define NO_main_data  0
#define STOP 0
#define RUN  1
#define ON           true
#define OFF          false
#define myEOF               0xFF
/****************************/

#define SIZE_WEEK    7
#define MAX_HOUR     23
#define MAX_MIN_SEC  59
#define MAX_MONTH    12
#define MAX_DAY      31
#define MAX_YEAR     99
#define MONTH_NUMBER 12

#define MAX_DWIN_DATA 249


#define BACKGROUND_COLOR WHITE

/* size buffer */

#define ESP_WIFI_CHANNEL   0
#define MAX_OTA_SIZE  1500000
/* Scratch server_buffer size */
#define SCRATCH_SIZE  1000 // 2 KB
#define MAX_URI_LEN 50
#define MAX_LEN_CHUNC_IMG 1000

#define SIZE_API            32
#define MAX_STR_LEN         SIZE_API
#define SIZE_BUF            (MAX_STR_LEN+1)
#define SIZE_BRIGHT         1
#define SIZE_COLORS_INTERFACE           3

#define NOTIF_PER_DAY 4
#define SIZE_NOTIFICATION       (SIZE_WEEK*NOTIF_PER_DAY*2)
#define SIZE_BUF_NOTIFICATION   (SIZE_NOTIFICATION)
#define SIZE_TIMER          3
#define SIZE_BUF_TIME       7
#define MAX_SLAVE_DEVICES   4


/* UART */
#define UART_BUF_SIZE 255
#define UART_DWIN               0

#define UART_WAKEUP_THRESHOLD   3
#define U0RXD_GPIO_NUM 1
#define U0TXD_GPIO_NUM 3

#define TXD_PIN (U0RXD_GPIO_NUM)
#define RXD_PIN (U0TXD_GPIO_NUM)

/* buzzer */
#define NUMBER_SIG_BUZ       10
#define LOUD_BUZZER          70
#define QUIET_BUZZER         30

/* delays show data */
#define DELAY_SHOW_ITEM          (75/portTICK_PERIOD_MS)
#define DELAY_SHOW_MESSAGE       (2000/portTICK_PERIOD_MS)
#define DELAY_CHANGE_PIC         (50/portTICK_PERIOD_MS)

/* delay process */
#define DELAY_SCAN_SSID          (1500/portTICK_PERIOD_MS)
#define WAIT_SERVICE    (7000/portTICK_PERIOD_MS)

/* events delay*/
#define WAIT_WIFI_EVENT          (1000/portTICK_PERIOD_MS)
#define TIMEOUT_SEND_EVENTS      (100/portTICK_PERIOD_MS)
#define TICKS_TO_RUN_LOOP         100
#define TASK_EVENT_SIZE           3
#define DELAY_SLOW_LOOP          (500/portTICK_PERIOD_MS)
#define DELAY_FAST_LOOP          (20/portTICK_PERIOD_MS)
#define DEALAY_START_TASK        (1000/portTICK_PERIOD_MS)
#define DEALAY_START_ESPNOWTASK  (4000/portTICK_PERIOD_MS)
#define SIZE_EVENTS_UART          10
#define TIMEOUT_PUSH_KEY         (100/portTICK_PERIOD_MS)

#define ESPNOW_MAXDELAY          1000

#define ESPNOW_QUEUE_SIZE         7


#define TIMER_WAKEUP_LONG_TIME_US    (10 * 1000)
#define TIMER_WAKEUP_SHORT_TIME_US   (10 * 1000UL)

#define WINDOW_ESPNOW_MS 65000
#define CLIENT_BUF_LEN 4000
#define SIZE_IP 4
#define SIZE_MAC 8
#define SIZE_BUF_INFO_SEND 18
#define START_FLAG_SENSORS SENSOR_1_OK
#define START_FLAG_TIMERS TIMER_DEVICE_1


#define SIZE_MAC_STR 17
#define MAX_NAME_DEVICE 20

#define MY_DEVICE_TYPE  DWIN_SCREEN_DEVICE
#define MY_DEVICE_NAME "dwinScreen2"
#define NUMBER_SENSOR 2

#define VARIABLE_VALUE '\0'

/* AP */
#define AP_WIFI_SSID      "NASA"
#define AP_WIFI_PWD      "dwinSecond"
#define MY_IP "http://192.168.4.1"

/* weather service*/
#define NUMBER_DATA_WEATHER 5
#define MAX_LEN_DESCRIPTION 10
#define LEN_BUF_DESCRIPTION (MAX_LEN_DESCRIPTION+1)
#define SIZE_URL_BUF 250
#define FIRST_URL "https://api.openweathermap.org/data/2.5/forecast?q="
#define SECOND_URL "&units=metric&cnt=5&appid="
#define INITIAL_SIZE_LIST_KEYS 10
#define SHIFT_DT_TX 12
#define STEP_DT_TX  3
#define WAIT_PROCEES (pdMS_TO_TICKS(5000))
#define FIRST_WAIT_WIFI_BIT  (pdMS_TO_TICKS(1000))
#define DELAI_UPDATE_WEATHER 900
#define DELAI_FIRST_UPDATE_WEATHER 10
#define DELAI_UPDATE_WEATHER_FAIL 600
/* Clock */
#define DELAI_UPDATE_TIME_ON_SCREEN 60
#define FORMAT_CLOCK_DEFAULT "EET-2EEST,M3.5.0/3,M10.5.0/4"
#define SYNCH_10_HOUR 36000000
#define SYNC_15_MIN 900000

#define SIZE_BUF_FORMAT_CLOCK (sizeof(FORMAT_CLOCK_DEFAULT)+2)

#define SIZE_UART_EVENTS 5
#define SIZE_SHOW_BUF 250

#define DELAY_CHANGE_CNTX 100