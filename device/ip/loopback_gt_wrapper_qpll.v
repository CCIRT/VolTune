// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

`default_nettype none

// This module is the wrapper of Gigabit Transceiver(GT).
// This module has only minimum interfaces.
module loopback_gt_wrapper (
  // System clk, reset
  input  wire sysclk,
  input  wire soft_reset,
  // Differential clock input for GT
  input  wire gt_refclk_p,
  input  wire gt_refclk_n,
  // Differential signal output from GT
  output wire gt_tx_p,
  output wire gt_tx_n,
  // Differential signal input to GT
  input  wire gt_rx_p,
  input  wire gt_rx_n,
  // Send data input
  input  wire [63:0] tx_data,
  input  wire  [7:0] tx_ctrl,
  input  wire        tx_data_valid,
  output wire        tx_usrclk2,
  output wire        tx_fsm_reset_done,
  // Received data output
  output wire [63:0] rx_data,
  output wire  [7:0] rx_ctrl,
  input  wire        rx_data_valid,
  output wire        rx_usrclk2,
  output wire        rx_reset_done
);
  wire drpclk;  // 100 MHz

  // Wrapped MMCM instance for drpclk
  mmcm_wrapper #(
    .CLKFBOUT_MULT_F  (3.0),
    .DIVCLK_DIVIDE    (1.0),
    .CLKIN1_PERIOD    (5.0),
    .CLKOUT0_DIVIDE_F (6.0),
    .CLKOUT1_DIVIDE   (1.0),
    .CLKOUT2_DIVIDE   (1.0),
    .CLKOUT3_DIVIDE   (1.0)
  ) mmcm_drpclk (
    .clkin1   (sysclk), // 200 MHz
    .clkout0  (drpclk), // 100 MHz
    .clkout1  (),
    .clkout2  (),
    .clkout3  (),
    .rst      (),
    .locked   ()
  );

  // Sync register for tx_fsm_reset_done
  wire gt0_tx_fsm_reset_done_out;
  sync_reg #(
    .DATA_WIDTH (1)
  ) sync_reg_tx_fsm_reset_done (
    .clk      (tx_usrclk2),
    .data_in  (gt0_tx_fsm_reset_done_out),
    .data_out (tx_fsm_reset_done)
  );

  // Gigabit Transceiver(GT) instance
  gt gt_i (
    .sysclk_in                    (drpclk),
    .soft_reset_tx_in             (soft_reset),
    .soft_reset_rx_in             (soft_reset),
    .dont_reset_on_data_error_in  ('d0),
    .q2_clk0_gtrefclk_pad_p_in    (gt_refclk_p),
    .q2_clk0_gtrefclk_pad_n_in    (gt_refclk_n),
    .gt0_tx_mmcm_lock_out         (),
    .gt0_rx_mmcm_lock_out         (),
    .gt0_tx_fsm_reset_done_out    (gt0_tx_fsm_reset_done_out),
    .gt0_rx_fsm_reset_done_out    (),
    .gt0_data_valid_in            (tx_data_valid | rx_data_valid),
    .gt0_txusrclk_out             (),
    .gt0_txusrclk2_out            (tx_usrclk2),
    .gt0_rxusrclk_out             (),
    .gt0_rxusrclk2_out            (rx_usrclk2),
    //_________________________________________________________________________
    //GT0  (X1Y8)
    //____________________________CHANNEL PORTS________________________________
    //-------------------------- Channel - DRP Ports  --------------------------
    // Not used
    .gt0_drpaddr_in ('d0),
    .gt0_drpdi_in   ('d0),
    .gt0_drpdo_out  (),
    .gt0_drpen_in   ('d0),
    .gt0_drprdy_out (),
    .gt0_drpwe_in   ('d0),
    //------------------------- Digital Monitor Ports --------------------------
    // Not used
    .gt0_dmonitorout_out  (),
    //----------------------------- Loopback Ports -----------------------------
    // Not used
    .gt0_loopback_in      ('d0),
    //--------------------------- PCI Express Ports ----------------------------
    // Not used
    .gt0_rxrate_in        ('d0),
    //---------------------------- Power-Down Ports ----------------------------
    // Not used
    .gt0_rxpd_in          ('d0),
    .gt0_txpd_in          ('d0),
    //------------------- RX Initialization and Reset Ports --------------------
    // Not used
    .gt0_eyescanreset_in  (1'b0),
    .gt0_rxuserrdy_in     (1'b1),
    //------------------------ RX Margin Analysis Ports ------------------------
    // Not used
    .gt0_eyescandataerror_out (),
    .gt0_eyescantrigger_in    (1'b0),
    //----------------------- Receive Ports - CDR Ports ------------------------
    // Not used
    .gt0_rxcdrhold_in     (1'b0),
    //---------------- Receive Ports - FPGA RX interface Ports -----------------
    .gt0_rxdata_out       (rx_data),
    //----------------- Receive Ports - Pattern Checker Ports ------------------
    // Not used
    .gt0_rxprbserr_out    (),
    .gt0_rxprbssel_in     ('d0),
    //----------------- Receive Ports - Pattern Checker ports ------------------
    // Not used
    .gt0_rxprbscntreset_in  (1'b0),
    //---------------- Receive Ports - RX 8B/10B Decoder Ports -----------------
    // Not used
    .gt0_rxdisperr_out    (),
    .gt0_rxnotintable_out (),
    //------------------------- Receive Ports - RX AFE -------------------------
    .gt0_gtxrxp_in        (gt_rx_p),
    //---------------------- Receive Ports - RX AFE Ports ----------------------
    .gt0_gtxrxn_in        (gt_rx_n),
    //------------ Receive Ports - RX Byte and Word Alignment Ports ------------
    .gt0_rxbyteisaligned_out  (),
    .gt0_rxbyterealign_out    (),
    .gt0_rxcommadet_out       (),
    .gt0_rxmcommaalignen_in   (1'b1),
    .gt0_rxpcommaalignen_in   (1'b1),
    //------------------- Receive Ports - RX Equalizer Ports -------------------
    // Not used
    .gt0_rxdfeagcovrden_in  (1'b0),
    .gt0_rxdfelpmreset_in   (1'b0),
    .gt0_rxmonitorout_out   (),
    .gt0_rxmonitorsel_in    ('d0),
    //---------- Receive Ports - RX Fabric ClocK Output Control Ports ----------
    // Not used
    .gt0_rxratedone_out     (),
    //------------- Receive Ports - RX Fabric Output Control Ports -------------
    .gt0_rxoutclkfabric_out (),
    //----------- Receive Ports - RX Initialization and Reset Ports ------------
    // Not used
    .gt0_gtrxreset_in   (1'b0),
    .gt0_rxpcsreset_in  (1'b0),
    .gt0_rxpmareset_in  (1'b0),
    //---------------- Receive Ports - RX Margin Analysis ports ----------------
    // Not used
    .gt0_rxlpmen_in     (1'b1),
    //--------------- Receive Ports - RX Polarity Control Ports ----------------
    // Not used
    .gt0_rxpolarity_in  (1'b0),
    //----------------- Receive Ports - RX8B/10B Decoder Ports -----------------
    .gt0_rxchariscomma_out  (),
    .gt0_rxcharisk_out      (rx_ctrl),
    //------------ Receive Ports -RX Initialization and Reset Ports ------------
    .gt0_rxresetdone_out  (rx_reset_done),
    //---------------------- TX Configurable Driver Ports ----------------------
    // Not used
    .gt0_txpostcursor_in  ('d0),
    .gt0_txprecursor_in   ('d0),
    //------------------- TX Initialization and Reset Ports --------------------
    // Not used
    .gt0_gttxreset_in     (1'b0),
    .gt0_txuserrdy_in     (1'b1),
    //-------------- Transmit Ports - 8b10b Encoder Control Ports --------------
    // Not used
    .gt0_txchardispmode_in  ('d0),
    .gt0_txchardispval_in   ('d0),
    //------------------- Transmit Ports - PCI Express Ports -------------------
    // Not used
    .gt0_txelecidle_in    (1'b0),
    //---------------- Transmit Ports - Pattern Generator Ports ----------------
    // Not used
    .gt0_txprbsforceerr_in(1'b0),
    //-------------------- Transmit Ports - TX Buffer Ports --------------------
    // Not used
    .gt0_txbufstatus_out  (),
    //------------- Transmit Ports - TX Configurable Driver Ports --------------
    // Not used
    .gt0_txdiffctrl_in    ('d0),
    //---------------- Transmit Ports - TX Data Path interface -----------------
    .gt0_txdata_in        (tx_data),
    //-------------- Transmit Ports - TX Driver and OOB signaling --------------
    .gt0_gtxtxn_out       (gt_tx_n),
    .gt0_gtxtxp_out       (gt_tx_p),
    //--------- Transmit Ports - TX Fabric Clock Output Control Ports ----------
    // Not used
    .gt0_txoutclkfabric_out (),
    .gt0_txoutclkpcs_out    (),
    //------------------- Transmit Ports - TX Gearbox Ports --------------------
    .gt0_txcharisk_in     (tx_ctrl),
    //----------- Transmit Ports - TX Initialization and Reset Ports -----------
    // Not used
    .gt0_txpcsreset_in    (1'b0),
    .gt0_txpmareset_in    (1'b0),
    .gt0_txresetdone_out  (),
    //--------------- Transmit Ports - TX Polarity Control Ports ---------------
    // Not used
    .gt0_txpolarity_in    (1'b0),
    //---------------- Transmit Ports - pattern Generator Ports ----------------
    // Not used
    .gt0_txprbssel_in     ('d0),
    //____________________________COMMON PORTS________________________________
    .gt0_qplllock_out       (),
    .gt0_qpllrefclklost_out (),
    .gt0_qplloutclk_out     (),
    .gt0_qplloutrefclk_out  ()
  );

endmodule

`default_nettype wire
