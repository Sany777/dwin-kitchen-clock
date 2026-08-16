#include "dwin_help.h"
#include "dwin_drivers.h"
#include "dwin_timer.h"
#include "screen_handlers.h"
#include <sys/time.h>
bool notification_alarm(const dwin_data_t *main_data, 
                            const struct tm* cur_time, 
                            const bool alarm)
{
    bool signal = false, is_notif = false;
    const int wday = cur_time->tm_wday;
    uint8_t notif_hour, notif_min, cur_hour, cur_min;
    cur_hour = cur_time->tm_hour;
    cur_min = cur_time->tm_min;
    if(IS_DAY_ACTIVE(wday)){
        for(int notif=0; notif<NOTIF_PER_DAY && !is_notif; notif++){
            if(IS_NOTIF_ACTIVE(notif, wday)){
                notif_hour = VALUE_NOTIF_HOUR(notif, wday);
                notif_min = VALUE_NOTIF_MIN(notif, wday);
                if(cur_hour == notif_hour){
                    if(cur_min == notif_min){
                        signal = true;
                    }
                    if (notif_min <= (cur_min + MIN_BEFORE_NOTIFICATION)
                                && notif_min >= cur_min)
                    {
                        is_notif = true;
                    }
                } else if((cur_hour+1) == notif_hour 
                            && (notif_min+60) < 
                                (MIN_BEFORE_NOTIFICATION+cur_min))
                {
                    is_notif = true;
                }                             
            }
        }
    }
    if(alarm 
        && cur_hour >= 6 
        && cur_hour <= 23 
        && (signal || cur_min == 0))
    {
        dwin_sound_buzzer(cur_hour == 7 
                    || cur_hour == 12 
                    || cur_hour == 18 
                    || signal
                        ? LOUD_BUZZER
                        : NORMAL_BUZZER);
    }
        
    return is_notif;
}


void show_screen(int32_t key, const void *data_send, const size_t size_data)
{                                     
    show_queue_data_t to_send = {        
        .command = key,  
    };      
    if(size_data && data_send && size_data <= sizeof(to_send.data)){
        memcpy(to_send.data, data_send, size_data);
    }                              
    xQueueSend(show_queue, &to_send, 200);
}

void wifi_dwin_set_timezone(int offset)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(xEventGroup&BIT_SNTP_ALLOW){
        if(offset < 24 && offset > -24){
            char env_format[sizeof(TIME_ZONA_FORMAT)+4];
            sprintf(env_format, "EET%+dEEST,M3.5.0/3,M10.5.0/4", offset);
            setenv("TZ", env_format, 1);
            tzset();
        }
    } else {
        unsetenv("TZ");
    }
}


const uint16_t *dwin_help_get_y_points( const float *points, 
                                const uint16_t number,
                                const uint16_t height)
{
    float max = 0, min_val = 0;
    uint16_t *buf_out  = NULL;

        for(uint16_t i=0; i<number; i++){
            if(points[i] > max){
                max = points[i];
            }
            if(points[i] < min_val){
                min_val = points[i];
            }
        }
        float corection = 0;
        if( min_val<0 ){
            corection = -min_val;
        }
        buf_out = (uint16_t *)(send_buf + MAX_DATA_LEN - number*sizeof(uint16_t));
        float diapazon = max-min_val;
        if( diapazon < 10)diapazon = 10;
        const float k_h = (height)/(diapazon);
        for(uint16_t i=0; i<number; i++){
            buf_out[i] = (points[i]+corection) * k_h;
        }
    return buf_out;
}

static void performTimeAction(struct tm* cur)
{
    static bool night;
    if(cur->tm_hour < 6){
        if(!night){
            dwin_set_brightness(30);
            night = true;
        }
    } else if(night){
        dwin_set_brightness(100);
        night = false;
    }
    if(cur->tm_hour == GET_CURRENCY_HOUR && cur->tm_min == 0){
        dwin_timer_set_periodic_event(UPDATE_CURRENCY, 40, ONLY_ONCE);
    }
}

static void fixDay(struct tm* cur)
{
    cur->tm_wday = (cur->tm_wday+6)%SIZE_WEEK;
}

struct tm* wifi_dwin_get_time_tm(void)
{
    static struct tm cur;
    time_t time_now;
    time(&time_now);
    localtime_r(&time_now, &cur);
    fixDay(&cur);
    performTimeAction(&cur);
    return &cur;
}

void set_time_tv(struct timeval *tv)
{
    settimeofday(tv, NULL);
    xEventGroupSetBits(dwin_event_group, BIT_IS_TIME);
    dwin_timer_set_new_command(UPDATE_TIME_COMPLETE);
}

void set_dwin_clock()
{
    const struct tm* cur_tm = wifi_dwin_get_time_tm();
    dwin_clock_set(cur_tm);
}

void wifi_dwin_set_time_tm(struct tm *timeptr)
{
    if(timeptr->tm_year >= 123 && timeptr->tm_year < 223){
        time_t time = mktime(timeptr);
        struct timeval tv = {
            .tv_sec = time,
        };
        set_time_tv(&tv);
        dwin_clock_set(timeptr);
    }
}

const char *get_data_from_uri(const char *uri_str, 
                                const char *base_path)
{
    const size_t base_pathlen = strnlen(base_path, MAX_URI_LEN);
    size_t pathlen = strnlen(uri_str, MAX_URI_LEN + 1);
    if (pathlen <= base_pathlen
        || pathlen > MAX_URI_LEN
        || strncmp(uri_str, base_path, base_pathlen) != 0
        || uri_str[base_pathlen] != '/'){
        return NULL;
    }
    return uri_str+base_pathlen+1;
}

const char *get_chip(int model_id)
{
    switch(model_id){
        case 1: return "ESP32";
        case 2: return "ESP32-S2";
        case 3: return "ESP32-S3";
        case 5: return "ESP32-C3";
        case 6: return "ESP32-H2";
        case 12: return "ESP32-C2";
        default: break;
    }
    return "uknown";
}


void dwin_help_init_currency_val(dwin_data_t *main_data)
{
    usd_Sale = DWIN_NO_DATA;
    eur_Sale = DWIN_NO_DATA;
    usd_Bay = DWIN_NO_DATA;
    eur_Bay = DWIN_NO_DATA;
}
