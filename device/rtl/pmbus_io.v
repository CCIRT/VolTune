// Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
// All rights reserved.
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php

module pmbus_io (
    (* X_INTERFACE_INFO = "xilinx.com:interface:gpio_rtl:1.0 GPIO TRI_I" *)
    (* X_INTERFACE_MODE = "slave GPIO" *)
    output  [3:0] gpio_io_i,
    (* X_INTERFACE_INFO = "xilinx.com:interface:gpio_rtl:1.0 GPIO TRI_O" *)
    input [3:0] gpio_io_o,
    (* X_INTERFACE_INFO = "xilinx.com:interface:gpio_rtl:1.0 GPIO TRI_T" *)
    input [3:0] gpio_io_t,

    inout wire SCL,
    inout wire SDA
);

  IOBUF u_sclk (
    .IO(SCL),
    .O(gpio_io_i[0]),
    .I(gpio_io_o[0]),
    .T(gpio_io_t[0])
  );
  assign gpio_io_i[2] = gpio_io_i[0];

  IOBUF u_sda (
    .IO(SDA),
    .O(gpio_io_i[1]),
    .I(gpio_io_o[1]),
    .T(gpio_io_t[1])
  );
  assign gpio_io_i[3] = gpio_io_i[1];

endmodule