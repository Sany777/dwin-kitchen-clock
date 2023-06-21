#include "sensor.h"


bool set_sensor(main_data_t *main_data, 
                        const uint8_t *mac, 
                        const char *name)
{
    if(!main_data->sensor_data){
        main_data->sensor_data = calloc(NUMBER_SENSOR, sizeof(sensor_data_t));
        if(!main_data->sensor_data) return false;
    }
    for(size_t i=0; i<NUMBER_SENSOR; i++){
        if(memcmp(main_data->sensor_data[i].mac, mac, SIZE_MAC) == 0){
            return true;
        }
    }
    for(size_t i=0; i<NUMBER_SENSOR; i++){
        if(!main_data->sensor_data[i].name){
            const size_t name_len = strnlen(name, MAX_NAME_DEVICE);
            main_data->sensor_data[i].name = malloc(name_len+1);
            if(main_data->sensor_data[i].name){
                memcpy(main_data->sensor_data[i].mac, mac, SIZE_MAC);
                memcpy(main_data->sensor_data[i].name, name, name_len);
                main_data->sensor_data[i].name[name_len] = 0;
                return true;
            }
        }
    }
    return false;
}

bool set_sensor_data(main_data_t *main_data, 
                        const uint8_t *mac, 
                        const float temperature,
                        const int humidity)
{
    for(size_t i=0; i<NUMBER_SENSOR; i++){
        if(memcmp(main_data->sensor_data[0].mac, mac, SIZE_MAC) == 0){
            main_data->sensor_data[i].tem = temperature;
            main_data->sensor_data[i].hum = humidity;
            return true;
        }
    }
    return false;
}