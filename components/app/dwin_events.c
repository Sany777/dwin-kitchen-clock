#include "dwin_events.h"
#include "dwin_memory.h"
#include "dwin_timer.h"
#include "dwin_services.h"
#include "wifi_dwin.h"
#include "dwin_drivers.h"
#include "esp_timer.h"
uint8_t cur_screen_id;
uint32_t cur_view_generation;

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
            ESP_LOGI("DWIN", "Dispatch touch command=0x%02x symbol=0x%02x screen=%u",
                     command, symbol, cur_screen_id);
            if (command == SERVER_SCREEN) {
                ESP_LOGI("DWIN", "Ignoring AP button; AP mode is disabled from the display");
            } else if(KEY_IS_SERVICE_COMMAND(command)){
                xQueueSend(service_queue, &command, 500);
            } else if(KEY_IS_SET_TASK(command)){
                ESP_LOGI("DWIN", "Switching screen %u -> %u",
                         cur_screen_id, command);
                const int64_t transition_started_us = esp_timer_get_time();
                xSemaphoreTake(dwin_display_mutex, portMAX_DELAY);
                const int64_t render_wait_us = esp_timer_get_time() - transition_started_us;
                xQueueReset(show_queue);
                cur_view_generation++;
                int64_t stage_started_us = esp_timer_get_time();
                screen_handler(main_data, KEY_CLOSE, 0);
                const int64_t close_us = esp_timer_get_time() - stage_started_us;
                stage_started_us = esp_timer_get_time();
                dwin_clear_screen();
                const int64_t clear_us = esp_timer_get_time() - stage_started_us;
                cur_screen_id = command;
                area_SCREEN = 0;
                stage_started_us = esp_timer_get_time();
                switch (cur_screen_id) {
                case CLOCK_SCREEN:
                    dwin_set_picture(CLOCK_PIC);
                    break;
                case NOTIFICATION_SCREEN:
                    dwin_set_picture(NOTIF_PIC);
                    break;
                case SET_COLOR_SCREEN:
                    dwin_set_picture(CUSTOM_COLORS_PIC);
                    break;
                default:
                    break;
                }
                const int64_t picture_us = esp_timer_get_time() - stage_started_us;
                stage_started_us = esp_timer_get_time();
                screen_handler(main_data, KEY_INIT, 0);
                const int64_t init_us = esp_timer_get_time() - stage_started_us;
                const int64_t transition_us = esp_timer_get_time() - transition_started_us;
                xSemaphoreGive(dwin_display_mutex);
                ESP_LOGI("DWIN", "Screen %u ready: render_wait=%lldms setup=%lldms",
                         cur_screen_id, (long long)(render_wait_us / 1000),
                         (long long)((transition_us - render_wait_us) / 1000));
                ESP_LOGI("DWIN", "Transition stages: close=%lldms clear=%lldms picture=%lldms init=%lldms",
                         (long long)(close_us / 1000), (long long)(clear_us / 1000),
                         (long long)(picture_us / 1000), (long long)(init_us / 1000));
            } else {
                if(cur_screen_id != MAIN_SCREEN 
                        && cur_screen_id != TIMER_SCREEN 
                        && cur_screen_id != SERVER_SCREEN)
                {
                    dwin_timer_set_periodic_event(MAIN_SCREEN, DELAY_AUTOCLOSE, ONLY_ONCE);
                }
                xSemaphoreTake(dwin_display_mutex, portMAX_DELAY);
                screen_handler(main_data, command, symbol);
                xSemaphoreGive(dwin_display_mutex);
            }
        }
    }
}

void task_show(void *main_data)
{
    show_queue_data_t data_in = {0};
    while(1) {
        if(xQueueReceive(show_queue, &data_in, portMAX_DELAY) == pdTRUE){
            xSemaphoreTake(dwin_display_mutex, portMAX_DELAY);
            if (data_in.screen_id == cur_screen_id &&
                data_in.view_generation == cur_view_generation &&
                data_in.screen_id >= START_SCREEN_TASK &&
                data_in.screen_id < END_KEY_SCREEN_TASK) {
                const int64_t render_started_us = esp_timer_get_time();
                dwin_show_handler_t handler =
                    screens_handlers[data_in.screen_id - START_SCREEN_TASK].show_handler;
                handler(main_data, data_in.command, data_in.data);
                const int64_t finished_us = esp_timer_get_time();
                ESP_LOGI("DWIN", "Rendered screen=%u cmd=%ld queue=%lldms render=%lldms",
                         data_in.screen_id, (long)data_in.command,
                         (long long)((render_started_us - data_in.queued_at_us) / 1000),
                         (long long)((finished_us - render_started_us) / 1000));
            } else {
                ESP_LOGI("DWIN", "Discard stale show command=%ld screen=%u/%u current=%u/%u",
                         (long)data_in.command, data_in.screen_id,
                         (unsigned)data_in.view_generation, cur_screen_id,
                         (unsigned)cur_view_generation);
            }
            xSemaphoreGive(dwin_display_mutex);
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
