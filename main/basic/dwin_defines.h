#pragma once

#include "dwin_common.h"

/*for weather_data item*/
#define weather_PIC             (main_data->weather_data.weather_pic)
#define sunrise_HOUR            (main_data->weather_data.sunrise_hour)
#define sunrise_MIN             (main_data->weather_data.sunrise_min)
#define sunset_HOUR             (main_data->weather_data.sunset_hour)
#define sunset_MIN              (main_data->weather_data.sunset_min)
#define time_ZONE               (main_data->weather_data.timezone)
#define PoP                     (main_data->weather_data.pop)
#define temp_FEELS_LIKE         (main_data->weather_data.feels_like)
#define dt_TX                   (main_data->weather_data.dt_tx)
#define temp_INDOOR             (main_data->sensor_data)
#define temp_INDOOR_0           (main_data->sensor_data[0].tem)
#define temp_INDOOR_1           (main_data->sensor_data[1].tem)
#define temp_OUTDOOR            (main_data->weather_data.outdoor)
#define description_WEATHER     (main_data->weather_data.description)
#define sensor_DATA            (main_data->sensor_data)
/*for main_data instance*/
#define area_SCREEN             (main_data->area)
#define pwd_WIFI                (main_data->pwd_wifi)
#define name_SSID               (main_data->ssid_name)
#define name_CITY               (main_data->city_name)
#define api_KEY                 (main_data->buf_api)
#define colors_INTERFACE        (main_data->colors_interface)
#define notification_DATA       (main_data->notif_data)

/*GET INPUT VALUE*/
#define GET_AREA_VALUE(key_from_dwin)             ((key_from_dwin)-KEY_START_AREA)

#define GET_VALUE_TOGGLE(key_from_dwin)      ((key_from_dwin)-KEY_TOGGLE_1)

#define GET_DAY_TOGGLE(key_from_dwin)             ((key_from_dwin)-KEY_TOGGLE_DAY_1)

#define GET_DAY(key_from_dwin)                    ((key_from_dwin)-KEY_DAY_1)

#define GET_NUMBER(key_from_dwin)                 ((key_from_dwin)-'0')

#define GET_NEW_MIN_SEC_VALUE(old_val, in_val)    (old_val<6 ? old_val*10+in_val : in_val)

#define GET_NEW_HOUR_VALUE(old_val, in_val)       ((old_val*10) + in_val < 24 ? old_val*10+in_val: in_val)

#define GET_COLOR(item)                           (USED_COLORS[item])

#define KEY_IS_SET_TASK(key_from_dwin)            ((key_from_dwin) >= START_SCREEN_TASK && \
                                                        (key_from_dwin) < END_KEY_SCREEN_TASK)

#define GET_SCREEN_TASK(key_from_dwin)            ((key_from_dwin)-START_SCREEN_TASK)

/*NOTIFICATIONS*/
#define GET_NOTIF_HOUR(_number_notif, _day_week)       (notification_DATA[(_day_week)*(NOTIF_PER_DAY)*2+(_number_notif)*2])

#define VALUE_NOTIF_HOUR(_number_notif, _day_week)     (GET_NOTIF_HOUR(_number_notif, _day_week)%100)

#define VALUE_NOTIF_MIN(_number_notif, _day_week)      (notification_DATA[(_day_week)*(NOTIF_PER_DAY)*2+(_number_notif)*2+1])

#define IS_NOTIF_ACTIVE(_notif, _day)                  (GET_NOTIF_HOUR(_notif, _day) < 99)

#define IS_DAY_ACTIVE(_day)                            (IS_NOTIF_ACTIVE(0, _day)            \
                                                            || IS_NOTIF_ACTIVE(1, _day)     \
                                                            || IS_NOTIF_ACTIVE(2, _day)     \
                                                            || IS_NOTIF_ACTIVE(3, _day))  

#define GET_NOTIF_NUMBER(_area)                         ((_area)/2)

#define GET_NOTIF_TYPE_DATA(_area)                      ((_area)%2 ? AREA_MIN : AREA_HOUR)

#define SET_OFF_DAY_NOTIF(_day_week)                                \
    do{                                                             \
        for(int notif=0; NOTIF_PER_DAY>notif; notif++){             \
            if(IS_NOTIF_ACTIVE((notif), (_day_week))){              \
                GET_NOTIF_HOUR((notif), (_day_week)) += 100;        \
            }                                                       \
        }                                                           \
    }while(0) 

#define SET_ON_DAY_NOTIF(_day_week)                                 \
    do{                                                             \
        for(int notif=0; NOTIF_PER_DAY>notif; notif++){             \
            if(!IS_NOTIF_ACTIVE((notif), (_day_week))){             \
                GET_NOTIF_HOUR((notif), (_day_week)) %=100;         \
            }                                                       \
        }                                                           \
    }while(0) 

#define TOOGLE_NOTIF(_notif, _day)                                  \
    do{                                                             \
        if(IS_NOTIF_ACTIVE((_notif),_day)){                         \
            GET_NOTIF_HOUR((_notif), (_day)) +=100;                 \
        } else {                                                    \
            GET_NOTIF_HOUR((_notif), (_day)) %=100;                 \
        }                                                           \
    }while(0) 

