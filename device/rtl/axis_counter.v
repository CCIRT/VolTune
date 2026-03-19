module axis_counter #(
  parameter WIDTH = 64
) (
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF m_axis_cnt:s_axis_clear,ASSOCIATED_RESET resetn" *)
  input wire clock,

  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire resetn,

  output wire [WIDTH-1:0] counter,

  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis_cnt TDATA" *)
  output wire [WIDTH-1:0] m_axis_cnt_TDATA,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis_cnt TVALID" *)
  output wire             m_axis_cnt_TVALID,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis_cnt TREADY" *)
  input wire              m_axis_cnt_TREADY,// ignore

  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis_clear TDATA" *)
  input wire [7:0] s_axis_clear_TDATA, // ignore value
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis_clear TVALID" *)
  input wire       s_axis_clear_TVALID,
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 s_axis_clear TREADY" *)
  output wire      s_axis_clear_TREADY
);

  reg [WIDTH-1:0] c;

  wire clear = s_axis_clear_TVALID;

  always @(posedge clock)
    if (!resetn)    c <= {WIDTH{1'b0}};
    else if (clear) c <= {WIDTH{1'b0}};
    else            c <= c + 1'b1;

  assign counter = c;
  assign m_axis_cnt_TVALID = 1'b1;
  assign m_axis_cnt_TDATA = c;
  assign s_axis_clear_TREADY = 1'b1;
endmodule
