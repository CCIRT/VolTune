create_clock -period 8.000 [get_ports Q2_CLK0_GTREFCLK_PAD_P_IN]
create_clock -period 10.000 -name drpclk_in_i [get_ports DRP_CLK_IN_P]

set_false_path -to [get_pins -filter {REF_PIN_NAME=~*CLR} -of_objects [get_cells -hierarchical -filter {NAME =~ *_txfsmresetdone_r*}]]
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*D}   -of_objects [get_cells -hierarchical -filter {NAME =~ *_txfsmresetdone_r*}]]
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*D}   -of_objects [get_cells -hierarchical -filter {NAME =~ *reset_on_error_in_r*}]]
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
set_property C_INPUT_PIPE_STAGES 2 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets gt0_rxusrclk2_i]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 64 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {gt0_frame_check/error_count_r[0]} {gt0_frame_check/error_count_r[1]} {gt0_frame_check/error_count_r[2]} {gt0_frame_check/error_count_r[3]} {gt0_frame_check/error_count_r[4]} {gt0_frame_check/error_count_r[5]} {gt0_frame_check/error_count_r[6]} {gt0_frame_check/error_count_r[7]} {gt0_frame_check/error_count_r[8]} {gt0_frame_check/error_count_r[9]} {gt0_frame_check/error_count_r[10]} {gt0_frame_check/error_count_r[11]} {gt0_frame_check/error_count_r[12]} {gt0_frame_check/error_count_r[13]} {gt0_frame_check/error_count_r[14]} {gt0_frame_check/error_count_r[15]} {gt0_frame_check/error_count_r[16]} {gt0_frame_check/error_count_r[17]} {gt0_frame_check/error_count_r[18]} {gt0_frame_check/error_count_r[19]} {gt0_frame_check/error_count_r[20]} {gt0_frame_check/error_count_r[21]} {gt0_frame_check/error_count_r[22]} {gt0_frame_check/error_count_r[23]} {gt0_frame_check/error_count_r[24]} {gt0_frame_check/error_count_r[25]} {gt0_frame_check/error_count_r[26]} {gt0_frame_check/error_count_r[27]} {gt0_frame_check/error_count_r[28]} {gt0_frame_check/error_count_r[29]} {gt0_frame_check/error_count_r[30]} {gt0_frame_check/error_count_r[31]} {gt0_frame_check/error_count_r[32]} {gt0_frame_check/error_count_r[33]} {gt0_frame_check/error_count_r[34]} {gt0_frame_check/error_count_r[35]} {gt0_frame_check/error_count_r[36]} {gt0_frame_check/error_count_r[37]} {gt0_frame_check/error_count_r[38]} {gt0_frame_check/error_count_r[39]} {gt0_frame_check/error_count_r[40]} {gt0_frame_check/error_count_r[41]} {gt0_frame_check/error_count_r[42]} {gt0_frame_check/error_count_r[43]} {gt0_frame_check/error_count_r[44]} {gt0_frame_check/error_count_r[45]} {gt0_frame_check/error_count_r[46]} {gt0_frame_check/error_count_r[47]} {gt0_frame_check/error_count_r[48]} {gt0_frame_check/error_count_r[49]} {gt0_frame_check/error_count_r[50]} {gt0_frame_check/error_count_r[51]} {gt0_frame_check/error_count_r[52]} {gt0_frame_check/error_count_r[53]} {gt0_frame_check/error_count_r[54]} {gt0_frame_check/error_count_r[55]} {gt0_frame_check/error_count_r[56]} {gt0_frame_check/error_count_r[57]} {gt0_frame_check/error_count_r[58]} {gt0_frame_check/error_count_r[59]} {gt0_frame_check/error_count_r[60]} {gt0_frame_check/error_count_r[61]} {gt0_frame_check/error_count_r[62]} {gt0_frame_check/error_count_r[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 1 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list gt0_frame_check/complete]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe2]
set_property port_width 1 [get_debug_ports u_ila_0/probe2]
connect_debug_port u_ila_0/probe2 [get_nets [list gt0_frame_check/error_detected_c]]
set_property C_CLK_INPUT_FREQ_HZ 250000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets gt0_rxusrclk2_i]
