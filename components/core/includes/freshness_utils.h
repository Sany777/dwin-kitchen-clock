#pragma once

#include <stdbool.h>
#include <stdint.h>

bool timestamp_is_fresh(int64_t updated_at, int64_t now, int64_t max_age);
