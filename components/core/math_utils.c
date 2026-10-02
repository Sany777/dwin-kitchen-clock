#include "math_utils.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

bool map_float_series_to_y(const float *values, size_t count, uint16_t height,
                           uint16_t *output) {
  if (values == NULL || output == NULL || count == 0) {
    return false;
  }

  float max_value = 0.0f;
  float min_value = 0.0f;
  for (size_t i = 0; i < count; i++) {
    if (!isfinite(values[i])) {
      return false;
    }
    if (values[i] > max_value) {
      max_value = values[i];
    }
    if (values[i] < min_value) {
      min_value = values[i];
    }
  }

  const float correction = min_value < 0.0f ? -min_value : 0.0f;
  float value_range = max_value - min_value;
  if (value_range < 10.0f) {
    value_range = 10.0f;
  }

  const float scale = (float)height / value_range;
  for (size_t i = 0; i < count; i++) {
    output[i] = (uint16_t)((values[i] + correction) * scale);
  }
  return true;
}

bool parse_float_string(const char *text, float *value) {
  if (text == NULL || value == NULL || *text == '\0') {
    return false;
  }

  errno = 0;
  char *end = NULL;
  const float parsed = strtof(text, &end);
  if (end == text || errno == ERANGE || !isfinite(parsed)) {
    return false;
  }

  while (isspace((unsigned char)*end)) {
    end++;
  }
  if (*end != '\0') {
    return false;
  }

  *value = parsed;
  return true;
}

static bool parse_long_long_string(const char *text, long long *value) {
  if (text == NULL || value == NULL || *text == '\0') {
    return false;
  }

  errno = 0;
  char *end = NULL;
  const long long parsed = strtoll(text, &end, 10);
  if (end == text || errno == ERANGE) {
    return false;
  }

  while (isspace((unsigned char)*end)) {
    end++;
  }
  if (*end != '\0') {
    return false;
  }

  *value = parsed;
  return true;
}

bool parse_int_string(const char *text, int *value) {
  if (value == NULL) {
    return false;
  }

  long long parsed;
  if (!parse_long_long_string(text, &parsed) || parsed < INT_MIN ||
      parsed > INT_MAX) {
    return false;
  }
  *value = (int)parsed;
  return true;
}

bool parse_int64_string(const char *text, int64_t *value) {
  if (value == NULL) {
    return false;
  }

  long long parsed;
  if (!parse_long_long_string(text, &parsed) || parsed < INT64_MIN ||
      parsed > INT64_MAX) {
    return false;
  }
  *value = (int64_t)parsed;
  return true;
}
