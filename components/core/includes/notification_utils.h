#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool format_notification_schedule(const uint8_t *notification_data,
                                  size_t notification_data_size,
                                  size_t day_count,
                                  size_t notifications_per_day,
                                  char *output, size_t output_size);
