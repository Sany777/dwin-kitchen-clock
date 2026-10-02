cmake_minimum_required(VERSION 3.16)
project(notification_utils_host_tests C)

enable_testing()

add_executable(test_notification_utils
  test_notification_utils.c
  ../../notification_utils.c
)
target_include_directories(test_notification_utils PRIVATE ../../includes)
target_compile_features(test_notification_utils PRIVATE c_std_11)
add_test(NAME notification_utils COMMAND test_notification_utils)
