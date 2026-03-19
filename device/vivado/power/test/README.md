# Power Test Scripts

This directory contains Tcl-based test scripts for the power-oriented Vivado designs.

These tests set the voltage of `MGTAVCC`, start transmitting the specified data pattern, and collect results including error-bit count, latency, and power-related values.

## Test scripts

- [power_test_loopback.tcl](./power_test_loopback.tcl), test script for a single KC705
- [power_test_dual.tcl](./power_test_dual.tcl), test script for dual KC705 boards

## Test parameters

- `host`
  - Set `hw_server` host.
  - Vivado default is `localhost`.
- `port`
  - Set `hw_server` port.
  - Vivado default is `3121`.
- `target`, `target_rx`, `target_tx`
  - Set test target FPGA number.
- `test_pattern`
  - Set test pattern from the list below.
    - `0`: All Zero
    - `1`: All Hi
    - `2`: Count Up
- `test_size`
  - Set test size.
  - This value is aligned to 8 Byte.
- `test_voltage`
  - Set test voltage of `MGTAVCC`.
  - This value is fixed-point 16 bit.
    - Integer part: 4 bit
    - Fractional part: 12 bit
    - e.g. `1.000 V -> 0x1000`
    - e.g. `0.900 V -> 0x0E66`
- `i2cClockSelect`
  - Set PMBus speed from the list below.
    - `0`: 100 kHz
    - `1`: 400 kHz
    - `2`: 1 MHz

## Test results

- `Recv data size [Byte]`
  - Shows received data size.
  - This value should be the same as `test_size`, aligned to 8 Byte.
- `Error bit count`
  - Shows error bit count.
  - This value is usually zero.
- `Latency`
  - Shows cycle count between sending the first data and receiving the first data.
- `Result`
  - Shows the result value of `test_application`.
  - This value should be zero. If not zero, please refer to the error code in [test_app_base.cpp](../../../hls/test_app_base/src/test_app_base.cpp).
- `Min Power`
  - Shows minimum power value while testing.
- `Max Power`
  - Shows maximum power value while testing.
- `Sum Power`
  - Shows accumulated power value while testing.
- `Sum Power 2`
  - Shows accumulated squared power value while testing.
- `Sum Count`
  - Shows cycle count value while testing.

## Notes

- These scripts are used by the power-oriented test flow under `device/vivado/power/`.
- The reported values correspond to the test application and measurement registers exposed by the FPGA-side design.
