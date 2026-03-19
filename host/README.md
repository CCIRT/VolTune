# host

This directory contains host test programs.

## Environment

- OS: Windows10
- MSYS2
- Skyworks ClockBuilder Pro Version 4.7

### Hardware

- 2x [Xilinx Kintex-7 FPGA KC705 Evaluation Kit](https://www.xilinx.com/products/boards-and-kits/ek-k7-kc705-g.html)
- 1x [Skyworks Si5391 clock generators](https://www.skyworksinc.com/-/media/Skyworks/SL/documents/public/data-sheets/si5391-datasheet.pdf)
- 7x SMC to SMC cables
- Some USB and power cables for each board
- Host PC which is installed Windows 10/11

## Folder

- evm : It contains a simple program to test whether PowerManager works with CoraZ7 and evaluation power supply boards.
- power: It contains the program for measuring BER and power.
- util: Utility
- voltage: It contains the program for measuring voltage.
- xsdb_wrapper: Wrapper library for manipulating XSDB from C++
