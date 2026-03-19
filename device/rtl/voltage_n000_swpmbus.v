`default_nettype none

// This is top module of FPGA design.
module voltage_n000_swpmbus (
  // Reset
  input  wire reset,
  // System clock
  input  wire sys_diff_clock_clk_p, // 200 MHz
  input  wire sys_diff_clock_clk_n,
  // PMBus
  inout  wire SCL,
  inout  wire SDA,
  // UART
  output wire rs232_uart_txd,
  input  wire rs232_uart_rxd,
  // LED
  output wire [7:0] led
);

  // IP Integrator instance.
  design_1 design_1_i (
    .rs232_uart_txd (rs232_uart_txd),
    .rs232_uart_rxd (rs232_uart_rxd),
    .SCL    (SCL),
    .SDA    (SDA),
    .led    (led),
    .reset  (reset),
    .sys_diff_clock_clk_p (sys_diff_clock_clk_p),
    .sys_diff_clock_clk_n (sys_diff_clock_clk_n)
  );

endmodule

`default_nettype wire
