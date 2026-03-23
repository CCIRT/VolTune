# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

set line_rate 7.5
set ref_clk 117.188
set pll QPLL
set_property generic LED_SPEED=2'b10 [current_fileset]
set mydir [file dirname [file normalize [info script]]]
add_files $mydir/loopback_gt_wrapper_qpll.v
source $mydir/gt_base.tcl
set_property -dict [list \
  CONFIG.gt_val_tx_pll $pll \
  CONFIG.gt_val_rx_pll $pll \
  CONFIG.identical_val_no_tx {false} \
  CONFIG.identical_val_no_rx {false} \
  CONFIG.identical_val_tx_line_rate $line_rate \
  CONFIG.gt0_val_tx_line_rate $line_rate \
  CONFIG.identical_val_rx_line_rate $line_rate \
  CONFIG.gt0_val_rx_line_rate $line_rate \
  CONFIG.identical_val_tx_reference_clock $ref_clk \
  CONFIG.identical_val_rx_reference_clock $ref_clk \
  CONFIG.gt0_val_tx_reference_clock $ref_clk \
  CONFIG.gt0_val_rx_reference_clock $ref_clk] [get_ips gt]
