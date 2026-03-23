// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

/**
 * Convert AXI Stream run and error signal to led on/off.
 * LED flashing and device-specific differences should be handled in a separate module.
 */
module axis_led_adaptor (
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF m_axis_run,ASSOCIATED_RESET resetn, FREQ_HZ 100000000" *)
  input wire clock,

  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire resetn,

  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis_run TDATA" *)
  input wire [7:0]      m_axis_run_TDATA, // [0]: run, [1]: ok, [2]: fail
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis_run TVALID" *)
  input wire            m_axis_run_TVALID,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis_run TREADY" *)
  output wire           m_axis_run_TREADY,// ignore
  input wire            error,

  output wire           runLED,
  output wire           okLED,
  output wire           failLED,
  output wire           fatalErrorLED
);

  reg e = 1'b0; // no reset
  always @(posedge clock)
    if (error) e <= 1'b1;

  reg [2:0] l;
  always @(posedge clock)
    if (!resetn) l <= 3'b00;
    else if (e || error) l <= 3'b000;
    else if (m_axis_run_TVALID) l <= m_axis_run_TDATA[2:0];

  assign fatalErrorLED = e;
  assign runLED = l[0];
  assign okLED  = l[1];
  assign failLED  = l[2];

  assign m_axis_run_TREADY = 1'b1;
endmodule
