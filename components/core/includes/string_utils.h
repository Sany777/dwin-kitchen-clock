#pragma once

#include <stdbool.h>
#include <stddef.h>

bool copy_bounded_string(char *destination, size_t destination_size,
                         const char *source, size_t source_size);
size_t copy_text_field(char *destination, size_t destination_size,
                       const char *source, size_t source_size);
const char *uri_get_suffix(const char *uri, const char *base_path,
                           size_t max_uri_length);
