module axis_empty#(
  parameter WIDTH = 32
) (
  input wire ap_clk,
  input wire resten,
  output wire [WIDTH-1:0] m_axis_TDATA,
  output wire m_axis_TVALID,
  input wire m_axis_TREADY
);

  assign m_axis_TDATA = 'd0;
  assign m_axis_TVALID = 1'b0;

endmodule
