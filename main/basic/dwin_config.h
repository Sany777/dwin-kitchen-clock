#pragma once

#include "driver/uart.h"
#include "limits.h"


#define MAX_SCAN_LIST_SIZE 10

/* for notify */
#define NO_CLEAR_ENTRY     (0UL)
#define CLEAR_EXIT         (ULONG_MAX)

#define NO_DATA_DWIN  0
#define STOP 0
#define RUN  1
#define ON           true
#define OFF          false
#define myEOF               0xFF
/****************************/

#define SIZE_WEEK    7
#define SIZE_TIME    7
#define MAX_HOUR     23
#define MAX_MIN_SEC  59
#define MAX_MONTH    12
#define MAX_DAY      31
#define MAX_YEAR     99
#define MONTH_NUMBER 12

#define MAX_DWIN_DATA 249


#define BACKGROUND_COLOUR WHITE

/* size buffer */

#define ESP_WIFI_CHANNEL   1
#define MAX_OTA_SIZE  1500000
/* Scratch server_buffer size */
#define SCRATCH_SIZE  1000 // 2 KB
#define MAX_URI_LEN 50
#define MAX_LEN_CHUNC_IMG 1000

#define SIZE_API            32
#define MAX_STR_LEN         SIZE_API
#define SIZE_BUF            (MAX_STR_LEN+1)
#define SIZE_BRIGHT         1
#define SIZE_COLOURS_INTERFACE           3

#define NOTIF_PER_DAY 4
#define SIZE_NOTIFICATION   (SIZE_WEEK*NOTIF_PER_DAY*2)
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
#define WAIT_RESPONSE_SERVICE    (5000/portTICK_PERIOD_MS)

/* events delay*/
#define DELAY_WIFI_EVENT         (1000/portTICK_PERIOD_MS)
#define TIMEOUT_SEND_EVENTS      (100/portTICK_PERIOD_MS)
#define TICKS_TO_RUN_LOOP         100
#define TASK_EVENT_SIZE           3
#define DELAY_TASK_LOOP          (10/portTICK_PERIOD_MS)
#define SIZE_EVENTS_UART          10
#define TIMEOUT_PUSH_KEY         (50/portTICK_PERIOD_MS)

#define ESPNOW_MAXDELAY          512

#define ESPNOW_QUEUE_SIZE           5


#define TIMER_WAKEUP_LONG_TIME_US    (60 * 1000 * 1000UL)
#define TIMER_WAKEUP_SHORT_TIME_US   (10 * 1000UL)

#define WINDOW_ESPNOW_MS 65000

#define SIZE_IP 4
#define SIZE_MAC 6
#define SIZE_BUF_INFO_SEND 18
#define START_FLAG_SENSORS SENSOR_1_OK
#define START_FLAG_TIMERS TIMER_DEVICE_1


#define SIZE_MAC_STR 17
#define MAX_NAME_DEVICE 20

#define MY_DEVICE_TYPE  DWIN_SCREEN_DEVICE
#define MY_DEVICE_NAME "dwinScreen"
#define NUMBER_SENSOR 2

#define VARIABLE_VALUE '\0'
