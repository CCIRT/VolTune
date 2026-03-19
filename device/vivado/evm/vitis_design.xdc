# PMBus CLK = SCL, DATA = SDA
set_property -dict {PACKAGE_PIN P16 IOSTANDARD LVCMOS33} [get_ports SCL]
set_property PULLUP true [get_ports SCL]
set_property -dict {PACKAGE_PIN P15 IOSTANDARD LVCMOS33} [get_ports SDA]
set_property PULLUP true [get_ports SDA]

