# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

add_custom_target(power_test_loopback
  COMMAND
    ${VIVADO_BIN_DIR}/xsdb ${CMAKE_CURRENT_SOURCE_DIR}/power_test_loopback.tcl
)
add_custom_target(power_test_dual
  COMMAND
    ${VIVADO_BIN_DIR}/xsdb ${CMAKE_CURRENT_SOURCE_DIR}/power_test_dual.tcl
)
