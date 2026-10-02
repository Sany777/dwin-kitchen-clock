#include "dwin_timer.h"

#define MAX_PERIODIC_EVENTS 64

esp_timer_handle_t periodic_timer                                 = NULL;
static periodic_event_t list_periodic_events[MAX_PERIODIC_EVENTS] = {0};
portMUX_TYPE periodic_timers_s                                    = portMUX_INITIALIZER_UNLOCKED;

size_t number_event = 0;

void
dwin_timer_remove_periodic_event(uint8_t command)
{
    if (number_event) {
        for (int i = 0; i < MAX_PERIODIC_EVENTS; i++) {
            if (list_periodic_events[i].command == command) {
                list_periodic_events[i].time    = 0;
                list_periodic_events[i].command = 0;
                number_event--;
                return;
            }
        }
    }
}

void
dwin_timer_set_new_command(uint8_t command)
{
    uint8_t send[2] = {command, 0};
    xQueueSend(direct_queue, send, 300);
}

esp_err_t
dwin_timer_set_periodic_event(uint8_t command, size_t sec, int mode)
{
    taskENTER_CRITICAL(&periodic_timers_s);
    periodic_event_t *item = NULL, *empty = NULL;
    for (int i = 0; i < MAX_PERIODIC_EVENTS; i++) {
        if (list_periodic_events[i].command == command) {
            item = &list_periodic_events[i];
            break;
        } else if (empty == NULL && list_periodic_events[i].time == 0) {
            empty = &list_periodic_events[i];
        }
    }
    if (!item) {
        if (!empty) {
            taskEXIT_CRITICAL(&periodic_timers_s);
            return ESP_FAIL;
        }
        item          = empty;
        item->command = command;
        number_event++;
    }
    item->time_init = sec;
    item->time      = sec;
    item->mode      = mode;

    if (!periodic_timer) {
        DWIN_CHECK_AND_GO(init_event_timer(), err);
    }
    if (!esp_timer_is_active(periodic_timer)) {
        DWIN_CHECK_AND_GO(esp_timer_start_periodic(periodic_timer, 1000000), err);
    }
    taskEXIT_CRITICAL(&periodic_timers_s);
    return ESP_OK;
err:
    taskEXIT_CRITICAL(&periodic_timers_s);
    return ESP_FAIL;
}

void
periodic_timer_cb(void *arg)
{
    if (number_event) {
        BaseType_t change_cntx = false, need_change = false;
        periodic_event_t *item = NULL;
        for (int i = 0; i < MAX_PERIODIC_EVENTS; i++) {
            item = &list_periodic_events[i];
            if (item->command && item->time) {
                item->time--;
                if (item->time == 0) {
                    uint8_t send[2] = {item->command, 0};
                    xQueueSendFromISR(direct_queue, send, &change_cntx);
                    if (!need_change && change_cntx)
                        need_change = true;
                    if (item->mode == RELOAD_COUNT) {
                        item->time = item->time_init;
                    } else {
                        number_event--;
                        item->command = 0;
                    }
                }
            }
        }
        if (need_change)
            portYIELD_FROM_ISR();
    }
}

esp_err_t
init_event_timer(void)
{
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_cb, .arg = NULL, .name = "periodic"};
    return esp_timer_create(&periodic_timer_args, &periodic_timer);
}

void
stop_event_timer(void)
{
    esp_timer_stop(periodic_timer);
}

void
remove_event_timer(void)
{
    if (periodic_timer) {
        esp_timer_delete(periodic_timer);
        periodic_timer = NULL;
    }
}
