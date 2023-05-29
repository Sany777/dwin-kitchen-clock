#include "dwin_help.h"


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

char *get_pos_data_str_from_uri(const char *uri_str, 
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