cmake_minimum_required(VERSION 3.16)
project(string_utils_host_tests C)

enable_testing()

add_executable(test_string_utils
  test_string_utils.c
  ../../string_utils.c
)
target_include_directories(test_string_utils PRIVATE ../../includes)
target_compile_features(test_string_utils PRIVATE c_std_11)
add_test(NAME string_utils COMMAND test_string_utils)
