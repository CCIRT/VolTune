define_vivado(loopback-5g
  TOP         TxRx5g_exdes
  BOARD       *kc705*
  TCL1        ./ip/TxRx5g.tcl
  RTL         ./rtl
              ./data/gt_rom_init_rx.dat
              ./data/gt_rom_init_tx.dat
  CONSTRAINT  ./constraints/TxRx5g_exdes.xdc
)
