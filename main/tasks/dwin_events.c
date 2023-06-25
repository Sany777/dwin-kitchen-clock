#include "dwin_events.h"

uint8_t cur_screen_id;

const handlers_dwin_t screens_handlers[SIZE_LIST_TASKS] = {
    {
        .main_handler    = main_screen_handler,
        .show_handler    = show_main_handler
    },
    {
        .main_handler    = clock_handler,
        .show_handler    = show_clock_handler
    },
    {
        .main_handler    = search_screen_handler,
        .show_handler    = show_ssid_handler
    },
    {
        .main_handler    = setting_screen_handler,
        .show_handler    = show_settings_handler
    },
    {
        .main_handler    = set_color_screen_handler,
        .show_handler    = show_color_screen_handler
    },
    {
        .main_handler    = notifications_screen_handler,
        .show_handler    = show_notify_handler
    },
    {
        .main_handler    = ap_screen_handler,
        .show_handler    = show_ap_handler
    },
    {
        .main_handler    = state_screen_handler,
        .show_handler    = show_state_handler
    },
    {
        .main_handler    = timer_screen_handler,
        .show_handler    = show_timer_handler
    },
    {
        .main_handler    = info_screen_handler,
        .show_handler    = show_info_handler
    },
    {
        .main_handler    = device_screen_handler,
        .show_handler    = show_device_handler
    }
};


void check_net_data(main_data_t* main_data)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(xEventGroup&BIT_CON_STA_OK){
        write_memory(main_data, DATA_PWD);
        write_memory(main_data, DATA_SSID);
    } else {
        read_memory(main_data, DATA_PWD);
        read_memory(main_data, DATA_SSID);
    }
}

void direction_task(void *pv)
{
    uint8_t data_in[2] = {0}, command, symbol;
    main_data_t *main_data = (main_data_t*)pv;
    cur_screen_id = MAIN_SCREEN;
    while(1) {
        if(xQueueReceive(queue_direct, 
                            data_in,
                            portMAX_DELAY) == pdTRUE)
        {
            command = data_in[0];
            symbol = data_in[1];
            if(KEY_IS_SERVICE_COMMAND(command)){
                xQueueSend(queue_service, &command, 500);
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
                    set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
                }
                screen_handler(main_data, command, symbol);
            }
        }
    }
}

void show_task(void *main_data)
{
    show_queue_data_t data_in = {0};
    while(1) {
        if(xQueueReceive(queue_show, &data_in, portMAX_DELAY) == pdTRUE){
            show_handler(main_data, data_in.command, data_in.data);   
            if(data_in.data){
                free(data_in.data);
                data_in.data = NULL;
            }
        }
    }
}

void service_task(void *main_data)
{
    uint8_t key;
    int count = 0;
    while(1) {
        if(xQueueReceive(queue_service, &key, portMAX_DELAY) == pdTRUE){
            switch(key){
                case GET_WEATHER :
                {
                    get_weather(main_data, key);
                    break;
                } 
                case GET_TEMPERATURE :
                {
                    send_str("\n\rStart read %d", count++);
                    read_sensor_handler(main_data); 
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
                    xQueueSend(queue_espnow_tx, &espnow_send, 200);
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
