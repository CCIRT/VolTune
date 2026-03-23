# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

# Pin settings
set_property PACKAGE_PIN AD12 [get_ports sysclk_p]
set_property PACKAGE_PIN AD11 [get_ports sysclk_n]
set_property PACKAGE_PIN J8   [get_ports gt_refclk_p]
set_property PACKAGE_PIN J7   [get_ports gt_refclk_n]
set_property IOSTANDARD LVDS     [get_ports sysclk_p]

# Clock constraints
create_clock -period 5.000 [get_ports sysclk_p]
create_clock -period 8.5333 [get_ports gt_refclk_p]
set_clock_groups -asynchronous \
  -group [get_clocks -include_generated_clocks sysclk_p] \
  -group [get_clocks -include_generated_clocks gt_refclk_p] \
  -group [get_clocks -include_generated_clocks tx_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i/TXOUTCLK]

# Place constraints
set_property LOC GTXE2_CHANNEL_X0Y8 [get_cells tx_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i]
