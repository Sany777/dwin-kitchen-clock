#include "sensor.h"


bool sensor_set_device(dwin_data_t *main_data, 
                        const uint8_t *mac, 
                        const char *name)
{
    for(size_t i=0; i<NUMBER_SENSOR; i++){
        if(memcmp(main_data->sensor_data[i].mac, mac, SIZE_MAC) == 0){
            return true;
        }
    }
    for(size_t i=0; i<NUMBER_SENSOR; i++){
        if(main_data->sensor_data[i].name[0] == '\0'){
            const size_t name_len = strnlen(name, MAX_NAME_DEVICE);
            memcpy(main_data->sensor_data[i].mac, mac, SIZE_MAC);
            memcpy(main_data->sensor_data[i].name, name, name_len);
            main_data->sensor_data[i].name[name_len] = '\0';
            return true;
        }
    }
    return false;
}

bool sensor_set_data(dwin_data_t *main_data, 
                        const uint8_t *mac, 
                        const float temperature,
                        const int humidity)
{
    for(size_t i=0; i<NUMBER_SENSOR; i++){
        if(memcmp(main_data->sensor_data[i].mac, mac, SIZE_MAC) == 0){
            main_data->sensor_data[i].tem = temperature;
            main_data->sensor_data[i].hum = humidity;
            return true;
        }
    }
    return false;
}
