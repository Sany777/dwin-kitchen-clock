#include "dwin_help.h"




bool notification_alarm(const main_data_t *main_data, 
                            const struct tm* cur_time, 
                            const bool alarm)
{
    int wday = cur_time->tm_wday;
    if(IS_DAY_ACTIVE(wday)){
        uint8_t notif_hour, notif_min, cur_hour, cur_min;
        cur_hour = cur_time->tm_hour;
        cur_min = cur_time->tm_min;
        for(int notif=0; notif<NOTIF_PER_DAY; notif++){
            if(IS_NOTIF_ACTIVE(notif, wday)){
                notif_hour = VALUE_NOTIF_HOUR(notif, wday);
                notif_min = VALUE_NOTIF_MIN(notif, wday);
                if(cur_hour == notif_hour){
                    if(cur_min == notif_min){
                        if(alarm 
                            &&notif_hour >= 6 
                            && notif_hour <= 23)
                        {
                            dwin_buzer(notif_hour == 7 
                                        || notif_hour == 12 
                                        || notif_hour == 18 
                                            ? LOUD_BUZZER
                                            : NORMAL_BUZZER);
                        }
                        return true;
                    } else if (notif_min <= cur_min + MIN_BEFORE_NOTIFICATION
                                && notif_min > cur_min){
                        return true;
                    }
                } else if(cur_hour+1 == notif_hour 
                            && notif_min <= MIN_BEFORE_NOTIFICATION
                            && 60+notif_min < cur_min+MIN_BEFORE_NOTIFICATION){
                    return true;
                }                             
            }
        }
    }
    return false;
}


esp_err_t show_screen(int32_t key, const void *data_send, const size_t size_data)
{                                     
    show_queue_data_t to_send = {        
        .command = key,  
        .data = NULL             
    };      
    if(size_data){
        to_send.data = (void*)malloc(size_data);
        if(!to_send.data) return ESP_FAIL;
        memcpy(to_send.data, data_send, size_data);
    }                              
    if(xQueueSend(queue_show, &to_send, WAIT_SHOW) == pdTRUE){
        return ESP_OK;
    } else if(to_send.data){
        free(to_send.data);
    }
    return ESP_FAIL;
}

void set_timezone(int offset)
{
    if(offset < 24 || offset > -24){
        setenv("TZ", TIME_ZONA_FORMAT, offset);
        tzset();
    }
}


uint16_t *get_y_points(  int8_t *points, 
                            const int number,
                            const uint16_t height )
{
    int max = 0, min_val = 0;
    uint16_t *buf_out  = NULL;
    
    bool is_number = false;
    for(int i=0; i<number; i++){
        if(points[i] > max){
            max = points[i];
            if(!is_number){
                is_number = true;
            }
        } else if(points[i] < min_val){
            min_val = points[i];
        }
    }
    if(is_number){
        buf_out = (uint16_t *)(buf_operation + MAX_DATA_LEN - number*sizeof(uint16_t));
        min_val *= -1;
        max += min_val;
        const float k_h = (float)height/max;
        for(int i=0; i<number; i++){
            buf_out[i] = (uint16_t)((points[i]+min_val) * k_h);
        }
    }
    return buf_out;
}

struct tm* get_time_tm()
{
    time_t time_now;
    time(&time_now);
    
    return gmtime(&time_now);
}

void set_time_tv(struct timeval *tv)
{
    settimeofday(tv, NULL);
    xEventGroupSetBits(dwin_event_group, BIT_IS_TIME);
    set_new_event(UPDATE_DATA_COMPLETE);
}

void set_time_tm(struct tm *timeptr, const bool update_dwin)
{
    if(timeptr->tm_hour < 24 && timeptr->tm_year >= 123){
        time_t time = mktime(timeptr);
        struct timeval tv = {
            .tv_sec = time,
        };
        set_time_tv(&tv);
        if(update_dwin)dwin_clock_set(timeptr);
    }
}

char *get_data_from_uri(const char *uri_str, 
                                const char *base_path)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri_str);
    if (pathlen <= base_pathlen
        || uri_str[base_pathlen] != '/'
        || pathlen > MAX_URI_LEN){
        return NULL;
    }
    return uri_str+base_pathlen+1;
}

char *get_chip(int model_id)
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