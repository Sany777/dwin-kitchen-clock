#pragma once

#include "esp_mac.h"
#include <sys/queue.h>
#include "string.h"

#include "dwin_common.h"

device_inf_t *get_my_device();
bool add_device_inf_to_list(device_inf_t *new_elem);
device_inf_t* create_device_info(const type_device_t type, const uint8_t *mac, const char *name);
void delete_device_info(const uint8_t *mac);
device_inf_t *get_device_before(const uint8_t *mac);