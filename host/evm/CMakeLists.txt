# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

cmake_minimum_required(VERSION 3.0)

project(evm)
set(CMAKE_CXX_STANDARD 17)

add_executable(evm_voltage
  voltage.cpp
)

add_executable(evm_power
  power.cpp
)

target_link_libraries(evm_voltage util_lib xsdb_lib)
target_link_libraries(evm_power util_lib xsdb_lib)
target_include_directories(evm_voltage
  PUBLIC ${DEV_HLS_COMMON_DIR}
)
target_include_directories(evm_power
  PUBLIC ${DEV_HLS_COMMON_DIR}
)

set_target_properties(evm_voltage
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
set_target_properties(evm_power
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
