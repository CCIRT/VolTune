set line_rate 5.0
set ref_clk 125.000
set pll CPLL
set_property generic LED_SPEED=2'b01 [current_fileset]
set mydir [file dirname [file normalize [info script]]]
add_files $mydir/loopback_gt_wrapper_cpll.v
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
