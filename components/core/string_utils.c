#include "string_utils.h"

#include <stdint.h>
#include <string.h>

bool copy_bounded_string(char *destination, size_t destination_size,
                         const char *source, size_t source_size) {
  if (destination == NULL || source == NULL || destination_size == 0 ||
      source_size == 0) {
    return false;
  }

  const char *terminator = memchr(source, '\0', source_size);
  const size_t source_length = terminator
                                   ? (size_t)(terminator - source)
                                   : source_size;
  const size_t copy_length = source_length < destination_size - 1
                                 ? source_length
                                 : destination_size - 1;
  memset(destination, 0, destination_size);
  memcpy(destination, source, copy_length);

  return terminator != NULL && source_length < destination_size;
}

size_t copy_text_field(char *destination, size_t destination_size,
                       const char *source, size_t source_size) {
  if (destination == NULL || source == NULL || destination_size == 0 ||
      source_size == 0) {
    return 0;
  }

  const char *terminator = memchr(source, '\0', source_size);
  const size_t source_length = terminator
                                   ? (size_t)(terminator - source)
                                   : source_size;
  const size_t copy_length = source_length < destination_size
                                 ? source_length
                                 : destination_size;
  memset(destination, 0, destination_size);
  memcpy(destination, source, copy_length);
  return copy_length;
}

const char *uri_get_suffix(const char *uri, const char *base_path,
                           size_t max_uri_length) {
  if (uri == NULL || base_path == NULL || max_uri_length == 0) {
    return NULL;
  }

  const size_t max_scan_length =
      max_uri_length == SIZE_MAX ? SIZE_MAX : max_uri_length + 1;
  const size_t base_length = strnlen(base_path, max_scan_length);
  const size_t uri_length = strnlen(uri, max_scan_length);
  if (base_length >= max_scan_length || uri_length <= base_length ||
      uri_length > max_uri_length || strncmp(uri, base_path, base_length) != 0 ||
      uri[base_length] != '/') {
    return NULL;
  }

  return uri + base_length + 1;
}
