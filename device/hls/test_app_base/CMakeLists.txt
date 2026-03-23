# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

project(test_app_base)

define_hls_project(test_app_base
  TOP     TestAppBase
  NAME    TestAppBase
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/test_app_base.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
  TB_SOURCES src/tb_test_app_base.cpp
  TB_LINK tbutil
)

set_target_properties(test_test_app_base
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
