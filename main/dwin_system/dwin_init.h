#pragma once

#include "driver/gpio.h"
#include "dwin_common.h"


void esp_init();
void wifi_init();


extern EventGroupHandle_t dwin_event_group;

