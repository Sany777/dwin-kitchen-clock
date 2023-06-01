#pragma once

#include "driver/gpio.h"
#include "dwin_common.h"


void esp_init(void);
void wifi_init(void);



extern EventGroupHandle_t dwin_event_group;

