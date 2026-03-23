// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

`default_nettype none

// This is top module of FPGA design.
module rx #(
  parameter LED_DIRECTION = 2'b10,
  parameter LED_SPEED = 2'b00
) (
  // System clock
  input  wire sysclk_p,     // 200 MHz
  input  wire sysclk_n,
  // Differential clock input for GT
  input  wire gt_refclk_p,  // 10.0g, 5.0g, 2.5g: 125.000 MHz
  input  wire gt_refclk_n,  //              7.5g: 117.188 MHz
  // Differential signal input to GT
  input  wire gt_rx_p,
  input  wire gt_rx_n,
  // PMBus
  inout  wire SCL,
  inout  wire SDA,
  // LED
  output wire [7:0] led,
  // Input for latency measurement.
  input  wire tx_data_valid_in
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

  wire sysclk;        // Clock 200 MHz
  wire rx_usrclk2;    // Clock
                      //   10.0g: 125.00 MHz
                      //    7.5g:  93.75 MHz
                      //    5.0g:  62.50 MHz
                      //    2.5g:  31.25 MHz
  wire rx_reset_done; // Reset

  // Differential buffer for sysclk
  IBUFDS ibufds_sysclk (
    .I  (sysclk_p),
    .IB (sysclk_n),
    .O  (sysclk)
  );

  wire [63:0] rx_data;
  wire  [7:0] rx_ctrl;
  wire        rx_data_valid = 1'b1;
  // Gigabit Transceiver(GT) system instance.
  rx_gt_wrapper rx_gt_wrapper_i (
    // System clk, reset
    .sysclk             (sysclk),
    .soft_reset         (1'b0),
    // Differential clock input for GT
    .gt_refclk_p        (gt_refclk_p),
    .gt_refclk_n        (gt_refclk_n),
    // Differential signal input to GT
    .gt_rx_p            (gt_rx_p),
    .gt_rx_n            (gt_rx_n),
    // Received data output
    .rx_data            (rx_data),
    .rx_ctrl            (rx_ctrl),
    .rx_data_valid      (rx_data_valid),
    .rx_usrclk2         (rx_usrclk2),
    .rx_reset_done      (rx_reset_done)
  );

  // Sync register for tx_data_valid
  wire tx_data_valid;
  sync_reg #(
    .DATA_WIDTH (1)
  ) sync_reg_tx_fsm_reset_done (
    .clk      (rx_usrclk2),
    .data_in  (tx_data_valid_in),
    .data_out (tx_data_valid)
  );

  // Check latency
  wire        reset_latency;
  wire [31:0] latency_cycle;
  latency_checker latency_checker_i (
    .clk           (rx_usrclk2),
    .rst_n         (reset_latency),
    .rx_ctrl       (rx_ctrl),
    .tx_data_valid (tx_data_valid),
    .latency_cycle (latency_cycle)
  );

  // IP Integrator instance.
  design_1 design_1_i (
    .sysclk   (sysclk),
    .sysrstn  (1'b1),
    .aclk     (rx_usrclk2),
    .aresetn  (rx_reset_done),
    .SCL      (SCL),
    .SDA      (SDA),
    .reset_latency  (reset_latency),
    .latency_cycle  (latency_cycle),
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

endmodule

`default_nettype wire