#define TOOGLE_DAY_NOTIF(_day_week)                                 \
    do{                                                             \
        if(IS_DAY_ACTIVE(_day_week)){                               \
            SET_OFF_DAY_NOTIF(_day_week);                           \
        } else {                                                    \
            SET_ON_DAY_NOTIF(_day_week);                            \
        }                                                           \
    }while(0) 

#define SET_NOTIF_HOUR(_number_notif, _day_week, _value)                    \    
    do{                                                                     \
        if(IS_NOTIF_ACTIVE((_number_notif), (_day_week))){                  \
            GET_NOTIF_HOUR((_number_notif), (_day_week)) = (_value);        \
        } else {                                                            \
            GET_NOTIF_HOUR((_number_notif), (_day_week)) = (_value + 100);  \
        }                                                                   \
    }while(0) 

#define SET_NOTIF_MIN(_number_notif, _day_week, _value) \  
    do{                                                                     \
        VALUE_NOTIF_MIN((_number_notif), (_day_week)) = (_value);           \
    }while(0) 


/*TIMER*/
#define timer_MIN                   (main_data->timer_data[INDEX_MIN_T])

#define timer_SEC                   (main_data->timer_data[INDEX_SEC_T])

#define timer_HOUR                  (main_data->timer_data[INDEX_HOUR_T])

#define timer_DATA                  (main_data->timer_data)

/*CHECK VALUE*/
#define IS_DAY_WEEK(day_week)       ((day_week) >= 0 && (day_week) < SIZE_WEEK)

#define IS_MIN_OR_SEC(val)          ((val) >= 0 && (val) <= MAX_MIN_SEC)

#define IS_MONTH(val)               ((val) > 0 && (val) <= MAX_MONTH)

#define IS_HOUR(val)                ((val) >= 0 && (val) <= MAX_HOUR)

#define IS_DAY(val)                 ((val) >= 0 && (val) <= MAX_DAY)

#define IS_YEAR(val)                ((val) >= 0 && (val) <= MAX_YEAR)

/* CHECK INPUT KEY*/
#define KEY_IS_CHAR(key_from_dwin)            (((key_from_dwin) >= 'A' && (key_from_dwin) <= 'Z') \
                                                    || ((key_from_dwin) >= 'a' && (key_from_dwin) <= 'z'))

#define KEY_IS_NUMBER(key_from_dwin)          ((key_from_dwin) >= '0' && (key_from_dwin) <= '9')
#define KEY_IS_AREA_SCAN_SSID(key_from_dwin)  ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+MAX_SCAN_LIST_SIZE)

#define KEY_IS_AREA_CLOCK(key_from_dwin)      ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+END_AREA_CLOCK)

#define KEY_IS_AREA_TIMERS(key_from_dwin)     ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+END_AREA_TIMERS)

#define KEY_IS_AREA_STOP_TIMER(key_from_dwin) ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+END_AREA_STOP_TIMERS)

#define KEY_IS_AREA_NOTIF(key_from_dwin)      ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+END_AREA_NOTIFICATIONS)

#define KEY_IS_AREA_CUSTOM(key_from_dwin)     ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+END_AREA_COLORS)

#define KEY_IS_AREA_SETTING(key_from_dwin)    ((key_from_dwin) >= KEY_START_AREA \
                                                    && (key_from_dwin) < KEY_START_AREA+END_AREA_SETTINGS)

#define KEY_IS_SYMBOL(key_from_dwin)          ((key_from_dwin) >= '!'&& (key_from_dwin) <= '~')

#define KEY_IS_AREA_TOGGLE(key_from_dwin)     ((key_from_dwin) >= KEY_TOGGLE_1 \
                                                    && (key_from_dwin) < (KEY_TOGGLE_1+MAX_TOGGLE_UNIT_ON_SCREEN))

#define KEY_IS_AREA_TOGGLE_DAY(key_from_dwin) ((key_from_dwin) >= KEY_TOGGLE_DAY_1 \
                                                    && (key_from_dwin) < (KEY_TOGGLE_DAY_1+SIZE_WEEK))

#define KEY_IS_DAY(key_from_dwin)             ((key_from_dwin) >= KEY_DAY_1 \
                                                    && (key_from_dwin) < KEY_DAY_1+SIZE_WEEK)

#define color_CLOCK                           (USED_COLORS[colors_INTERFACE[AREA_CLOCK_COLOR]])

#define color_DESC                            (USED_COLORS[colors_INTERFACE[AREA_DESC_COLOR]])

#define color_INFO                            (USED_COLORS[colors_INTERFACE[AREA_INFO_COLOR]])

#define GET_COLOR_AREA(n)                     ((n) == (area_SCREEN) ? COLOR_ENABLE : COLOR_DISABLE)

#define DEC_TO_HEX(num)                          (num + 6 * (num / 10))
#define HEX_TO_DEC(num)                          (num - 6 * (num / 16))


#ifndef MIN
    #define MIN(a, b)((a) > (b)? (b): (a))
#endif



#define DWIN_SHOW_ERR(a) \
    do {                                             \
        if (ESP_OK != (a)) {                         \
            ESP_LOGE(TAG,"%s", esp_err_to_name(a));  \ 
        }                                            \
    } while (0)

