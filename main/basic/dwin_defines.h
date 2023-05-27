#pragma once

#include "dwin_common.h"

/*for weather_data item*/
#define weather_PIC             (weather_data->weather_pic)
#define sunrise_HOUR            (weather_data->sunrise_hour)
#define sunrise_MIN             (weather_data->sunrise_min)
#define sunset_HOUR             (weather_data->sunset_hour)
#define sunset_MIN              (weather_data->sunset_min)
#define time_ZONE               (weather_data->timezone)
#define clouds_DESCRIPTION      (weather_data->clouds)
#define PoP                     (weather_data->pop)
#define temp_FEELS_LIKE         (weather_data->feels_like)
#define temp_INDOOR             (weather_data->indoor)
#define temp_OUTDOOR            (weather_data->outdoor)
#define description_WEATHER     (weather_data->description)

/*for data_dwin instance*/
#define cur_CLOCK               (data_dwin->cur_time)
#define area_SCREEN             (data_dwin->area)
#define selected_VALUE_CLOCK    (data_dwin->cur_time[area_SCREEN])
#define cur_HOUR                (data_dwin->cur_time[INDEX_HOUR])
#define cur_MIN                 (data_dwin->cur_time[INDEX_MIN])
#define cur_MONTH               (data_dwin->cur_time[INDEX_MONTH])
#define cur_DAY                 (data_dwin->cur_time[INDEX_DAY])
#define cur_YEAR                (data_dwin->cur_time[INDEX_YEAR])
#define cur_SEC                 (data_dwin->cur_time[INDEX_SEC])
#define cur_WEEK_DAY            (data_dwin->cur_time[INDEX_WEEK_DAY])
#define pwd_WIFI                (data_dwin->pwd_wifi)
#define name_SSID               (data_dwin->ssid_name)
#define name_CITY               (data_dwin->city_name)
#define api_KEY                 (data_dwin->buf_api)
#define colors_INTERFACE        (data_dwin->colors_interface)
#define notification_DATA       (data_dwin->notif)

/*GET INPUT VALUE*/
#define GET_AREA_VALUE(key_from_dwin)             ((key_from_dwin)-KEY_START_AREA)
#define GET_AREA_VALUE_TOGGLE(key_from_dwin)      ((key_from_dwin)-KEY_START_TOGGLE_AREA)
#define GET_DAY_TOGGLE(key_from_dwin)             ((key_from_dwin)-KEY_START_TOGLE_DAY)
#define GET_DAY(key_from_dwin)                    ((key_from_dwin)-KEY_START_DAY)
#define GET_NUMBER(key_from_dwin)                 ((key_from_dwin)-'0')
#define GET_NEW_TWO_DIGIT_VALUE(old_val, in_val)  (((old_val) % 10) * 10+(in_val))
#define GET_COLOUR(item)                          (USED_COLOURS[item])
#define KEY_IS_SET_TASK(key_from_dwin)            ((key_from_dwin) >= START_SCREEN_TASKS_KEYS && \
                                                            (key_from_dwin) <= END_SCREEN_TASKS_KEYS)
#define GET_SCREEN_TASK(key_from_dwin)            ((key_from_dwin)-START_SCREEN_TASKS_KEYS)


/*NOTIFICATIONS*/
#define FIRST_MIN_MEMBER                               (SIZE_NOTIFICATION/2)
#define GET_NOTIF_HOUR(_number_notif, _day_week)       (notification_DATA[(_day_week)*NOTIF_PER_DAY+_number_notif])
#define GET_NOTIF_MIN(_number_notif, _day_week)        (notification_DATA[FIRST_MIN_MEMBER+_day_week*NOTIF_PER_DAY+_number_notif])

#define NOTIF_ACTIVE(val)                               (val < 99)
#define SET_OFF_NOTIF(val)                               do{ \
                                                            if(NOTIF_ACTIVE(val))val+=100; \
                                                        }while(0)
#define SET_ON_NOTIF(val)                               do{ \
                                                            if(!NOTIF_ACTIVE(val))val-=100; \
                                                        }while(0)
#define TOOGLE_NOTIF(val)                               do{ \
                                                            if(NOTIF_ACTIVE(val))val+=100; \
                                                            else val-=100; \
                                                        }while(0)

#define SET_NOTIF_HOUR(number_notif, day_week, value)   (notification_DATA[day_week*NOTIF_PER_DAY+number_notif] = value+100)
#define SET_NOTIF_MIN(number_notif, day_week, value)    (notification_DATA[FIRST_MIN_MEMBER + day_week*NOTIF_PER_DAY+number_notif] = value)
#define SET_NOTIF_CUR_AREA(value)                       (notification_DATA[area_SCREEN] = value+100)         

