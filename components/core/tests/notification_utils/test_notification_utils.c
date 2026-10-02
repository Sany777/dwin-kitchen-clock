#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "notification_utils.h"

static void test_formats_slots_in_notification_then_day_order(void) {
  const uint8_t schedule[] = {
      7, 5, 23, 30, 8, 10, 199, 15,
  };
  char output[17];

  assert(format_notification_schedule(schedule, sizeof(schedule), 2, 2, output,
                                      sizeof(output)));
  assert(strcmp(output, "0705081023309915") == 0);
}

static void test_rejects_invalid_or_insufficient_buffers_without_writing(void) {
  const uint8_t schedule[] = {7, 5, 8, 10};
  char output[9] = "preserve";

  assert(!format_notification_schedule(NULL, sizeof(schedule), 1, 2, output,
                                       sizeof(output)));
  assert(!format_notification_schedule(schedule, sizeof(schedule), 1, 2, NULL,
                                       sizeof(output)));
  assert(!format_notification_schedule(schedule, sizeof(schedule), 0, 2,
                                       output, sizeof(output)));
  assert(!format_notification_schedule(schedule, sizeof(schedule), 1, 0,
                                       output, sizeof(output)));
  assert(!format_notification_schedule(schedule, sizeof(schedule) - 1, 1, 2,
                                       output, sizeof(output)));
  assert(!format_notification_schedule(schedule, sizeof(schedule), 1, 2,
                                       output, sizeof(output) - 1));
  assert(!format_notification_schedule(schedule, sizeof(schedule), SIZE_MAX, 2,
                                       output, sizeof(output)));
  assert(strcmp(output, "preserve") == 0);
}

int main(void) {
  test_formats_slots_in_notification_then_day_order();
  test_rejects_invalid_or_insufficient_buffers_without_writing();
  return 0;
}
