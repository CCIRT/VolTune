`default_nettype none
`timescale 1ns / 1ps

module clk_counter #(
  parameter integer MAX_CLK1_COUNT = 10000000
)(
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_RESET rstn1" *)
  input wire clk1,
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire rstn1,

  input wire run,                         // run count
  output wire [31:0] counter_with_valid,  // {valid, clk2 counter}

  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_RESET rstn2" *)
  input wire clk2,
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire rstn2
);

  wire [30:0] counter; // clk2 counter
  wire valid;          // counter is valid
  assign counter_with_valid = {valid, counter};

  // ------------------------------ clk2 -------------------------------

  wire run2_from_clk1; // from clk1
  (* ASYNC_REG = "TRUE" *) reg run2_0, run2_1; // async run2_from_clk1
  reg run2;
  reg [30:0] cnt2; // counter for clk2
  wire clear_cnt2 = run2_1 && !run2; // reset cnt2

  always @(posedge clk2)
    if (!rstn2) {run2_0, run2_1, run2} <= 3'd0;
    else        {run2_0, run2_1, run2} <= {run2_from_clk1, run2_0, run2_1};

  always @(posedge clk2)
    if (!rstn2)          cnt2 <= 31'd0;
    else if (clear_cnt2) cnt2 <= 31'd0;
    else if (run2)       cnt2 <= cnt2 + 1'b1;

  // ------------------------------ clk1 -------------------------------

  // ------ ASYNC ----------
  (* ASYNC_REG = "TRUE" *) reg clk1_run2_0, clk1_run2;
  reg [30:0] clk1_cnt2; //** IMPORTANT: Set max delay from cnt2  **
  // -----------------------


  reg run1; // process is busy
  reg run1_cnt; // cnt1 is busy
  reg done; // process is done
  wire capture = !run1_cnt && clk1_run2; // capture cnt2
  reg [30:0] cnt1; // counter for clk1
  wire cnt1_full = cnt1 == MAX_CLK1_COUNT;

  assign run2_from_clk1 = run1_cnt;

  reg run_0;
  wire posedge_run = run && !run_0;

  wire accept_run = posedge_run && !run1;

  always @(posedge clk1)
    if (!rstn1) run_0 <= 1'b0;
    else        run_0 <= run;


  always @(posedge clk1)
    if (!rstn1) {clk1_run2_0, clk1_run2} <= 3'd0;
    else        {clk1_run2_0, clk1_run2} <= {run2_from_clk1, run2_0};

  always @(posedge clk1)
    if (!rstn1)       clk1_cnt2 <= 31'd0;
    else if (capture) clk1_cnt2 <= cnt2;

  always @(posedge clk1)
    if (!rstn1)          done <= 1'b0;
    else if (accept_run) done <= 1'b0;
    else if (capture)    done <= 1'b1;

  always @(posedge clk1)
    if (!rstn1)          run1 <= 1'b0;
    else if (accept_run) run1 <= 1'b1;
    else if (done)       run1 <= 1'b0;

  always @(posedge clk1)
    if (!rstn1)                  cnt1 <= 31'd0;
    else if (accept_run)         cnt1 <= 31'd0;
    else if (run1 && !cnt1_full) cnt1 <= cnt1 + 1'b1;

  always @(posedge clk1)
    if (!rstn1)          run1_cnt <= 1'b0;
    else if (accept_run) run1_cnt <= 1'b1;
    else if (cnt1_full)  run1_cnt <= 1'b0;

  // ----------- Output -----------------------------------
  assign counter = clk1_cnt2;
  assign valid = done;

endmodule


`default_nettype wire
