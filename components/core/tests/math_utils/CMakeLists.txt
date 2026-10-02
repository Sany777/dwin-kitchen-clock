cmake_minimum_required(VERSION 3.16)
project(math_utils_host_tests C)

enable_testing()

add_executable(test_math_utils
  test_math_utils.c
  ../../math_utils.c
)
target_include_directories(test_math_utils PRIVATE ../../includes)
target_compile_features(test_math_utils PRIVATE c_std_11)
add_test(NAME math_utils COMMAND test_math_utils)
