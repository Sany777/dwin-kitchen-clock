#pragma once 

#include "string.h"
#include "stdlib.h"
#include "dwin_config.h"
#include "dwin_data_types.h"
#include "dwin_defines.h"

void set_sensor_data(main_data_t *main_data, 
                        const uint8_t *mac, 
                        const float temperature,
                        const int humidity,  
                        const char *name);