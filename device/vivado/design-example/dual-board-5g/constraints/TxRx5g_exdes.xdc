create_clock -period 8.000 [get_ports Q2_CLK0_GTREFCLK_PAD_P_IN]
create_clock -period 10.000 -name drpclk_in_i [get_ports DRP_CLK_IN_P]

set_false_path -to [get_pins -filter REF_PIN_NAME=~*CLR -of_objects [get_cells -hierarchical -filter {NAME =~ *_txfsmresetdone_r*}]]
set_false_path -to [get_pins -filter REF_PIN_NAME=~*D -of_objects [get_cells -hierarchical -filter {NAME =~ *_txfsmresetdone_r*}]]
################################# RefClk Location constraints #####################
set_property PACKAGE_PIN J8 [get_ports Q2_CLK0_GTREFCLK_PAD_P_IN]
set_property PACKAGE_PIN J7 [get_ports Q2_CLK0_GTREFCLK_PAD_N_IN]

set_property PACKAGE_PIN AD12 [get_ports DRP_CLK_IN_P]
set_property PACKAGE_PIN AD11 [get_ports DRP_CLK_IN_N]

set_property PACKAGE_PIN Y23 [get_ports RESET_OUT]
set_property PACKAGE_PIN Y24 [get_ports RESET_IN]
################################# mgt wrapper constraints #####################

##---------- Set placement for gt0_gtx_wrapper_i/GTXE2_CHANNEL ------
set_property LOC GTXE2_CHANNEL_X0Y8 [get_cells TxRx5g_support_i/TxRx5g_init_i/inst/TxRx5g_i/gt0_TxRx5g_i/gtxe2_i]

set_property PACKAGE_PIN Y29 [get_ports START]
set_property PACKAGE_PIN G12 [get_ports RESET]

set_property BITSTREAM.GENERAL.UNCONSTRAINEDPINS Allow [current_design]

