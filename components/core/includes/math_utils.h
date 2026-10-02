#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>

bool map_float_series_to_y(const float *values, size_t count, uint16_t height,
                           uint16_t *output);
bool parse_float_string(const char *text, float *value);
bool parse_int_string(const char *text, int *value);
bool parse_int64_string(const char *text, int64_t *value);
