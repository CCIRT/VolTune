set mydir [file dirname [file normalize [info script]]]
source $mydir/TxRx.tcl
set_property -dict [list \
  CONFIG.identical_val_tx_line_rate {10.0} \
  CONFIG.identical_val_rx_line_rate {10.0} \
  CONFIG.gt0_val_tx_line_rate {10.0} \
  CONFIG.gt0_val_rx_line_rate {10.0}] [get_ips TxRx5g]
