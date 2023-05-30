#include "dwin_timer.h"

static esp_timer_handle_t periodic_timer = NULL;
SLIST_HEAD(list_events_timer, time_func) list_timer_func = SLIST_HEAD_INITIALIZER(list_timer_func);


esp_err_t add_function_to_timer_loop(int32_t event_id, size_t sec, mode_time_func_t mode, void *pv)
{
    time_func_t *item = NULL;
    bool no_event = true;
    SLIST_FOREACH(item, &list_timer_func, next){
        if(item->event_id == event_id){
            no_event = false;
            break;
        }
    }
    if(no_event){
       item = malloc(sizeof(time_func_t)); 
    }
    if(item){
        item->time_init = sec;
        item->time = 0;
        item->mode = mode;
        item->event_id = item->event_id;
        SLIST_INSERT_HEAD(&list_timer_func, item, next);
        if(!esp_timer_is_active(periodic_timer)){
            return esp_timer_start_periodic(periodic_timer, 1000000);
        }
        return ESP_OK;
    }
    return ESP_ERR_NO_MEM;
}


void periodic_timer_callback(void* arg)
{
    if(!SLIST_EMPTY(&list_timer_func)){
        time_func_t *item = NULL;
        SLIST_FOREACH(item, &list_timer_func, next){
            if(item->time < item->time_init){
                item->time++;
                if(item->time == item->time_init){
                    esp_event_post_to( loop_service,
                                        TIMER_TURNED_ON,
                                        item->event_id,
                                        item->pv,
                                        0,
                                        TIMEOUT_PUSH_KEY );
                    if(item->mode == RELOAD_COUNT){
                        item->time = 0;
                    } else if(item->mode == WITH_REMOVING) {
                        time_func_t *tmp = item;
                        item = SLIST_NEXT(item, next);          
                        SLIST_REMOVE(&list_timer_func, tmp, time_func, next);
                    }
                }
            } 
        }

    } else {
        esp_timer_stop(periodic_timer);
    }
}

esp_err_t set_mode_timer_function(int32_t event_id, action_timer_func_t mode)
{
    if(!SLIST_EMPTY(&list_timer_func)){
        time_func_t *item = NULL, *item_del = NULL;
        SLIST_FOREACH(item, &list_timer_func, next){
            if(item->event_id == event_id){
                if(mode == PAUSE_TIMER_FUNC){
                    item->mode = WITHOUT_RELOAD;
                    item->time = item->time_init;
                } else if(mode == START_TIMER_FUNC) {
                    item->time = 0;
                } else if(mode == STOP_TIMER_FUNC) {
                    if(item->pv)free(item->pv);
                    SLIST_REMOVE(&list_timer_func, item, time_func, next);
                }
                return ESP_OK;
            }
        }
    }
    return ESP_ERR_NOT_FOUND;
}

esp_err_t init_loop_event_timer(main_data_t *main_data)
{
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .arg = (void*) main_data,
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
    for(time_func_t *item = NULL; !SLIST_EMPTY(&list_timer_func); ){
        item = SLIST_FIRST(&list_timer_func);
        SLIST_REMOVE_HEAD(&list_timer_func, next);
        free(item);
    }
    SLIST_INIT(&list_timer_func);
}