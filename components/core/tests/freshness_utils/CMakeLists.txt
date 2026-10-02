cmake_minimum_required(VERSION 3.16)
project(freshness_utils_host_tests C)

enable_testing()

add_executable(test_freshness_utils
  test_freshness_utils.c
  ../../freshness_utils.c
)
target_include_directories(test_freshness_utils PRIVATE ../../includes)
target_compile_features(test_freshness_utils PRIVATE c_std_11)
add_test(NAME freshness_utils COMMAND test_freshness_utils)
