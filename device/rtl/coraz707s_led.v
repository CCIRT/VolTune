module coraz707s_led #(
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

  input [WIDTH-1: 0]   counter,

  output wire led0_b,
  output wire led0_g,
  output wire led0_r,

  output wire led1_b,
  output wire led1_g,
  output wire led1_r
);

  wire toggle = ~counter[24:0] == 'd0;

  reg l;
  always @(posedge clock)
    if (!resetn) l <= 1'b1;
    else if (toggle) l <= ~l;

  reg [2:0] rgb0, rgb1;

  always @(posedge clock)
    if (!resetn) begin
      rgb0 <= 3'b000;
      rgb1 <= 3'b000;
    end else if (fatalErrorLED) begin
      rgb0 <= 3'b100;
      rgb1 <= {l, 2'b00};
    end else if (runLED) begin
      rgb0 <= {l, l, l};
      rgb1 <= 3'b000;
    end else if (okLED) begin
      rgb0 <= 3'b010;
      rgb1 <= 3'b000;
    end else if (failLED) begin
      rgb0 <= 3'b000;
      rgb1 <= 3'b001;
    end else begin
      rgb0 <= 3'b000;
      rgb1 <= 3'b000;
    end

  OBUF l0r( .O(led0_r), .I(rgb0[2]));
  OBUF l0g( .O(led0_g), .I(rgb0[1]));
  OBUF l0b( .O(led0_b), .I(rgb0[0]));
  OBUF l1r( .O(led1_r), .I(rgb1[2]));
  OBUF l1g( .O(led1_g), .I(rgb1[1]));
  OBUF l1b( .O(led1_b), .I(rgb1[0]));

endmodule
