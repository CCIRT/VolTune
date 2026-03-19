# Btn 0
# set_property -dict {PACKAGE_PIN D20 IOSTANDARD LVCMOS33} [get_ports btn]

# Btn 1
set_property -dict {PACKAGE_PIN D19 IOSTANDARD LVCMOS33} [get_ports reset]

# PMBus CLK = SCL, DATA = SDA
set_property -dict {PACKAGE_PIN P16 IOSTANDARD LVCMOS33} [get_ports SCL]
set_property PULLUP true [get_ports SCL]
set_property -dict {PACKAGE_PIN P15 IOSTANDARD LVCMOS33} [get_ports SDA]
set_property PULLUP true [get_ports SDA]

## RGB LEDs
set_property -dict { PACKAGE_PIN L15   IOSTANDARD LVCMOS33 } [get_ports { led0_b }];
set_property -dict { PACKAGE_PIN G17   IOSTANDARD LVCMOS33 } [get_ports { led0_g }];
set_property -dict { PACKAGE_PIN N15   IOSTANDARD LVCMOS33 } [get_ports { led0_r }];

set_property -dict { PACKAGE_PIN G14   IOSTANDARD LVCMOS33 } [get_ports { led1_b }];
set_property -dict { PACKAGE_PIN L14   IOSTANDARD LVCMOS33 } [get_ports { led1_g }];
set_property -dict { PACKAGE_PIN M15   IOSTANDARD LVCMOS33 } [get_ports { led1_r }];
