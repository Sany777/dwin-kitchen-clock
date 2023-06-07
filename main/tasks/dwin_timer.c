#include "dwin_timer.h"



esp_timer_handle_t periodic_timer = NULL;
periodic_event_t *list_periodic_events = NULL;
portMUX_TYPE periodic_timers_s = portMUX_INITIALIZER_UNLOCKED;

size_t number_event = 0;
size_t size_list = 0;
#define STEP_RESIZE 3

void resize_list()
{
    taskENTER_CRITICAL(&periodic_timers_s);
    if(number_event == 0){
        if(list_periodic_events){
            free(list_periodic_events);
            list_periodic_events = NULL;
            size_list = 0;
            esp_timer_stop(periodic_timer);
        }
    } else if(size_list > (number_event + STEP_RESIZE)){
        const size_t new_size = (number_event/STEP_RESIZE)*STEP_RESIZE + STEP_RESIZE;
        periodic_event_t *new_list_periodic_events = calloc(new_size, sizeof(periodic_event_t));
        if(new_list_periodic_events){
            for(size_t oldi=0, ni=0; oldi<size_list; oldi++){
                if(list_periodic_events[oldi].event_loop){
                    memcpy(&new_list_periodic_events[ni++], &list_periodic_events[oldi], sizeof(periodic_event_t));
                }
            }
            size_list = new_size;
            free(list_periodic_events);
            list_periodic_events = new_list_periodic_events;
        }
    }
    taskEXIT_CRITICAL(&periodic_timers_s);
}

void remove_periodic_event(esp_event_base_t base, int32_t event_id)
{
    if(number_event){
        periodic_event_t *item;
        for(int i=0; i<size_list; i++){
            item = &list_periodic_events[i];
            if(item->event_loop
                        && item->event_id == event_id 
                        && item->base == base)
                {
                    item->event_loop = NULL;
                    number_event--;
                    if(number_event == 0 || size_list > (number_event + STEP_RESIZE)){
                        resize_list();
                    }
                    return;
                }
        }
    }
}



esp_err_t  set_periodic_event(esp_event_loop_handle_t event_loop, 
                                esp_event_base_t base, 
                                int32_t event_id, 
                                size_t sec, 
                                mode_time_func_t mode)
{
    taskENTER_CRITICAL(&periodic_timers_s);
    if(size_list < number_event+1){
        periodic_event_t *new_list_periodic_events = calloc(size_list + STEP_RESIZE, sizeof(periodic_event_t));
        DWIN_CHECK_NULL_AND_GO(new_list_periodic_events, "no create new_list_periodic_events", err);
        if(list_periodic_events){
            memcpy(new_list_periodic_events, list_periodic_events, sizeof(periodic_event_t)*size_list);
            free(list_periodic_events);
        }
        list_periodic_events = new_list_periodic_events;
        size_list += STEP_RESIZE;
    }
    periodic_event_t *item = NULL, *empty = NULL, *cur_item = NULL;
    for(int i=0; i<size_list; i++){
        cur_item = &list_periodic_events[i];
        if(cur_item->event_loop){
            if(cur_item->event_id == event_id 
                    && cur_item->base == base)
            {
                item = cur_item;
                break;
            }
        } else {
            empty = cur_item;
        }
    }
    if(!item) {
        item = empty;
        item->base = base;
        item->event_id = event_id;
    }
    item->event_loop = event_loop;
    item->time_init = sec;
    item->time = 0;
    item->mode = mode;
    number_event++;
    if(!periodic_timer){
        DWIN_CHECK_AND_GO(init_event_timer(), err);
    }
    if(!esp_timer_is_active(periodic_timer)){
        DWIN_CHECK_AND_GO(esp_timer_start_periodic(periodic_timer, 1000000), err);
    }
    taskEXIT_CRITICAL(&periodic_timers_s);
    return ESP_OK;
err:
    taskEXIT_CRITICAL(&periodic_timers_s);
    return ESP_FAIL;
}


void periodic_timer_cb(void* arg)
{
    if(number_event){
        periodic_event_t *item = NULL;
        for(int i=0; i<size_list; i++){
            item = &list_periodic_events[i];
            if(item->event_loop){
                item->time++;
                if(item->time == item->time_init){
                    esp_event_isr_post_to( item->event_loop,
                                            item->base,
                                            item->event_id,
                                            NULL,
                                            0,
                                            NULL );
                    if(item->mode == RELOAD_COUNT){
                        item->time = 0;
                    } else {
                        number_event--;
                        item->event_loop = NULL;
                    }
                }
            }
        }
        if(number_event == 0 || size_list > (number_event + STEP_RESIZE)){
            resize_list();
        }
    }
}

esp_err_t init_event_timer(void)
{
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_cb,
        .arg = NULL,
        .name = "periodic"
    };
    return esp_timer_create(&periodic_timer_args, &periodic_timer);
}

void stop_event_timer(void)
{
    esp_timer_stop(periodic_timer);
}

void remove_event_timer(void)
{
    if(periodic_timer){
        esp_timer_delete(periodic_timer);
        periodic_timer = NULL;
    }
}