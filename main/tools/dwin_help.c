#include "dwin_help.h"


void set_timezone(int hour)
{
    if(hour < 23 || hour > -23){
        char *buf_format_time = calloc(1, SIZE_BUF_FORMAT_CLOCK);
        if(buf_format_time){
            sprintf(buf_format_time, "EET%+dEEST,M3.5.0/3,M10.5.0/4", hour);
            setenv("TZ", buf_format_time, 1);
            tzset();
            free(buf_format_time);
        }    
    }
}

void fix_scale_height(uint16_t *h_points, 
                const int number,
                const uint16_t height)
{
    uint8_t max_height = 0;
    for(int i=0; i<number; i++){
        if(h_points[i] > max_height){
            max_height = h_points[i];
        }
    }
    const float k_h = height/max_height;
    for(int i=0; i<number; i++){
        h_points[i] = h_points[i] * k_h;
    }
}


void set_time_tv(struct timeval *tv)
{
    settimeofday(tv, NULL);
    esp_event_post_to(direct_loop, EVENTS_DIRECTION, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
    if(sntp_get_sync_interval() < SYNCH_10_HOUR)sntp_set_sync_interval(SYNCH_10_HOUR);
}

void set_time_tm(struct tm *timeptr)
{
    time_t time = mktime(timeptr);
    struct timeval tv = {
        .tv_sec = time,
    };
    set_time_tv(&tv);
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