`default_nettype none

module latency_checker (
  input  wire        clk,
  input  wire        rst_n,
  input  wire  [7:0] rx_ctrl,
  input  wire        tx_data_valid,
  output wire [31:0] latency_cycle
);

  // Snatch up aligned_signal.
  wire aligned_signal = (rx_ctrl == 8'h00);

  // Detect positive edge of tx_data_valid
  reg  tx_data_valid_reg = 0;
  wire tx_data_valid_pos_edge = !tx_data_valid_reg & tx_data_valid;
  always @ (posedge clk) begin
    if (!rst_n) begin
      tx_data_valid_reg <= 0;
    end else begin
      tx_data_valid_reg <= tx_data_valid;
    end
  end

  // Detect positive edge of aligned_signal
  reg  aligned_signal_reg = 0;
  wire aligned_signal_pos_edge = !aligned_signal_reg & aligned_signal;
  always @ (posedge clk) begin
    if (!rst_n) begin
      aligned_signal_reg <= 0;
    end else begin
      aligned_signal_reg <= aligned_signal;
    end
  end

  // Assert count_enable when positive edge of tx_data_valid
  // until positive edge of aligned_signal
  reg count_enable_reg = 1'b0;
  reg count_finish_reg = 1'b0;
  always @ (posedge clk) begin
    if (!rst_n) begin
      count_enable_reg <= 1'b0;
      count_finish_reg <= 1'b0;
    end else begin
      if (aligned_signal_pos_edge) begin
        count_enable_reg <= 1'b0;
        count_finish_reg <= 1'b1;
      end else if (tx_data_valid_pos_edge & !count_finish_reg) begin
        count_enable_reg <= 1'b1;
      end
    end
  end

  // Count-up latency
  reg [31:0] latency_cycle_reg = 'd0;
  assign latency_cycle = latency_cycle_reg;
  always @ (posedge clk) begin
    if (!rst_n) begin
      latency_cycle_reg <= 'd0;
    end else begin
      if (count_enable_reg) begin
        latency_cycle_reg <= latency_cycle_reg + 'd1;
      end
    end
  end

endmodule

`default_nettype wire
