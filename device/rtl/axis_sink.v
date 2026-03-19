module axis_sink#(
  parameter WIDTH = 32
) (
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF s_axis, ASSOCIATED_RESET resetn, FREQ_HZ 100000000" *)
  input wire ap_clk,

  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire resten,

  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TDATA" *)
  input wire [WIDTH-1:0] s_axis_TDATA,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TVALID" *)
  input wire s_axis_TVALID,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis TREADY" *)
  output wire s_axis_TREADY
);
  assign s_axis_TREADY = 1'b1;
endmodule
