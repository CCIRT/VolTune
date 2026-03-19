define_vivado(dual-board-10g
  TOP         TxRx5g_exdes
  BOARD       *kc705*
  TCL1        ../loopback-5g/ip/TxRx10g.tcl
  RTL         ./rtl/support/txrx5g_gt_usrclk_source.v
              ../dual-board-5g/rtl/txrx5g_exdes.v
              ../loopback-5g/rtl/support/txrx5g_clock_module.v
              ../loopback-5g/rtl/support/txrx5g_common_reset.v
              ../loopback-5g/rtl/support/txrx5g_common.v
              ../loopback-5g/rtl/support/txrx5g_support.v
              ../loopback-5g/rtl/txrx5g_gt_frame_check.v
              ../loopback-5g/rtl/txrx5g_gt_frame_gen.v
              ../loopback-5g/data/gt_rom_init_rx.dat
              ../loopback-5g/data/gt_rom_init_tx.dat
  CONSTRAINT  ./constraints/TxRx5g_exdes.xdc
)
