# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

cmake_minimum_required(VERSION 3.14)
project(host)

set(DEVICE_NAME KC705) # use to determine whether KC705 or UCD9224EVM is on the HLS code.
set(EVM_DEVICE_NAME UCD9224EVM)


############# init cmake #####################################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
# avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()
find_package(GoogleTest REQUIRED MODULE)
###############################################################

# HLS commln
set(DEV_HLS_COMMON_DIR_REL ../device/hls/common)
get_filename_component(DEV_HLS_COMMON_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/${DEV_HLS_COMMON_DIR_REL}
  ABSOLUTE
)
if (NOT EXISTS ${DEV_HLS_COMMON_DIR})
  message(FATAL_ERROR "${DEV_HLS_COMMON_DIR_REL} is not exists.")
endif()
###############################################################

if (EVM) # cmake .. -DEVM=ON
  # Override
  set(DEVICE_NAME ${EVM_DEVICE_NAME})
endif()

add_compile_definitions(${DEVICE_NAME})

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-long-long -pedantic")
endif()

add_subdirectory(xsdb_wrapper)
add_subdirectory(util)
add_subdirectory(voltage)
add_subdirectory(power)