create_debug_core u_ila_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0]
set_property ALL_PROBE_SAME_MU_CNT 1 [get_debug_cores u_ila_0]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_0]
set_property C_DATA_DEPTH 1024 [get_debug_cores u_ila_0]
set_property C_EN_STRG_QUAL false [get_debug_cores u_ila_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets gt0_rxusrclk2_i]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 64 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {gt0_frame_check/data_xored[0]} {gt0_frame_check/data_xored[1]} {gt0_frame_check/data_xored[2]} {gt0_frame_check/data_xored[3]} {gt0_frame_check/data_xored[4]} {gt0_frame_check/data_xored[5]} {gt0_frame_check/data_xored[6]} {gt0_frame_check/data_xored[7]} {gt0_frame_check/data_xored[8]} {gt0_frame_check/data_xored[9]} {gt0_frame_check/data_xored[10]} {gt0_frame_check/data_xored[11]} {gt0_frame_check/data_xored[12]} {gt0_frame_check/data_xored[13]} {gt0_frame_check/data_xored[14]} {gt0_frame_check/data_xored[15]} {gt0_frame_check/data_xored[16]} {gt0_frame_check/data_xored[17]} {gt0_frame_check/data_xored[18]} {gt0_frame_check/data_xored[19]} {gt0_frame_check/data_xored[20]} {gt0_frame_check/data_xored[21]} {gt0_frame_check/data_xored[22]} {gt0_frame_check/data_xored[23]} {gt0_frame_check/data_xored[24]} {gt0_frame_check/data_xored[25]} {gt0_frame_check/data_xored[26]} {gt0_frame_check/data_xored[27]} {gt0_frame_check/data_xored[28]} {gt0_frame_check/data_xored[29]} {gt0_frame_check/data_xored[30]} {gt0_frame_check/data_xored[31]} {gt0_frame_check/data_xored[32]} {gt0_frame_check/data_xored[33]} {gt0_frame_check/data_xored[34]} {gt0_frame_check/data_xored[35]} {gt0_frame_check/data_xored[36]} {gt0_frame_check/data_xored[37]} {gt0_frame_check/data_xored[38]} {gt0_frame_check/data_xored[39]} {gt0_frame_check/data_xored[40]} {gt0_frame_check/data_xored[41]} {gt0_frame_check/data_xored[42]} {gt0_frame_check/data_xored[43]} {gt0_frame_check/data_xored[44]} {gt0_frame_check/data_xored[45]} {gt0_frame_check/data_xored[46]} {gt0_frame_check/data_xored[47]} {gt0_frame_check/data_xored[48]} {gt0_frame_check/data_xored[49]} {gt0_frame_check/data_xored[50]} {gt0_frame_check/data_xored[51]} {gt0_frame_check/data_xored[52]} {gt0_frame_check/data_xored[53]} {gt0_frame_check/data_xored[54]} {gt0_frame_check/data_xored[55]} {gt0_frame_check/data_xored[56]} {gt0_frame_check/data_xored[57]} {gt0_frame_check/data_xored[58]} {gt0_frame_check/data_xored[59]} {gt0_frame_check/data_xored[60]} {gt0_frame_check/data_xored[61]} {gt0_frame_check/data_xored[62]} {gt0_frame_check/data_xored[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 64 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list {gt0_frame_check/bit_error[0]} {gt0_frame_check/bit_error[1]} {gt0_frame_check/bit_error[2]} {gt0_frame_check/bit_error[3]} {gt0_frame_check/bit_error[4]} {gt0_frame_check/bit_error[5]} {gt0_frame_check/bit_error[6]} {gt0_frame_check/bit_error[7]} {gt0_frame_check/bit_error[8]} {gt0_frame_check/bit_error[9]} {gt0_frame_check/bit_error[10]} {gt0_frame_check/bit_error[11]} {gt0_frame_check/bit_error[12]} {gt0_frame_check/bit_error[13]} {gt0_frame_check/bit_error[14]} {gt0_frame_check/bit_error[15]} {gt0_frame_check/bit_error[16]} {gt0_frame_check/bit_error[17]} {gt0_frame_check/bit_error[18]} {gt0_frame_check/bit_error[19]} {gt0_frame_check/bit_error[20]} {gt0_frame_check/bit_error[21]} {gt0_frame_check/bit_error[22]} {gt0_frame_check/bit_error[23]} {gt0_frame_check/bit_error[24]} {gt0_frame_check/bit_error[25]} {gt0_frame_check/bit_error[26]} {gt0_frame_check/bit_error[27]} {gt0_frame_check/bit_error[28]} {gt0_frame_check/bit_error[29]} {gt0_frame_check/bit_error[30]} {gt0_frame_check/bit_error[31]} {gt0_frame_check/bit_error[32]} {gt0_frame_check/bit_error[33]} {gt0_frame_check/bit_error[34]} {gt0_frame_check/bit_error[35]} {gt0_frame_check/bit_error[36]} {gt0_frame_check/bit_error[37]} {gt0_frame_check/bit_error[38]} {gt0_frame_check/bit_error[39]} {gt0_frame_check/bit_error[40]} {gt0_frame_check/bit_error[41]} {gt0_frame_check/bit_error[42]} {gt0_frame_check/bit_error[43]} {gt0_frame_check/bit_error[44]} {gt0_frame_check/bit_error[45]} {gt0_frame_check/bit_error[46]} {gt0_frame_check/bit_error[47]} {gt0_frame_check/bit_error[48]} {gt0_frame_check/bit_error[49]} {gt0_frame_check/bit_error[50]} {gt0_frame_check/bit_error[51]} {gt0_frame_check/bit_error[52]} {gt0_frame_check/bit_error[53]} {gt0_frame_check/bit_error[54]} {gt0_frame_check/bit_error[55]} {gt0_frame_check/bit_error[56]} {gt0_frame_check/bit_error[57]} {gt0_frame_check/bit_error[58]} {gt0_frame_check/bit_error[59]} {gt0_frame_check/bit_error[60]} {gt0_frame_check/bit_error[61]} {gt0_frame_check/bit_error[62]} {gt0_frame_check/bit_error[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe2]
set_property port_width 64 [get_debug_ports u_ila_0/probe2]
connect_debug_port u_ila_0/probe2 [get_nets [list {gt0_frame_check/error_count_r[0]} {gt0_frame_check/error_count_r[1]} {gt0_frame_check/error_count_r[2]} {gt0_frame_check/error_count_r[3]} {gt0_frame_check/error_count_r[4]} {gt0_frame_check/error_count_r[5]} {gt0_frame_check/error_count_r[6]} {gt0_frame_check/error_count_r[7]} {gt0_frame_check/error_count_r[8]} {gt0_frame_check/error_count_r[9]} {gt0_frame_check/error_count_r[10]} {gt0_frame_check/error_count_r[11]} {gt0_frame_check/error_count_r[12]} {gt0_frame_check/error_count_r[13]} {gt0_frame_check/error_count_r[14]} {gt0_frame_check/error_count_r[15]} {gt0_frame_check/error_count_r[16]} {gt0_frame_check/error_count_r[17]} {gt0_frame_check/error_count_r[18]} {gt0_frame_check/error_count_r[19]} {gt0_frame_check/error_count_r[20]} {gt0_frame_check/error_count_r[21]} {gt0_frame_check/error_count_r[22]} {gt0_frame_check/error_count_r[23]} {gt0_frame_check/error_count_r[24]} {gt0_frame_check/error_count_r[25]} {gt0_frame_check/error_count_r[26]} {gt0_frame_check/error_count_r[27]} {gt0_frame_check/error_count_r[28]} {gt0_frame_check/error_count_r[29]} {gt0_frame_check/error_count_r[30]} {gt0_frame_check/error_count_r[31]} {gt0_frame_check/error_count_r[32]} {gt0_frame_check/error_count_r[33]} {gt0_frame_check/error_count_r[34]} {gt0_frame_check/error_count_r[35]} {gt0_frame_check/error_count_r[36]} {gt0_frame_check/error_count_r[37]} {gt0_frame_check/error_count_r[38]} {gt0_frame_check/error_count_r[39]} {gt0_frame_check/error_count_r[40]} {gt0_frame_check/error_count_r[41]} {gt0_frame_check/error_count_r[42]} {gt0_frame_check/error_count_r[43]} {gt0_frame_check/error_count_r[44]} {gt0_frame_check/error_count_r[45]} {gt0_frame_check/error_count_r[46]} {gt0_frame_check/error_count_r[47]} {gt0_frame_check/error_count_r[48]} {gt0_frame_check/error_count_r[49]} {gt0_frame_check/error_count_r[50]} {gt0_frame_check/error_count_r[51]} {gt0_frame_check/error_count_r[52]} {gt0_frame_check/error_count_r[53]} {gt0_frame_check/error_count_r[54]} {gt0_frame_check/error_count_r[55]} {gt0_frame_check/error_count_r[56]} {gt0_frame_check/error_count_r[57]} {gt0_frame_check/error_count_r[58]} {gt0_frame_check/error_count_r[59]} {gt0_frame_check/error_count_r[60]} {gt0_frame_check/error_count_r[61]} {gt0_frame_check/error_count_r[62]} {gt0_frame_check/error_count_r[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe3]
set_property port_width 64 [get_debug_ports u_ila_0/probe3]
connect_debug_port u_ila_0/probe3 [get_nets [list {gt0_frame_check/expected_data_i[0]} {gt0_frame_check/expected_data_i[1]} {gt0_frame_check/expected_data_i[2]} {gt0_frame_check/expected_data_i[3]} {gt0_frame_check/expected_data_i[4]} {gt0_frame_check/expected_data_i[5]} {gt0_frame_check/expected_data_i[6]} {gt0_frame_check/expected_data_i[7]} {gt0_frame_check/expected_data_i[8]} {gt0_frame_check/expected_data_i[9]} {gt0_frame_check/expected_data_i[10]} {gt0_frame_check/expected_data_i[11]} {gt0_frame_check/expected_data_i[12]} {gt0_frame_check/expected_data_i[13]} {gt0_frame_check/expected_data_i[14]} {gt0_frame_check/expected_data_i[15]} {gt0_frame_check/expected_data_i[16]} {gt0_frame_check/expected_data_i[17]} {gt0_frame_check/expected_data_i[18]} {gt0_frame_check/expected_data_i[19]} {gt0_frame_check/expected_data_i[20]} {gt0_frame_check/expected_data_i[21]} {gt0_frame_check/expected_data_i[22]} {gt0_frame_check/expected_data_i[23]} {gt0_frame_check/expected_data_i[24]} {gt0_frame_check/expected_data_i[25]} {gt0_frame_check/expected_data_i[26]} {gt0_frame_check/expected_data_i[27]} {gt0_frame_check/expected_data_i[28]} {gt0_frame_check/expected_data_i[29]} {gt0_frame_check/expected_data_i[30]} {gt0_frame_check/expected_data_i[31]} {gt0_frame_check/expected_data_i[32]} {gt0_frame_check/expected_data_i[33]} {gt0_frame_check/expected_data_i[34]} {gt0_frame_check/expected_data_i[35]} {gt0_frame_check/expected_data_i[36]} {gt0_frame_check/expected_data_i[37]} {gt0_frame_check/expected_data_i[38]} {gt0_frame_check/expected_data_i[39]} {gt0_frame_check/expected_data_i[40]} {gt0_frame_check/expected_data_i[41]} {gt0_frame_check/expected_data_i[42]} {gt0_frame_check/expected_data_i[43]} {gt0_frame_check/expected_data_i[44]} {gt0_frame_check/expected_data_i[45]} {gt0_frame_check/expected_data_i[46]} {gt0_frame_check/expected_data_i[47]} {gt0_frame_check/expected_data_i[48]} {gt0_frame_check/expected_data_i[49]} {gt0_frame_check/expected_data_i[50]} {gt0_frame_check/expected_data_i[51]} {gt0_frame_check/expected_data_i[52]} {gt0_frame_check/expected_data_i[53]} {gt0_frame_check/expected_data_i[54]} {gt0_frame_check/expected_data_i[55]} {gt0_frame_check/expected_data_i[56]} {gt0_frame_check/expected_data_i[57]} {gt0_frame_check/expected_data_i[58]} {gt0_frame_check/expected_data_i[59]} {gt0_frame_check/expected_data_i[60]} {gt0_frame_check/expected_data_i[61]} {gt0_frame_check/expected_data_i[62]} {gt0_frame_check/expected_data_i[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe4]
set_property port_width 64 [get_debug_ports u_ila_0/probe4]
connect_debug_port u_ila_0/probe4 [get_nets [list {gt0_frame_check/rx_data_r_track[0]} {gt0_frame_check/rx_data_r_track[1]} {gt0_frame_check/rx_data_r_track[2]} {gt0_frame_check/rx_data_r_track[3]} {gt0_frame_check/rx_data_r_track[4]} {gt0_frame_check/rx_data_r_track[5]} {gt0_frame_check/rx_data_r_track[6]} {gt0_frame_check/rx_data_r_track[7]} {gt0_frame_check/rx_data_r_track[8]} {gt0_frame_check/rx_data_r_track[9]} {gt0_frame_check/rx_data_r_track[10]} {gt0_frame_check/rx_data_r_track[11]} {gt0_frame_check/rx_data_r_track[12]} {gt0_frame_check/rx_data_r_track[13]} {gt0_frame_check/rx_data_r_track[14]} {gt0_frame_check/rx_data_r_track[15]} {gt0_frame_check/rx_data_r_track[16]} {gt0_frame_check/rx_data_r_track[17]} {gt0_frame_check/rx_data_r_track[18]} {gt0_frame_check/rx_data_r_track[19]} {gt0_frame_check/rx_data_r_track[20]} {gt0_frame_check/rx_data_r_track[21]} {gt0_frame_check/rx_data_r_track[22]} {gt0_frame_check/rx_data_r_track[23]} {gt0_frame_check/rx_data_r_track[24]} {gt0_frame_check/rx_data_r_track[25]} {gt0_frame_check/rx_data_r_track[26]} {gt0_frame_check/rx_data_r_track[27]} {gt0_frame_check/rx_data_r_track[28]} {gt0_frame_check/rx_data_r_track[29]} {gt0_frame_check/rx_data_r_track[30]} {gt0_frame_check/rx_data_r_track[31]} {gt0_frame_check/rx_data_r_track[32]} {gt0_frame_check/rx_data_r_track[33]} {gt0_frame_check/rx_data_r_track[34]} {gt0_frame_check/rx_data_r_track[35]} {gt0_frame_check/rx_data_r_track[36]} {gt0_frame_check/rx_data_r_track[37]} {gt0_frame_check/rx_data_r_track[38]} {gt0_frame_check/rx_data_r_track[39]} {gt0_frame_check/rx_data_r_track[40]} {gt0_frame_check/rx_data_r_track[41]} {gt0_frame_check/rx_data_r_track[42]} {gt0_frame_check/rx_data_r_track[43]} {gt0_frame_check/rx_data_r_track[44]} {gt0_frame_check/rx_data_r_track[45]} {gt0_frame_check/rx_data_r_track[46]} {gt0_frame_check/rx_data_r_track[47]} {gt0_frame_check/rx_data_r_track[48]} {gt0_frame_check/rx_data_r_track[49]} {gt0_frame_check/rx_data_r_track[50]} {gt0_frame_check/rx_data_r_track[51]} {gt0_frame_check/rx_data_r_track[52]} {gt0_frame_check/rx_data_r_track[53]} {gt0_frame_check/rx_data_r_track[54]} {gt0_frame_check/rx_data_r_track[55]} {gt0_frame_check/rx_data_r_track[56]} {gt0_frame_check/rx_data_r_track[57]} {gt0_frame_check/rx_data_r_track[58]} {gt0_frame_check/rx_data_r_track[59]} {gt0_frame_check/rx_data_r_track[60]} {gt0_frame_check/rx_data_r_track[61]} {gt0_frame_check/rx_data_r_track[62]} {gt0_frame_check/rx_data_r_track[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe5]
set_property port_width 1 [get_debug_ports u_ila_0/probe5]
connect_debug_port u_ila_0/probe5 [get_nets [list gt0_frame_check/complete]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe6]
set_property port_width 1 [get_debug_ports u_ila_0/probe6]
connect_debug_port u_ila_0/probe6 [get_nets [list gt0_frame_check/error_detected_c]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe7]
set_property port_width 1 [get_debug_ports u_ila_0/probe7]
connect_debug_port u_ila_0/probe7 [get_nets [list gt0_frame_check/next_begin_c]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe8]
set_property port_width 1 [get_debug_ports u_ila_0/probe8]
connect_debug_port u_ila_0/probe8 [get_nets [list gt0_frame_check/next_data_error_detected_c]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe9]
set_property port_width 1 [get_debug_ports u_ila_0/probe9]
connect_debug_port u_ila_0/probe9 [get_nets [list gt0_frame_check/next_track_data_c]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe10]
set_property port_width 1 [get_debug_ports u_ila_0/probe10]
connect_debug_port u_ila_0/probe10 [get_nets [list gt0_frame_check/track_data_r3]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets gt0_rxusrclk2_i]
