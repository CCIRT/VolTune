# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

cmake_minimum_required(VERSION 3.14)
project(device)

############# init cmake #####################################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
# avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()
find_package(HLS REQUIRED MODULE)
find_package(Vivado REQUIRED MODULE)
find_package(Vitis REQUIRED MODULE)
find_package(Util REQUIRED MODULE)
###############################################################


##############################################################
# Authority
##############################################################
set(HLS_VENDOR_NAME "Fixstars")    # HLS IP Vendor name
set(HLS_TAXONOMY "NotCategorized") # HLS IP Category

##############################################################
# KC705 Setting
##############################################################
set(FPGA_PART xc7k325tffg900-2)
set(FPGA_PERIOD 10) # 100MHz
set(FPGA_MACRO KC705) # use to determine whether KC705 or UCD9224EVM is on the HLS code.

if (MGTAVCC_ONLY) # cmake -DMGTAVCC_ONLY=ON
  list(APPEND FPGA_MACRO MGTAVCC_ONLY)
  # * `MGTAVCC_ONLY` macro
  #    This macro is used in hls/common/pm_common_host.hpp.
  #    If this macro is defined, isValidLane function returns ok only if the lane is MGTAVCC.
endif()

##############################################################
# UCD9224EVM-464 (TI evaluation board)
# When `-DEVM=ON` cmake option is specified,
# this setting is enabled.
##############################################################
set(EVM_FPGA_PART xc7z007sclg400-1)
set(EVM_FPGA_PERIOD 10) # 100MHz
set(EVM_FPGA_MACRO UCD9224EVM)

##############################################################
# Directories
##############################################################
set(BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(BUILD_HLS_DIR ${BUILD_DIR}/hls)
set(RTL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rtl)
set(CONSTRAINT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/constraint)
set(IP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ip)
set(VITIS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/vitis_src)

##############################################################
if (EVM) # cmake .. -DEVM=ON
  # Override
  message ("[INFO] EVM Build enable")
  set(FPGA_PART ${EVM_FPGA_PART})
  set(FPGA_PERIOD ${EVM_FPGA_PERIOD})
  set(FPGA_MACRO ${EVM_FPGA_MACRO})
endif()

add_subdirectory(hls)
add_subdirectory(vivado)
