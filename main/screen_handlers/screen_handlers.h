#pragma once


#include "dwin_config.h"
#include "dwin_data_types.h"
#include "screen_keys.h"
#include "dwin_defines.h"

#include "dwin_uart.h"
#include "dwin_drivers.h"
#include "screen_keys.h"
#include "dwin_events.h"
#include "screen_handlers.h"
#include "wifi_dwin.h"

/*screen handlers*/
dwin_handler_t setting_handler;
dwin_handler_t search_handler;
dwin_handler_t ap_handler;
dwin_handler_t device_handler;
dwin_handler_t main_handler;
dwin_handler_t weather_detail_handler;
dwin_handler_t clock_handler;
dwin_handler_t customization_handler;
dwin_handler_t notification_handler;
dwin_handler_t timer_handler;


