# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

cmake_minimum_required(VERSION 3.0)

project(util)
set(CMAKE_CXX_STANDARD 17)

add_library(util_lib
  STATIC
    src/bitstream.cpp
    src/trans_mode.cpp
)

target_include_directories(util_lib
  PUBLIC
    include/
)
target_include_directories(util_lib
  # SYSTEM: to avoid unknown pragma (#pragma HLS) warning
  SYSTEM PUBLIC ${DEV_HLS_COMMON_DIR}
)

target_link_libraries(util_lib xsdb_lib)

# -----------------------
# Test
# -----------------------
enable_testing()
# Generate dummy files for testing
set(UTIL_TEST_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/testfiles)
configure_file(test/config.hpp.ini ${CMAKE_CURRENT_BINARY_DIR}/inc/config.hpp)
file(MAKE_DIRECTORY ${UTIL_TEST_DIRECTORY})
file(WRITE ${UTIL_TEST_DIRECTORY}/foobar.bin "bin")
file(WRITE ${UTIL_TEST_DIRECTORY}/foobar.bit "bit")
file(WRITE ${UTIL_TEST_DIRECTORY}/foobar.txt "txt")
file(MAKE_DIRECTORY ${UTIL_TEST_DIRECTORY}/piyo.bin)

add_executable(gtest-util
  test/main.cpp
)
target_include_directories(gtest-util
  PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR}/inc
)
target_link_libraries(gtest-util
  util_lib
  gtest_main
)
add_test(
  NAME util_test
  COMMAND $<TARGET_FILE:gtest-util>
)
set_tests_properties(util_test PROPERTIES LABELS lib)
