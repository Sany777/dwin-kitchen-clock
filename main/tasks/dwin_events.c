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
    uint8_t data_in[2] = {0};
    main_data_t *main_data = (main_data_t*)pv;
    cur_screen_id = MAIN_SCREEN;
    while(1) {
        if(xQueueReceive(queue_direct, 
                            data_in,
                            portMAX_DELAY) == pdTRUE)
        {

            if(cur_screen_id != MAIN_SCREEN 
                    && cur_screen_id != TIMER_SCREEN 
                    && cur_screen_id != SERVER_SCREEN)
            {
                set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
            }
            if(KEY_IS_SET_TASK(data_in[0])){
                cur_screen_id = data_in[0];
                screen_handler(main_data, KEY_CLOSE, 0);
                if(cur_screen_id == MAIN_SCREEN){
                    set_power_mode_eco(true);
                } else {
                    set_power_mode_eco(false);
                }
                area_SCREEN = 0;
                screen_handler(main_data, KEY_INIT, 0);
            } else  if(data_in[0] > START_SERVICE_EVENTS && data_in[0] < END_SERVICE_EVENTS){
                xQueueSend(queue_service, &data_in[0], 200);
            } else {
                screen_handler(main_data, data_in[0], data_in[1]);
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
    while(1) {
        if(xQueueReceive(queue_service, &key, portMAX_DELAY) == pdTRUE){
            if(key == GET_WEATHER){
                get_weather(main_data, key);
            } else if (key == INIT_SNTP || key == STOP_SNTP){
                set_sntp(main_data, key);
            } else if(key == CHECK_NET_DATA){
                check_net_data(main_data);
            } else if (key == KEY_NEED_TEMP){
                espnow_send_t espnow_send = {0};
                espnow_send.action = NEED_TEMP;
                xQueueSend(queue_espnow_tx, &espnow_send, 200);
            }else {
                set_wifi(main_data, key);
            } 
        }
    }
}


void vApplicationIdleHook(void)
{ 
    EventBits_t xEventGroup;
    bool cur_mode = false, now;
    while (1) {
        xEventGroup = xEventGroupGetBits(dwin_event_group);
        now = xEventGroup&BIT_NIGHT;
        if(now != cur_mode){
            if(now){
                dwin_set_brightness(30);
            } else {
                dwin_set_brightness(100);
            }
            cur_mode = now;
        }
    }
}
