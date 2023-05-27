#include "device_info.h"


SLIST_HEAD(head_list, device_inf) devices_list = SLIST_HEAD_INITIALIZER(devices_list);

device_inf_t *get_my_device()
{
    if(SLIST_EMPTY(&devices_list)){
        uint8_t mac[8];
        if(esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK){
            device_inf_t *my_device = create_device_info(MY_DEVICE_TYPE, mac, MY_DEVICE_NAME);
            add_device_inf_to_list(my_device);
            return my_device;
        }
    } else {
        return SLIST_FIRST(&devices_list);
    }
    return NULL;
}

bool add_device_inf_to_list(device_inf_t *new_elem)
{
    if(!new_elem)return false;
    if(SLIST_EMPTY(&devices_list)){
        SLIST_INSERT_HEAD(&devices_list, new_elem, next);
    } else {
        SLIST_INSERT_AFTER(SLIST_FIRST(&devices_list), new_elem, next);
    }
    return true;
}

device_inf_t* create_device_info(const type_device_t type, const uint8_t *mac, const char *name)
{
    size_t len_name = strnlen(name, MAX_NAME_DEVICE)+1;
    device_inf_t *new_device = calloc(1, sizeof(device_inf_t) + len_name);
    if(new_device){
        memcpy(new_device->name, name, len_name);
        memcpy(new_device->mac, mac, SIZE_MAC);
        new_device->type = type;
    }
    return new_device;
}

void delete_device_info(const uint8_t *mac)
{
    device_inf_t *item  = get_device_before(mac);
    if(item){
        SLIST_REMOVE_AFTER(item, next);
    }
}

device_inf_t *get_device_before(const uint8_t *mac)
{
    if(mac){
        device_inf_t *item = NULL, *next_item = NULL;
        SLIST_FOREACH(item, &devices_list, next){
            next_item = SLIST_NEXT(item, next);
            if(next_item && memcmp(next_item->mac, mac, SIZE_MAC) == 0){
                return item;
            }
        }
    }
    return NULL;
}
