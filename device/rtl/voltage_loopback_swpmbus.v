`default_nettype none

// This is top module of FPGA design.
module voltage_loopback_swpmbus (
  // Reset
  input  wire reset,
  // System clock
  input  wire sysclk_p,     // 200 MHz
  input  wire sysclk_n,
  // Differential clock input for GT
  input  wire gt_refclk_p,  // 10.0g, 5.0g, 2.5g: 125.000 MHz
  input  wire gt_refclk_n,  //              7.5g: 117.188 MHz
  // Differential signal output from GT
  output wire gt_tx_p,
  output wire gt_tx_n,
  // Differential signal input to GT
  input  wire gt_rx_p,
  input  wire gt_rx_n,
  // PMBus
  inout  wire SCL,
  inout  wire SDA,
  // UART
  output wire rs232_uart_txd,
  input  wire rs232_uart_rxd,
  // LED
  output wire [7:0] led
);

  wire sysclk;  // Clock 200 MHz
  wire tx_usrclk2, rx_usrclk2;  // Clock
                                //   10.0g: 125.00 MHz
                                //    7.5g:  93.75 MHz
                                //    5.0g:  62.50 MHz
                                //    2.5g:  31.25 MHz
  wire tx_fsm_reset_done, rx_reset_done;  // Reset

  // Differential buffer for sysclk
  IBUFDS ibufds_sysclk (
    .I  (sysclk_p),
    .IB (sysclk_n),
    .O  (sysclk)
  );

  // IP Integrator instance.
  design_1 design_1_i (
    .rs232_uart_txd (rs232_uart_txd),
    .rs232_uart_rxd (rs232_uart_rxd),
    .SCL    (SCL),
    .SDA    (SDA),
    .led    (led),
    .reset  (reset),
    .sysclk (sysclk)
  );

  wire [63:0] tx_data_from_injector = 64'd0;
  wire  [7:0] tx_ctrl_from_injector = 8'd0;
  wire        tx_data_valid = 1'b0;
  wire [63:0] rx_data;
  wire  [7:0] rx_ctrl;
  wire        rx_data_valid = 1'b1;
  wire [63:0] tx_data = (tx_data_valid)? tx_data_from_injector: 64'hFEDCBA98765432BC;
  wire  [7:0] tx_ctrl = (tx_data_valid)? tx_ctrl_from_injector: 8'h01;
  // Gigabit Transceiver(GT) system instance.
  loopback_gt_wrapper loopback_gt_wrapper_i (
    // System clk, reset
    .sysclk             (sysclk),
    .soft_reset         (1'b0),
    // Differential clock input for GT
    .gt_refclk_p        (gt_refclk_p),
    .gt_refclk_n        (gt_refclk_n),
    // Differential signal output from GT
    .gt_tx_p            (gt_tx_p),
    .gt_tx_n            (gt_tx_n),
    // Differential signal input to GT
    .gt_rx_p            (gt_rx_p),
    .gt_rx_n            (gt_rx_n),
    // Send data input
    .tx_data            (tx_data),
    .tx_ctrl            (tx_ctrl),
    .tx_data_valid      (tx_data_valid),
    .tx_usrclk2         (tx_usrclk2),
    .tx_fsm_reset_done  (tx_fsm_reset_done),
    // Received data output
    .rx_data            (rx_data),
    .rx_ctrl            (rx_ctrl),
    .rx_data_valid      (rx_data_valid),
    .rx_usrclk2         (rx_usrclk2),
    .rx_reset_done      (rx_reset_done)
  );

endmodule

`default_nettype wire
