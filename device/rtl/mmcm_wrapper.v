`default_nettype none

// This module is the wrapper of MMCM.
// This module has only minimum interfaces.
module mmcm_wrapper #(
  parameter CLKFBOUT_MULT_F  = 1,
  parameter DIVCLK_DIVIDE    = 1,
  parameter CLKIN1_PERIOD    = 10,
  parameter CLKOUT0_DIVIDE_F = 1,
  parameter CLKOUT1_DIVIDE   = 1,
  parameter CLKOUT2_DIVIDE   = 1,
  parameter CLKOUT3_DIVIDE   = 1
) (
  input  wire clkin1,
  output wire clkout0,
  output wire clkout1,
  output wire clkout2,
  output wire clkout3,
  input  wire rst,
  output wire locked
);
  // Clock buffers
  wire clkin1_int;
  BUFG buf_clkin1  (.I (clkin1),      .O (clkin1_int));
  wire clkout0_int;
  BUFG buf_clkout0 (.I (clkout0_int), .O (clkout0));
  wire clkout1_int;
  BUFG buf_clkout1 (.I (clkout1_int), .O (clkout1));
  wire clkout2_int;
  BUFG buf_clkout2 (.I (clkout2_int), .O (clkout2));
  wire clkout3_int;
  BUFG buf_clkout3 (.I (clkout3_int), .O (clkout3));

  // MMCM instance
  wire clkfbout;
  MMCME2_ADV #(
    .BANDWIDTH            ("OPTIMIZED"),
    .CLKOUT4_CASCADE      ("FALSE"),
    .COMPENSATION         ("ZHOLD"),
    .STARTUP_WAIT         ("FALSE"),
    .DIVCLK_DIVIDE        (DIVCLK_DIVIDE),
    .CLKFBOUT_MULT_F      (CLKFBOUT_MULT_F),
    .CLKFBOUT_PHASE       (0.000),
    .CLKFBOUT_USE_FINE_PS ("FALSE"),
    .CLKOUT0_DIVIDE_F     (CLKOUT0_DIVIDE_F),
    .CLKOUT0_PHASE        (0.000),
    .CLKOUT0_DUTY_CYCLE   (0.500),
    .CLKOUT0_USE_FINE_PS  ("FALSE"),
    .CLKIN1_PERIOD        (CLKIN1_PERIOD),
    .CLKOUT1_DIVIDE       (CLKOUT1_DIVIDE),
    .CLKOUT1_PHASE        (0.000),
    .CLKOUT1_DUTY_CYCLE   (0.500),
    .CLKOUT1_USE_FINE_PS  ("FALSE"),
    .CLKOUT2_DIVIDE       (CLKOUT2_DIVIDE),
    .CLKOUT2_PHASE        (0.000),
    .CLKOUT2_DUTY_CYCLE   (0.500),
    .CLKOUT2_USE_FINE_PS  ("FALSE"),
    .CLKOUT3_DIVIDE       (CLKOUT3_DIVIDE),
    .CLKOUT3_PHASE        (0.000),
    .CLKOUT3_DUTY_CYCLE   (0.500),
    .CLKOUT3_USE_FINE_PS  ("FALSE"),
    .REF_JITTER1          (0.010)
  ) mmcm_adv_i (
    .CLKFBOUT             (clkfbout),
    .CLKFBOUTB            (),
    .CLKOUT0              (clkout0_int),
    .CLKOUT0B             (),
    .CLKOUT1              (clkout1_int),
    .CLKOUT1B             (),
    .CLKOUT2              (clkout2_int),
    .CLKOUT2B             (),
    .CLKOUT3              (clkout3_int),
    .CLKOUT3B             (),
    .CLKOUT4              (),
    .CLKOUT5              (),
    .CLKOUT6              (),
    // Input clock control
    .CLKFBIN              (clkfbout),
    .CLKIN1               (clkin1_int),
    .CLKIN2               (1'b0),
    .CLKINSEL             (1'b1),
    .DADDR                (7'h0),
    .DCLK                 (1'b0),
    .DEN                  (1'b0),
    .DI                   (16'h0),
    .DO                   (),
    .DRDY                 (),
    .DWE                  (1'b0),
    // Ports for dynamic phase shift
    // Not used
    .PSCLK                (1'b0),
    .PSEN                 (1'b0),
    .PSINCDEC             (1'b0),
    .PSDONE               (),
    // Other control and status signals
    .LOCKED               (locked),
    .CLKINSTOPPED         (),
    .CLKFBSTOPPED         (),
    .PWRDWN               (1'b0),
    .RST                  (rst)
  );

endmodule

`default_nettype wire
