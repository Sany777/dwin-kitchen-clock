#include "sensor.h"

bool add_sensor(main_data_t *main_data, const uint8_t *mac, const char *name)
{
    int index_push = 0;
    while(main_data->sensor_data[index_push] != NULL){
        index_push++;
        if(index_push > NUMBER_SENSOR)return false;
    }
    size_t name_len = strlen(name);
    sensor_data_t *sensor_data = (sensor_data_t *)malloc(sizeof(sensor_data_t) + name_len);
    if(sensor_data){
        memcpy(main_data->sensor_data[index_push]->mac, mac, SIZE_MAC);
        memcpy(main_data->sensor_data[index_push]->name, name, name_len);
        return true;
    }
    return false;
}

sensor_data_t *get_sensor(main_data_t *main_data, const uint8_t *mac)
{
    for(int i=0; i<NUMBER_SENSOR; i++){
        if(main_data->sensor_data[i]){
            if( memcmp(main_data->sensor_data[i]->mac, mac, SIZE_MAC) == 0){
                return main_data->sensor_data[i];
            }
        }
    }
    return NULL;
}

bool set_sensor_data(main_data_t *main_data, const uint8_t *mac, const temperature_t temperature, const int humidity)
{
    sensor_data_t *sensor = get_sensor(main_data, mac);
    if(!sensor){
        return false;
    }
    sensor->temperature = temperature;
    sensor->humidity = humidity;
    return true;
}