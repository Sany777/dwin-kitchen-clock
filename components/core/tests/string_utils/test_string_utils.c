#include <assert.h>
#include <string.h>

#include "string_utils.h"

static void test_copies_terminated_string(void) {
  const char source[] = "clock";
  char destination[8];

  assert(copy_bounded_string(destination, sizeof(destination), source,
                             sizeof(source)));
  assert(strcmp(destination, "clock") == 0);
}

static void test_truncates_and_terminates_destination(void) {
  const char source[] = "kitchen-clock";
  char destination[8];

  assert(!copy_bounded_string(destination, sizeof(destination), source,
                              sizeof(source)));
  assert(strcmp(destination, "kitchen") == 0);
}

static void test_exact_fit_empty_and_single_byte_destinations(void) {
  const char exact[] = "1234567";
  const char empty[] = "";
  char destination[8];
  char single_byte[1] = {'x'};

  assert(copy_bounded_string(destination, sizeof(destination), exact,
                             sizeof(exact)));
  assert(strcmp(destination, exact) == 0);
  assert(copy_bounded_string(destination, sizeof(destination), empty,
                             sizeof(empty)));
  assert(destination[0] == '\0');
  assert(!copy_bounded_string(single_byte, sizeof(single_byte), exact,
                              sizeof(exact)));
  assert(single_byte[0] == '\0');
}

static void test_handles_nonterminated_source_buffer(void) {
  const char source[] = {'a', 'b', 'c', 'd'};
  char destination[8];

  assert(!copy_bounded_string(destination, sizeof(destination), source,
                              sizeof(source)));
  assert(strcmp(destination, "abcd") == 0);
}

static void test_clears_unused_destination_bytes(void) {
  const char source[] = "xy";
  char destination[8];
  memset(destination, 'z', sizeof(destination));

  assert(copy_bounded_string(destination, sizeof(destination), source,
                             sizeof(source)));
  assert(destination[0] == 'x' && destination[1] == 'y');
  for (size_t i = 2; i < sizeof(destination); i++) {
    assert(destination[i] == '\0');
  }
}

static void test_rejects_invalid_arguments(void) {
  const char source[] = "data";
  char destination[8] = "keep";

  assert(!copy_bounded_string(NULL, sizeof(destination), source,
                              sizeof(source)));
  assert(!copy_bounded_string(destination, 0, source, sizeof(source)));
  assert(!copy_bounded_string(destination, sizeof(destination), NULL, 1));
  assert(!copy_bounded_string(destination, sizeof(destination), source, 0));
  assert(strcmp(destination, "keep") == 0);
}

static void test_copies_full_width_text_field(void) {
  const char source[] = "12345678";
  char destination[8];

  assert(copy_text_field(destination, sizeof(destination), source,
                         sizeof(source)) == sizeof(destination));
  assert(memcmp(destination, source, sizeof(destination)) == 0);
}

static void test_text_field_truncates_and_zero_fills(void) {
  const char source[] = "abcdefghijk";
  char destination[5];

  assert(copy_text_field(destination, sizeof(destination), source,
                         sizeof(source)) == sizeof(destination));
  assert(memcmp(destination, "abcde", sizeof(destination)) == 0);
}

static void test_text_field_supports_empty_and_unterminated_sources(void) {
  const char empty[] = "";
  const char raw[] = {'a', 'b', 'c'};
  char destination[5];

  memset(destination, 'z', sizeof(destination));
  assert(copy_text_field(destination, sizeof(destination), empty,
                         sizeof(empty)) == 0);
  for (size_t i = 0; i < sizeof(destination); i++) {
    assert(destination[i] == '\0');
  }

  assert(copy_text_field(destination, sizeof(destination), raw,
                         sizeof(raw)) == sizeof(raw));
  assert(memcmp(destination, "abc\0\0", sizeof(destination)) == 0);
  assert(copy_text_field(destination, 1, raw, sizeof(raw)) == 1);
  assert(destination[0] == 'a');
}

static void test_text_field_rejects_invalid_arguments(void) {
  const char source[] = "data";
  char destination[5] = "keep";

  assert(copy_text_field(NULL, sizeof(destination), source, sizeof(source)) ==
         0);
  assert(copy_text_field(destination, 0, source, sizeof(source)) == 0);
  assert(copy_text_field(destination, sizeof(destination), NULL, 1) == 0);
  assert(copy_text_field(destination, sizeof(destination), source, 0) == 0);
  assert(strcmp(destination, "keep") == 0);
}

static void test_extracts_uri_suffix_after_exact_base_path(void) {
  assert(strcmp(uri_get_suffix("/sevepic/12", "/sevepic", 50), "12") == 0);
  assert(strcmp(uri_get_suffix("/sevepic/", "/sevepic", 50), "") == 0);
  assert(strcmp(uri_get_suffix("/img/a.jpg", "/img", 50), "a.jpg") == 0);
}

static void test_rejects_invalid_uri_prefix_and_length(void) {
  assert(uri_get_suffix(NULL, "/img", 50) == NULL);
  assert(uri_get_suffix("/img/a", NULL, 50) == NULL);
  assert(uri_get_suffix("/img/a", "/img", 0) == NULL);
  assert(uri_get_suffix("/img", "/img", 50) == NULL);
  assert(uri_get_suffix("/img/a", "/im", 50) == NULL);
  assert(uri_get_suffix("/imgx/a", "/img", 50) == NULL);
  assert(uri_get_suffix("/img/123456", "/img", 8) == NULL);
}

int main(void) {
  test_copies_terminated_string();
  test_truncates_and_terminates_destination();
  test_exact_fit_empty_and_single_byte_destinations();
  test_handles_nonterminated_source_buffer();
  test_clears_unused_destination_bytes();
  test_rejects_invalid_arguments();
  test_copies_full_width_text_field();
  test_text_field_truncates_and_zero_fills();
  test_text_field_supports_empty_and_unterminated_sources();
  test_text_field_rejects_invalid_arguments();
  test_extracts_uri_suffix_after_exact_base_path();
  test_rejects_invalid_uri_prefix_and_length();
  return 0;
}
