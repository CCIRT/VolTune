# test

## What is this directory?

This directory contains tests for Vivado designes.

## About tests

These tests set voltage of MGTAVCC, start transmitting specified data and get result include Error bit count, Latency and Power value.

- [power_test_loopback.tcl](./power_test_loopback.tcl) is for single KC705.
- [power_test_dual.tcl](./power_test_dual.tcl) is for dual KC705s.

### Test parameter

- host
  - Set hw_server host.
  - Vivado default is localhost.
- port
  - Set hw_server port.
  - Vivado default is 3121.
- target, target_rx, target_tx
  - Set test target FPGA number.
- test_pattern
  - Set test pattern from the list below.
    - 0: All Zero
    - 1: All Hi
    - 2: Count Up
- test_size
  - Set test size.
  - This value is aligned to 8 Byte.
- test_voltage
  - Set test voltage of MGTAVCC.
  - This value is fixed-point 16bit.
    - Integer part: 4 bit
    - Fractional part: 12 bit
    - e.g. 1.000 V -> 0x1000
    - e.g. 0.900 V -> 0x0E66
- i2cClockSelect
  - Set PMBus speed from the list below.
    - 0: 100kHz
    - 1: 400kHz
    - 2: 1MHz

### Test result

- Recv data size [Byte]
  - Show received data size.
  - This value should be as same as test size which is aligned to 8 Byte.
- Error bit count
  - Show error bit count.
  - This value is usually zero.
- Latency
  - Show cycle count between send first data to receive first data.
- Result
  - Show result value of test_application.
  - This value should be zero. If not zero, please refer to error code in [test_app_base.cpp](../../../hls/test_app_base/src/test_app_base.cpp)
- Min Power
  - Show minimum power value while testing.
- Max Power
  - Show maximum power value while testing.
- Sum Power
  - Show accumulated power value while testing.
- Sum Power 2
  - Show accumulated squared power value while testing.
- Sum Count"
  - Show cycle count value while testing.
