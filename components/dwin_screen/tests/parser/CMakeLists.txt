cmake_minimum_required(VERSION 3.16)
project(parser_host_tests C)

enable_testing()

add_executable(test_parser
  test_parser.c
  ../../parser.c
)
target_include_directories(test_parser PRIVATE
  stubs
  ../../includes
)
target_compile_features(test_parser PRIVATE c_std_11)
add_test(NAME parser COMMAND test_parser)
