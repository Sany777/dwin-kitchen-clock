#include "sensor.h"


void set_sensor_data(main_data_t *main_data, 
                        const uint8_t *mac, 
                        const float temperature,
                        const int humidity,  
                        const char *name)
{
    if(!main_data->sensor_data){
        main_data->sensor_data = calloc(NUMBER_SENSOR, sizeof(sensor_data_t));
        if(!main_data->sensor_data) return;
    }
    int index_push;
    if(main_data->sensor_data[0].mac[1] == 0 
        || memcmp(main_data->sensor_data[0].mac, mac, SIZE_MAC) == 0){
        index_push = 0;
    } else {
        index_push = 1;
    } 
    size_t len = strnlen(name, MAX_NAME_DEVICE);
    if(!main_data->sensor_data[index_push].name){
        main_data->sensor_data[index_push].name = malloc(MAX_NAME_DEVICE);
    }
    if(main_data->sensor_data[index_push].name){
        memcpy(main_data->sensor_data[index_push].mac, mac, SIZE_MAC);
        memcpy(main_data->sensor_data[index_push].name, name, len);
        main_data->sensor_data[index_push].name[len] = 0;
        main_data->sensor_data[index_push].tem = temperature;
        main_data->sensor_data[index_push].hum = humidity;
    }
}