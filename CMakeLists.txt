# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

#
# A dummy root CMake file of this repository.
# This file is NOT used in the actual build.
#
# The real root files are `device/CMakeLists.txt` and `host/CMakeLists.txt`.
# If `-DBUILD_FPGA=ON` is specified, it works as `device/CMakeLists.txt`.
# If `-DBUILD_FPGA=ON` is NOT specified, it works as `host/CMakeLists.txt`.
# If `-DBUILD_HOST=ON` is specified, it works as `host/CMakeLists.txt`.
#
# If you are using VSCode (with C/C++ Extension Pack),
# you can define a CMake tool kit like the following and specify it
# for this repository to enable C++ completion for HLS development.
#
#  {
#    "name": "Vivado 2022.1",
#    "compilers": {
#      "C": "/usr/bin/gcc",
#      "CXX": "/usr/bin/g++"
#    },
#    "cmakeSettings": {
#      "VITIS_HLS_ROOT": "/tools/Xilinx/Vitis_HLS/2022.1",
#      "VIVADO_ROOT": "/tools/Xilinx/Vivado/2022.1",
#      "BUILD_FPGA": "ON",
#      "BUILD_HOST": "ON"
#    }
#  }
#
cmake_minimum_required(VERSION 3.14)
project(fpga_power_control)

if (BUILD_FPGA)
  add_subdirectory(device)
  if (BUILD_HOST)
    add_subdirectory(host)
  endif()
else()
  add_subdirectory(host)
endif()
