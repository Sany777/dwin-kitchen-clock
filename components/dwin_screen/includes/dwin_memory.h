#pragma once

#include "dwin_data_types.h"
#include "dwin_common.h"


void dwin_memory_write(const dwin_data_t *main_data, const int data_identificator);
void dwin_memory_read(dwin_data_t *main_data, const int data_identificator);
void dwin_memory_read_all(dwin_data_t *main_data);
void dwin_memory_read_offset(int32_t *offset);
void dwin_memory_write_offset(const int32_t offset);
