# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

# PMBus CLK = SCL, DATA = SDA
set_property -dict {PACKAGE_PIN P16 IOSTANDARD LVCMOS33} [get_ports SCL]
set_property PULLUP true [get_ports SCL]
set_property -dict {PACKAGE_PIN P15 IOSTANDARD LVCMOS33} [get_ports SDA]
set_property PULLUP true [get_ports SDA]

