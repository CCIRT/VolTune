`default_nettype none

module sync_reg #(
  parameter DATA_WIDTH = 1
) (
  input  wire                   clk,
  input  wire [DATA_WIDTH-1:0]  data_in,
  output wire [DATA_WIDTH-1:0]  data_out
);

  (* ASYNC_REG = "TRUE" *) (* KEEP *) reg [DATA_WIDTH-1:0] reg_1, reg_2;

  assign data_out = reg_2;

  always @ (posedge clk) begin
    reg_1 <= data_in;
    reg_2 <= reg_1;
  end

endmodule

`default_nettype wire
