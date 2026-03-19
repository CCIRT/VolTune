`default_nettype none

module ap_start_only_once (
  input  wire ap_clk,
  input  wire ap_rst_n,
  input  wire ap_start_in,
  output wire ap_start_out,
  input  wire ap_done_in
);

  // Detect positive edge of ap_start_in
  reg  ap_start_in_reg = 0;
  wire ap_start_in_pos_edge = !ap_start_in_reg & ap_start_in;
  always @ (posedge ap_clk) begin
    if (!ap_rst_n) begin
      ap_start_in_reg <= 0;
    end else begin
      ap_start_in_reg <= ap_start_in;
    end
  end

  // Detect negative edge of ap_done_in
  reg  ap_done_in_reg = 0;
  wire ap_done_in_pos_edge = !ap_done_in_reg & ap_done_in;
  always @ (posedge ap_clk) begin
    if (!ap_rst_n) begin
      ap_done_in_reg <= 0;
    end else begin
      ap_done_in_reg <= ap_done_in;
    end
  end

  // Assert ap_start_out when positive edge of ap_start_in
  // until positive edge of ap_done_in
  reg ap_start_out_reg = 0;
  assign ap_start_out = ap_start_out_reg & !ap_done_in;
  always @ (posedge ap_clk) begin
    if (!ap_rst_n) begin
      ap_start_out_reg <= 0;
    end else begin
      if (ap_start_in_pos_edge) begin
        ap_start_out_reg <= 1;
      end else if (ap_done_in_pos_edge) begin
        ap_start_out_reg <= 0;
      end
    end
  end

endmodule

`default_nettype wire
