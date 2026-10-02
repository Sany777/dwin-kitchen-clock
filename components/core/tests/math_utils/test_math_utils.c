#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>

#include "math_utils.h"

static void test_scales_negative_and_positive_values(void) {
  const float values[] = {-10.0f, 0.0f, 10.0f};
  uint16_t output[3];

  assert(map_float_series_to_y(values, 3, 100, output));
  assert(output[0] == 0);
  assert(output[1] == 50);
  assert(output[2] == 100);
}

static void test_keeps_existing_minimum_range_behavior(void) {
  const float values[] = {5.0f, 5.0f};
  uint16_t output[2];

  assert(map_float_series_to_y(values, 2, 100, output));
  assert(output[0] == 50);
  assert(output[1] == 50);
}

static void test_scales_single_and_one_sided_series(void) {
  const float single[] = {5.0f};
  const float positive[] = {2.0f, 7.0f};
  const float negative[] = {-7.0f, -2.0f};
  uint16_t output[2] = {0};

  assert(map_float_series_to_y(single, 1, 100, output));
  assert(output[0] == 50);
  assert(map_float_series_to_y(positive, 2, 100, output));
  assert(output[0] == 20 && output[1] == 70);
  assert(map_float_series_to_y(negative, 2, 100, output));
  assert(output[0] == 0 && output[1] == 50);
}

static void test_rejects_missing_input_or_empty_series(void) {
  const float values[] = {1.0f};
  uint16_t output[1];

  assert(!map_float_series_to_y(NULL, 1, 100, output));
  assert(!map_float_series_to_y(values, 1, 100, NULL));
  assert(!map_float_series_to_y(values, 0, 100, output));
}

static void test_rejects_non_finite_series_without_changing_output(void) {
  const float nan_values[] = {1.0f, NAN};
  const float infinite_values[] = {INFINITY};
  uint16_t output[] = {17, 29};

  assert(!map_float_series_to_y(nan_values, 2, 100, output));
  assert(output[0] == 17 && output[1] == 29);
  assert(!map_float_series_to_y(infinite_values, 1, 100, output));
  assert(output[0] == 17 && output[1] == 29);
}

static void test_parses_finite_decimal_values(void) {
  float value = 0.0f;

  assert(parse_float_string("12.75", &value));
  assert(fabsf(value - 12.75f) < 0.0001f);
  assert(parse_float_string("-3.5 \t", &value));
  assert(fabsf(value + 3.5f) < 0.0001f);
  assert(parse_float_string(" +.25\n", &value));
  assert(fabsf(value - 0.25f) < 0.0001f);
  assert(parse_float_string("-0", &value));
  assert(value == 0.0f && signbit(value));
}

static void test_rejects_invalid_or_out_of_range_values(void) {
  float value = 7.0f;

  assert(!parse_float_string(NULL, &value));
  assert(!parse_float_string("", &value));
  assert(!parse_float_string(" \t\n", &value));
  assert(!parse_float_string("+", &value));
  assert(!parse_float_string("12.3xyz", &value));
  assert(!parse_float_string("1e9999", &value));
  assert(!parse_float_string("1e-9999", &value));
  assert(!parse_float_string("nan", &value));
  assert(!parse_float_string("inf", &value));
  assert(!parse_float_string("1 2", &value));
  assert(value == 7.0f);
  assert(!parse_float_string("1", NULL));
}

static void test_parses_integer_strings_strictly(void) {
  int value = 4;

  assert(parse_int_string("255", &value));
  assert(value == 255);
  assert(parse_int_string("-12 \n", &value));
  assert(value == -12);
  assert(parse_int_string("+0", &value));
  assert(value == 0);
  assert(parse_int_string("2147483647", &value));
  assert(value == INT_MAX);
  assert(parse_int_string("-2147483648", &value));
  assert(value == INT_MIN);
  assert(!parse_int_string("12px", &value));
  assert(!parse_int_string("1 2", &value));
  assert(!parse_int_string("", &value));
  assert(!parse_int_string(" \t", &value));
  assert(!parse_int_string("+", &value));
  assert(!parse_int_string("999999999999999999999", &value));
  assert(!parse_int_string("2147483648", &value));
  assert(!parse_int_string("-2147483649", &value));
  assert(value == INT_MIN);
  assert(!parse_int_string("1", NULL));
}

static void test_parses_int64_boundaries_and_rejects_invalid_values(void) {
  int64_t value = 123;

  assert(parse_int64_string("9223372036854775807", &value));
  assert(value == INT64_MAX);
  assert(parse_int64_string("-9223372036854775808", &value));
  assert(value == INT64_MIN);
  assert(parse_int64_string("1700000000123", &value));
  assert(value == INT64_C(1700000000123));

  assert(!parse_int64_string(NULL, &value));
  assert(!parse_int64_string("", &value));
  assert(!parse_int64_string("123ms", &value));
  assert(!parse_int64_string("9223372036854775808", &value));
  assert(!parse_int64_string("-9223372036854775809", &value));
  assert(value == INT64_C(1700000000123));
  assert(!parse_int64_string("1", NULL));
}

int main(void) {
  test_scales_negative_and_positive_values();
  test_keeps_existing_minimum_range_behavior();
  test_scales_single_and_one_sided_series();
  test_rejects_missing_input_or_empty_series();
  test_rejects_non_finite_series_without_changing_output();
  test_parses_finite_decimal_values();
  test_rejects_invalid_or_out_of_range_values();
  test_parses_integer_strings_strictly();
  test_parses_int64_boundaries_and_rejects_invalid_values();
  return 0;
}
