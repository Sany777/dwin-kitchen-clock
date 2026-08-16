#pragma once

#include "esp_mac.h"
#include <sys/queue.h>
#include "string.h"

#include "dwin_common.h"

device_inf_t *device_info_get_my();
bool device_info_add_to_list(device_inf_t *new_elem);
device_inf_t* device_info_create(const type_device_t type, const uint8_t *mac, const char *name);
void device_info_delete(const uint8_t *mac);
device_inf_t *device_info_get_before(const uint8_t *mac);
