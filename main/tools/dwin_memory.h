#pragma once

#include "dwin_data_types.h"
#include "dwin_common.h"


void write_memory(main_data_t *main_data, const int data_identificator);
void read_memory(main_data_t *main_data, const int data_identificator);
void read_all_memory(main_data_t *main_data);
void read_offset(int32_t *offset);
void write_offset(int32_t *offset);