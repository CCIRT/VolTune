module kc705_led #(
  parameter integer WIDTH = 64
)(
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
  (* X_INTERFACE_PARAMETER = "ASSOCIATED_RESET resetn, FREQ_HZ 100000000" *)
  input wire clock,

  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0  resetn RST" *)
  (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
  input wire resetn,

  input wire           runLED,
  input wire           okLED,
  input wire           failLED,
  input wire           fatalErrorLED,

  input wire [WIDTH-1: 0]   counter,

  output wire [7:0]    led
);

  wire toggle = (~counter[24:0]) == 'd0;

  reg l;
  always @(posedge clock)
    if (!resetn) l <= 1'b1;
    else if (toggle) l <= ~l;

  reg [7:0] v;
  always @(posedge clock)
    if (!resetn)            v <= 8'd0;
    else if (fatalErrorLED) v <=~8'd0;
    else if (runLED)        v <= {7'd0, l};
    else if (okLED)         v <= 8'd1;
    else if (failLED)       v <= 3'h55;
    else                    v <= 8'd0;

  OBUF u_obuf0( .I(v[0]), .O(led[0]));
  OBUF u_obuf1( .I(v[1]), .O(led[1]));
  OBUF u_obuf2( .I(v[2]), .O(led[2]));
  OBUF u_obuf3( .I(v[3]), .O(led[3]));
  OBUF u_obuf4( .I(v[4]), .O(led[4]));
  OBUF u_obuf5( .I(v[5]), .O(led[5]));
  OBUF u_obuf6( .I(v[6]), .O(led[6]));
  OBUF u_obuf7( .I(v[7]), .O(led[7]));

endmodule