/*TIMER*/
#define timer_MIN                   (timer_data[INDEX_MIN_T])
#define timer_SEC                   (timer_data[INDEX_SEC_T])
#define timer_HOUR                  (timer_data[INDEX_HOUR_T])

/*CHECK VALUE*/
#define IS_DAY_WEEK(day_week)       ((day_week) >= 0 && (day_week) < SIZE_WEEK)
#define IS_MIN_OR_SEC(val)          ((val) > 0 && (val) <= MAX_MIN_SEC)
#define IS_MONTH(val)               ((val) > 0 && (val) <= MAX_MONTH)
#define IS_HOUR(val)                ((val) >= 0 && (val) <= MAX_HOUR)
#define IS_DAY(val)                 ((val) >= 0 && (val) <= MAX_DAY)
#define IS_YEAR(val)                ((val) >= 0 && (val) <= MAX_YEAR)

/* CHECK INPUT KEY*/
#define KEY_IS_CHAR(key_from_dwin)            ((key_from_dwin) >= '!'&& (key_from_dwin) <= '~')
#define KEY_IS_NUMBER(key_from_dwin)          ((key_from_dwin) >= '0' && (key_from_dwin) <= '9')
#define KEY_IS_AREA_SCAN_SSID(key_from_dwin)  ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+MAX_SCAN_LIST_SIZE)
#define KEY_IS_AREA_CLOCK(key_from_dwin)      ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+END_AREA_CLOCK)
#define KEY_IS_AREA_TIMERS(key_from_dwin)     ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+END_AREA_TIMERS)
#define KEY_IS_AREA_STOP_TIMER(key_from_dwin) ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+END_AREA_STOP_TIMERS)
#define KEY_IS_AREA_NOTIF(key_from_dwin)      ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+END_AREA_NOTIFICATIONS)
#define KEY_IS_AREA_CUSTOM(key_from_dwin)     ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+END_AREA_COLOURS)
#define KEY_IS_AREA_SETTING(key_from_dwin)    ((key_from_dwin) >= KEY_START_AREA && (key_from_dwin) <= KEY_START_AREA+END_AREA_SETTINGS)
#define KEY_IS_SYMBOL(key_from_dwin)          ((key_from_dwin) >= '!'&& (key_from_dwin) <= '~')
#define KEY_IS_AREA_TOGGLE(key_from_dwin)     ((key_from_dwin) >= KEY_START_TOGGLE_AREA && \
                                                (key_from_dwin) <= (KEY_START_TOGGLE_AREA+MAX_TOGGLE_UNIT_ON_SCREEN))
#define KEY_IS_AREA_TOGGLE_DAY(key_from_dwin) ((key_from_dwin) >= KEY_START_TOGLE_DAY && \
                                                (key_from_dwin) <= (KEY_START_TOGLE_DAY+SIZE_WEEK))
#define KEY_IS_DAY(key_from_dwin)             ((key_from_dwin) >= KEY_START_DAY && \
                                                (key_from_dwin) <= KEY_START_DAY+SIZE_WEEK)

#define color_CLOCK                           (USED_COLOURS[colors_INTERFACE[AREA_CLOCK_COLOUR]])
#define color_DESC                            (USED_COLOURS[colors_INTERFACE[AREA_DESC_COLOUR]])
#define color_INFO                            (USED_COLOURS[colors_INTERFACE[AREA_INFO_COLOUR]])
#define GET_COLOR_AREA(n)                     ((n) == (area_SCREEN) ? COLOUR_ENABLE : COLOUR_DISABLE)

#define GET_HEX(num)                    (num + 6 * (num / 10))
#define GET_DEC(num)                    (num - 6 * (num / 16))


#ifndef MIN
    #define MIN(a, b)((a) > (b)? (b): (a))
#endif

static const char *TAG = "dwin";

#define DWIN_CHECK(a, str, goto_tag)                                                               \
                                do {                                                               \
                                    if (ESP_OK != (a)) {                                           \
                                        ESP_LOGE(TAG, "err :%s: , %s", esp_err_to_name(a), str);   \
                                        goto goto_tag;                                             \
                                    }                                                              \
                                } while (0)

#define DWIN_SHOW_ERR(a)                                                                           \
                                do {                                                               \
                                    if (ESP_OK != (a)) {                                           \
                                        ESP_LOGE(TAG, "err :%s", esp_err_to_name(a));              \ 
                                    }                                                              \
                                } while (0)