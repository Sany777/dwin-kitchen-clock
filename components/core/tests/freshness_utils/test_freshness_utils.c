#include <assert.h>
#include <stdint.h>

#include "freshness_utils.h"

static void test_accepts_data_within_age_limit(void) {
  assert(timestamp_is_fresh(1000, 1100, 100));
  assert(timestamp_is_fresh(1000, 1000, 0));
  assert(!timestamp_is_fresh(1000, 1001, 0));
}

static void test_rejects_expired_or_invalid_timestamps(void) {
  assert(!timestamp_is_fresh(1000, 1101, 100));
  assert(!timestamp_is_fresh(0, 1000, 100));
  assert(!timestamp_is_fresh(-1, 1000, 100));
  assert(!timestamp_is_fresh(1001, 1000, 100));
  assert(!timestamp_is_fresh(1000, 1000, -1));
}

static void test_cached_screen_data_expires_after_fifteen_hours(void) {
  const int64_t max_age_us = 15LL * 60 * 60 * 1000000;
  assert(timestamp_is_fresh(1, 1 + max_age_us, max_age_us));
  assert(!timestamp_is_fresh(1, 2 + max_age_us, max_age_us));
}

int main(void) {
  test_accepts_data_within_age_limit();
  test_rejects_expired_or_invalid_timestamps();
  test_cached_screen_data_expires_after_fifteen_hours();
  return 0;
}
