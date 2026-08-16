#include "device_info.h"

#define MAX_DEVICES 10

SLIST_HEAD(head_list, device_inf) devices_list = SLIST_HEAD_INITIALIZER(devices_list);
static device_inf_t devices_pool[MAX_DEVICES];
static bool is_allocated[MAX_DEVICES] = {false};

device_inf_t *
device_info_get_my()
{
    if (SLIST_EMPTY(&devices_list)) {
        uint8_t mac[8];
        if (esp_read_mac(mac, ESP_MAC_WIFI_STA) == ESP_OK) {
            device_inf_t *my_device = device_info_create(MY_DEVICE_TYPE, mac, CONFIG_MY_DEVICE_NAME);
            device_info_add_to_list(my_device);
            return my_device;
        }
    } else {
        return SLIST_FIRST(&devices_list);
    }
    return NULL;
}

bool
device_info_add_to_list(device_inf_t *new_elem)
{
    if (!new_elem)
        return false;
    if (SLIST_EMPTY(&devices_list)) {
        SLIST_INSERT_HEAD(&devices_list, new_elem, next);
    } else {
        SLIST_INSERT_AFTER(SLIST_FIRST(&devices_list), new_elem, next);
    }
    return true;
}

device_inf_t *
device_info_create(const type_device_t type, const uint8_t *mac, const char *name)
{
    device_inf_t *new_device = NULL;
    for (int i = 0; i < MAX_DEVICES; i++) {
        if (!is_allocated[i]) {
            is_allocated[i] = true;
            new_device      = &devices_pool[i];
            break;
        }
    }
    if (new_device) {
        const size_t len_name = strnlen(name, MAX_NAME_DEVICE);
        memcpy(new_device->name, name, len_name);
        new_device->name[len_name] = '\0';
        memcpy(new_device->mac, mac, SIZE_MAC);
        new_device->type = type;
    }
    return new_device;
}

void
device_info_delete(const uint8_t *mac)
{
    device_inf_t *item = device_info_get_before(mac);
    if (item) {
        device_inf_t *next_item = SLIST_NEXT(item, next);
        SLIST_REMOVE_AFTER(item, next);
        if (next_item) {
            for (int i = 0; i < MAX_DEVICES; i++) {
                if (&devices_pool[i] == next_item) {
                    is_allocated[i] = false;
                    break;
                }
            }
        }
    }
}

device_inf_t *
device_info_get_before(const uint8_t *mac)
{
    if (mac) {
        device_inf_t *item = NULL, *next_item = NULL;
        SLIST_FOREACH(item, &devices_list, next)
        {
            next_item = SLIST_NEXT(item, next);
            if (next_item && memcmp(next_item->mac, mac, SIZE_MAC) == 0) {
                return item;
            }
        }
    }
    return NULL;
}
