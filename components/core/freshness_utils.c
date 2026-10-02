#include "freshness_utils.h"

bool timestamp_is_fresh(int64_t updated_at, int64_t now, int64_t max_age) {
  return updated_at > 0 && now >= updated_at && max_age >= 0 &&
         now - updated_at <= max_age;
}
