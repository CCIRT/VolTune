# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

cmake_minimum_required(VERSION 3.0)

project(voltage)
set(CMAKE_CXX_STANDARD 17)

add_executable(voltage-measure
  src/main.cpp
  src/args.cpp
  src/config.cpp
)
target_link_libraries(voltage-measure util_lib xsdb_lib)
set_target_properties(voltage-measure
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
# ---------------------------------------------------
#  Test
# ---------------------------------------------------
set(VOLTAGE_PROJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(test/test_config.hpp.ini ${CMAKE_CURRENT_BINARY_DIR}/inc/test_config.hpp)
add_executable(gtest-voltage
  test/main.cpp
  src/args.cpp
  src/config.cpp
  src/result.cpp
)
target_include_directories(gtest-voltage
  PUBLIC
    src
    ${CMAKE_CURRENT_BINARY_DIR}/inc
)
target_link_libraries(gtest-voltage
  util_lib
  gtest_main
)
target_compile_options(gtest-voltage
  PRIVATE "-g"
)
add_test(
  NAME test-voltage
  COMMAND $<TARGET_FILE:gtest-voltage>
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
