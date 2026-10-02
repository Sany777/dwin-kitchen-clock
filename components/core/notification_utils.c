#include "notification_utils.h"

#include <stdint.h>

bool format_notification_schedule(const uint8_t *notification_data,
                                  size_t notification_data_size,
                                  size_t day_count,
                                  size_t notifications_per_day,
                                  char *output, size_t output_size) {
  if (notification_data == NULL || output == NULL || day_count == 0 ||
      notifications_per_day == 0 ||
      notifications_per_day > SIZE_MAX / day_count) {
    return false;
  }

  const size_t slot_count = day_count * notifications_per_day;
  if (slot_count > SIZE_MAX / 2 || slot_count > (SIZE_MAX - 1) / 4) {
    return false;
  }

  const size_t required_data_size = slot_count * 2;
  const size_t required_output_size = slot_count * 4 + 1;
  if (notification_data_size < required_data_size ||
      output_size < required_output_size) {
    return false;
  }

  size_t output_offset = 0;
  for (size_t notification = 0; notification < notifications_per_day;
       notification++) {
    for (size_t day = 0; day < day_count; day++) {
      const size_t data_offset =
          (day * notifications_per_day + notification) * 2;
      const unsigned int hour = notification_data[data_offset] % 100;
      const unsigned int minute = notification_data[data_offset + 1];
      output[output_offset++] = (char)('0' + hour / 10);
      output[output_offset++] = (char)('0' + hour % 10);
      output[output_offset++] = (char)('0' + minute / 10);
      output[output_offset++] = (char)('0' + minute % 10);
    }
  }
  output[output_offset] = '\0';
  return true;
}
