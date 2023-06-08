#include "dwin_help.h"


void set_timezone(int offset)
{
    if(offset < 24 || offset > -24){
        char buf_format_time[SIZE_BUF_FORMAT_CLOCK] = {0};
        sprintf(buf_format_time, "EET%+dEEST,M3.5.0/3,M10.5.0/4", offset);
        setenv("TZ", buf_format_time, 1);
        tzset();
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
    esp_event_post_to(direct_loop, EVENTS_DIRECTION, KEY_UPDATE_SCREEN, NULL, 0, TIMEOUT_SEND_EVENTS);
}

void set_time_tm(struct tm *timeptr, const bool update_dwin)
{
    if(timeptr->tm_hour < 24 && timeptr->tm_year >= 123){
        time_t time = mktime(timeptr);
        struct timeval tv = {
            .tv_sec = time,
        };
        set_time_tv(&tv);
        xEventGroupSetBits(dwin_event_group, BIT_IS_TIME);
        if(update_dwin)dwin_clock_set(&timeptr);
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