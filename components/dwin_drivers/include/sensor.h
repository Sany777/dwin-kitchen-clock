#pragma once 

#include "string.h"
#include "stdlib.h"
#include "dwin_config.h"
#include "dwin_data_types.h"
#include "dwin_defines.h"

bool sensor_set_data(dwin_data_t *main_data, 
                        const uint8_t *mac, 
                        const float temperature,
                        const int humidity);
bool sensor_set_device(dwin_data_t *main_data, 
                        const uint8_t *mac, 
                        const char *name);
