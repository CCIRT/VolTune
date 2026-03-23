// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

`default_nettype none

// This is top module of FPGA design.
module loopback_swpmbus #(
  parameter LED_DIRECTION = 2'b11,
  parameter LED_SPEED = 2'b00
) (
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

  // LEDs
  wire ap_clk_jtag;
  wire [7:0] running_out_tdata;
  wire       running_out_tvalid;
  wire [7:0] error_out_tdata;
  wire       error_out_tvalid;

  wire led_int_clk;
  wire led_ext_clk;
  reg  led_running = 1'b0;
  reg  led_error   = 1'b0;

  always @ (posedge ap_clk_jtag) begin
    if (running_out_tvalid) led_running <= running_out_tdata[0];
    if (error_out_tvalid)   led_error   <= error_out_tdata[0];
  end

  assign led[1:0] = LED_DIRECTION;
  assign led[3:2] = LED_SPEED;
  assign led[4]   = led_int_clk;
  assign led[5]   = led_ext_clk;
  assign led[6]   = led_running;
  assign led[7]   = led_error;

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

  wire        reset_latency;
  wire [31:0] latency_cycle;
  wire [63:0] tx_data_from_injector;
  wire  [7:0] tx_ctrl_from_injector;
  wire        tx_data_valid;
  wire [63:0] rx_data;
  wire  [7:0] rx_ctrl;
  wire        rx_data_valid = 1'b1;
  // IP Integrator instance.
  design_1 design_1_i (
    .SCL    (SCL),
    .SDA    (SDA),
    .sysclk (sysclk),
    .sysrstn  (1'b1),
    .tx_aclk    (tx_usrclk2),
    .tx_aresetn (tx_fsm_reset_done),
    .rx_aclk    (rx_usrclk2),
    .rx_aresetn (rx_reset_done),
    .reset_latency (reset_latency),
    .latency_cycle (latency_cycle),
    .rs232_uart_txd (rs232_uart_txd),
    .rs232_uart_rxd (rs232_uart_rxd),
    .data_out_tdata   (tx_data_from_injector),
    .data_out_tvalid  (tx_data_valid),
    .data_out_tready  (1'b1),
    .ctrl_out_tdata   (tx_ctrl_from_injector),
    .ctrl_out_tvalid  (),
    .ctrl_out_tready  (1'b1),
    .data_in_tdata  (rx_data),
    .data_in_tvalid (1'b1),
    .data_in_tready (),
    .ctrl_in_tdata  (rx_ctrl),
    .ctrl_in_tvalid (1'b1),
    .ctrl_in_tready (),
    .ap_clk_jtag        (ap_clk_jtag),
    .running_out_tdata  (running_out_tdata),
    .running_out_tvalid (running_out_tvalid),
    .running_out_tready (1'b1),
    .error_out_tdata    (error_out_tdata),
    .error_out_tvalid   (error_out_tvalid),
    .error_out_tready   (1'b1),
    .led_int_clk        (led_int_clk),
    .led_ext_clk        (led_ext_clk)
  );

  wire [63:0] tx_data = (tx_data_valid)? tx_data_from_injector: 64'hFEDCBA98765432BC;
  wire  [7:0] tx_ctrl = (tx_data_valid)? tx_ctrl_from_injector: 8'h01;

  // Check latency
  wire tx_data_valid_int = (tx_ctrl == 8'h00);
  latency_checker latency_checker_i (
    .clk           (rx_usrclk2),
    .rst_n         (reset_latency),
    .rx_ctrl       (rx_ctrl),
    .tx_data_valid (tx_data_valid_int),
    .latency_cycle (latency_cycle)
  );

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
