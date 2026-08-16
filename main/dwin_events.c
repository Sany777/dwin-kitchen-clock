#include "dwin_events.h"
#include "dwin_memory.h"
#include "dwin_timer.h"
#include "dwin_services.h"
#include "bmp280_main.h"
#include "wifi_dwin.h"
uint8_t cur_screen_id;

const handlers_dwin_t screens_handlers[SIZE_LIST_TASKS] = {
    {
        .main_handler    = screen_handler_main,
        .show_handler    = show_screen_main
    },
    {
        .main_handler    = screen_handler_clock,
        .show_handler    = show_clock_handler
    },
    {
        .main_handler    = screen_handler_wifi_scan,
        .show_handler    = show_screen_ssid
    },
    {
        .main_handler    = screen_handler_settings,
        .show_handler    = show_screen_settings
    },
    {
        .main_handler    = handle_color_settings_screen,
        .show_handler    = show_color_screen_handler
    },
    {
        .main_handler    = screen_handler_notifications,
        .show_handler    = show_screen_notify
    },
    {
        .main_handler    = screen_handler_ap,
        .show_handler    = show_screen_ap
    },
    {
        .main_handler    = handle_state_screen,
        .show_handler    = show_screen_state
    },
    {
        .main_handler    = screen_handler_timer,
        .show_handler    = show_screen_timer
    },
    {
        .main_handler    = screen_handler_info,
        .show_handler    = show_info_handler
    },
    {
        .main_handler    = screen_handler_device,
        .show_handler    = show_screen_device
    }
};


void check_net_data(dwin_data_t* main_data)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(xEventGroup&BIT_CON_STA_OK){
        dwin_memory_write(main_data, DATA_PWD);
        dwin_memory_write(main_data, DATA_SSID);
    } else {
        dwin_memory_read(main_data, DATA_PWD);
        dwin_memory_read(main_data, DATA_SSID);
    }
}

void task_direction(void *pv)
{
    uint8_t data_in[2] = {0}, command, symbol;
    dwin_data_t *main_data = (dwin_data_t*)pv;
    cur_screen_id = MAIN_SCREEN;
    while(1) {
        if(xQueueReceive(direct_queue, 
                            data_in,
                            portMAX_DELAY) == pdTRUE)
        {
            command = data_in[0];
            symbol = data_in[1];
            if(KEY_IS_SERVICE_COMMAND(command)){
                xQueueSend(service_queue, &command, 500);
            } else if(KEY_IS_SET_TASK(command)){
                screen_handler(main_data, KEY_CLOSE, 0);
                vTaskDelay(300);
                cur_screen_id = command;
                area_SCREEN = 0;
                screen_handler(main_data, KEY_INIT, 0);
            } else {
                if(cur_screen_id != MAIN_SCREEN 
                        && cur_screen_id != TIMER_SCREEN 
                        && cur_screen_id != SERVER_SCREEN)
                {
                    dwin_timer_set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
                }
                screen_handler(main_data, command, symbol);
            }
        }
    }
}

void task_show(void *main_data)
{
    show_queue_data_t data_in = {0};
    while(1) {
        if(xQueueReceive(show_queue, &data_in, portMAX_DELAY) == pdTRUE){
            show_handler(main_data, data_in.command, data_in.data);   
        }
    }
}

void task_service(void *main_data)
{
    uint8_t key;

    while(1) {
        if(xQueueReceive(service_queue, &key, portMAX_DELAY) == pdTRUE){
            switch(key){
                case UPDATE_CURRENCY :
                {
                    weather_client_get_currency(main_data);
                    break;
                }
                case GET_WEATHER :
                {
                    weather_client_get_weather(main_data, key);
                    break;
                } 
                case GET_TEMPERATURE :
                {
                    bme280_read_sensor(main_data); 
                    break;
                } 
                case INIT_SNTP :
                case STOP_SNTP :
                {
                   set_sntp(main_data, key); 
                   break; 
                }
                case CHECK_NET_DATA :
                {
                   check_net_data(main_data); 
                   break; 
                } 
                case GET_REMOTE_SENSOR : 
                {
                    espnow_send_t espnow_send = {0};
                    espnow_send.action = NEED_TEMP;
                    xQueueSend(espnow_tx_queue, &espnow_send, 200);
                    break;
                }
                default :
                {
                    set_wifi(main_data, key);
                    break;
                }     
            }
        }
    }
}


void vApplicationIdleHook(void)
{ 
    while (1) {

    }
}
