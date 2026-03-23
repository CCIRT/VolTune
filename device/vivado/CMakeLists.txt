# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

add_subdirectory(design-example)
add_subdirectory(power)
add_subdirectory(voltage)

if (EVM) # cmake .. -DEVM=ON
  include(FetchContent)
  FetchContent_Declare(
    xilinxboardstore
    GIT_REPOSITORY https://github.com/Xilinx/XilinxBoardStore.git
    GIT_TAG d4e7c68d16042b32a88790162d9b2b5642ca476b
  )
  FetchContent_MakeAvailable(xilinxboardstore)
  add_subdirectory(evm)
endif()
