#pragma once

#include "dwin_data_types.h"
#include "dwin_common.h"



void write_memory(main_data_t *data_dwin, const int data_identificator);
void read_memory(main_data_t *data_dwin, const int data_identificator);
void read_all_memory(main_data_t *data_dwin);