#include "dwin_timer.h"
portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
esp_timer_handle_t periodic_timer = NULL;
SLIST_HEAD(list_events_timer, periodic_event) list_periodic_events = SLIST_HEAD_INITIALIZER(list_periodic_events);

void remove_periodic_event(esp_event_base_t base, int32_t event_id)
{
    if(!SLIST_EMPTY(&list_periodic_events)){
        periodic_event_t *item = NULL;
        SLIST_FOREACH(item, &list_periodic_events, next){
            if(item->event_id == event_id && item->base == base){
        // taskENTER_CRITICAL(&myMutex);
                SLIST_REMOVE(&list_periodic_events, item, periodic_event, next);
                free(item);
                return;
        // taskEXIT_CRITICAL(&myMutex);
            }
        }
    }
}




void set_periodic_event(esp_event_loop_handle_t event_loop, esp_event_base_t base, int32_t event_id, size_t sec, mode_time_func_t mode)
{
    if(!periodic_timer){
        init_event_timer();
    }
    periodic_event_t *item = NULL, *tmp = NULL;
    SLIST_FOREACH(tmp, &list_periodic_events, next){
        if(tmp->event_id == event_id && tmp->base == base){
            item = tmp;
            break;
        }
    }

    if(!item){
       item = malloc(sizeof(periodic_event_t)); 
    }
    if(item){
        item->event_loop = event_loop;
        item->base = base;
        item->time_init = sec;
        item->time = 0;
        item->mode = mode;
        item->event_id = event_id;
        SLIST_INSERT_HEAD(&list_periodic_events, item, next);
    }

    if(!esp_timer_is_active(periodic_timer)){
        esp_timer_start_periodic(periodic_timer, 1000000);
    }
}


void periodic_timer_callback(void* arg)
{
    if(!SLIST_EMPTY(&list_periodic_events)){
        periodic_event_t *item = NULL, *tmp = NULL;
        SLIST_FOREACH_SAFE(item, &list_periodic_events, next, tmp){
            if(item ){
                item->time++;
                if(item->time == item->time_init ){
                    esp_event_isr_post_to( item->event_loop,
                                        item->base,
                                        item->event_id,
                                        NULL,
                                        0,
                                        TIMEOUT_PUSH_KEY );
                    if(item->mode == RELOAD_COUNT){
                        item->time = 0;
                    } else {
                        SLIST_REMOVE(&list_periodic_events, item, periodic_event, next);
                        free(item);
                        if(SLIST_EMPTY(&list_periodic_events)){
                            DWIN_SHOW_ERR(esp_timer_stop(periodic_timer));
                            break;
                        }
                    }
                }
            }
        }

    } else {
        esp_timer_stop(periodic_timer);
    }
}

esp_err_t init_event_timer(void)
{
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
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
    for(periodic_event_t *item = NULL; !SLIST_EMPTY(&list_periodic_events); ){
        item = SLIST_FIRST(&list_periodic_events);
        SLIST_REMOVE_HEAD(&list_periodic_events, next);
        free(item);
    }
    SLIST_INIT(&list_periodic_events);
}