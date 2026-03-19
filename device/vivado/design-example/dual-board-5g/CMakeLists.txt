define_vivado(dual-board-5g
  TOP         TxRx5g_exdes
  BOARD       *kc705*
  TCL1        ../loopback-5g/ip/TxRx5g.tcl
  RTL         ./rtl/txrx5g_exdes.v
              ../loopback-5g/rtl/support/
              ../loopback-5g/rtl/txrx5g_gt_frame_check.v
              ../loopback-5g/rtl/txrx5g_gt_frame_gen.v
              ../loopback-5g/data/gt_rom_init_rx.dat
              ../loopback-5g/data/gt_rom_init_tx.dat
  CONSTRAINT  ./constraints/TxRx5g_exdes.xdc
)
