# Pin settings
set_property PACKAGE_PIN AD12 [get_ports sysclk_p]
set_property PACKAGE_PIN AD11 [get_ports sysclk_n]
set_property PACKAGE_PIN J8   [get_ports gt_refclk_p]
set_property PACKAGE_PIN J7   [get_ports gt_refclk_n]
set_property IOSTANDARD LVDS  [get_ports sysclk_p]

# Clock constraints
create_clock -period 5.000 [get_ports sysclk_p]
create_clock -period 8.5333 [get_ports gt_refclk_p]
set_clock_groups -asynchronous \
  -group [get_clocks -include_generated_clocks sysclk_p] \
  -group [get_clocks -include_generated_clocks gt_refclk_p] \
  -group [get_clocks -include_generated_clocks loopback_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i/TXOUTCLK] \
  -group [get_clocks -include_generated_clocks loopback_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i/RXOUTCLK]

# Place constraints
set_property LOC GTXE2_CHANNEL_X0Y8 [get_cells loopback_gt_wrapper_i/gt_i/inst/gt_init_i/gt_i/gt0_gt_i/gtxe2_i]

# False path
set_false_path -from [get_clocks -of_objects [get_pins loopback_gt_wrapper_i/gt_i/inst/gt_usrclk_source/txoutclk_mmcm0_i/mmcm_adv_inst/CLKOUT0]] \
                 -to [get_clocks -of_objects [get_pins loopback_gt_wrapper_i/gt_i/inst/gt_usrclk_source/rxoutclk_mmcm1_i/mmcm_adv_inst/CLKOUT0]]
