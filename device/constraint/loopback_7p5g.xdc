# Pin settings
set_property PACKAGE_PIN AD12 [get_ports sysclk_p]
set_property PACKAGE_PIN AD11 [get_ports sysclk_n]
set_property PACKAGE_PIN J8   [get_ports gt_refclk_p]
set_property PACKAGE_PIN J7   [get_ports gt_refclk_n]
set_property IOSTANDARD LVDS  [get_ports sysclk_p]

# Clock constraints
create_clock -period 5.000  [get_ports sysclk_p]
create_clock -period 8.5333 [get_ports gt_refclk_p]
set_clock_groups -asynchronous \
  -group [get_clocks -include_generated_clocks sysclk_p] \
  -group [get_clocks -include_generated_clocks gt_refclk_p] \
  -group [get_clocks -include_generated_clocks loopback_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i/TXOUTCLK] \
  -group [get_clocks -include_generated_clocks loopback_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i/RXOUTCLK]

# Place constraints
set_property LOC GTXE2_CHANNEL_X0Y8 [get_cells loopback_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i]

# False path
set_false_path -to [get_cells "*/sync_reg_tx_fsm_reset_done/reg_1_reg[0]"]
set_false_path -to [get_cells "design_1_i/*/sync_reg_0/inst/reg_1_reg*"]
set_false_path -from [get_clocks -of_objects [get_pins loopback_gt_wrapper_i/gt_i/inst/gt_usrclk_source/txoutclk_mmcm0_i/mmcm_adv_inst/CLKOUT0]] \
                 -to [get_clocks -of_objects [get_pins loopback_gt_wrapper_i/gt_i/inst/gt_usrclk_source/rxoutclk_mmcm1_i/mmcm_adv_inst/CLKOUT0]]

# Max Delay
set_max_delay 10.0 -from [get_pins "design_1_i/test_manager/clk_counter_0/inst/cnt2_reg*/C"] \
                     -to [get_pins "design_1_i/test_manager/clk_counter_0/inst/clk1_cnt2_reg*/D"]

# Connect internal clk to dbg_hub
connect_debug_port dbg_hub/clk [get_nets design_1_i/clk_wiz_0/inst/clk_out1]
