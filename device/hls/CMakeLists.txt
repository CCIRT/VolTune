# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

add_subdirectory(common)
add_subdirectory(tbutil)
add_subdirectory(data_aligner)
add_subdirectory(comma_injector)
add_subdirectory(data_checker)
add_subdirectory(test_app_base)
add_subdirectory(power_manager)
add_subdirectory(voltage_test_manager)
add_subdirectory(ber_test_manager)
if (EVM)
  add_subdirectory(evm_power_read)
endif()
