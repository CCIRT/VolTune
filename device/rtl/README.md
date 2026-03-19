# rtl


This directory contains Verilog RTL.

## RTL Files

- [ap_start_only_once.v](ap_start_only_once.v)
- [axis_counter.v](axis_counter.v): A counter module that outputs the counter value via AXI Stream ignoring TREADY for use in HLS. It also receives a clear signal from AXI Stream.
- [axis_empty.v](axis_empty.v): An AXI Stream Master that does not produce any output.
- [axis_led.v](axis_led.v): A module that converts a simple signal to output to the LED by receiving the test state from AXI Stream.
- [axis_pmbus.v](axis_pmbus.v): A module that receives commands and data from AXI Stream to generate PMBus signals. PEC is not implemented.
- [axis_pmbus_with_pec.v](axis_pmbus_with_pec.v): A module that receives commands and data from AXI Stream to generate PMBus signals. PEC checking is always issued.
- [axis_pmbus_wrapper.v](axis_pmbus_wrapper.v): `axis_pmbus_with_pec.v` wrapper. This wrapper module outputs signals to the terminals from IOBUF. Connect it directly to the device's terminals.
- [axis_sink.v](axis_sink.v): A module that continuously reads AXI Stream. The data read is discarded.
- [clk_counter.v](clk_counter.v): A module that uses the system clock to measure the frequency of an external clock. It cannot measure if the external clock is not operating.
- coraz707s_led.v: LED for Cora Z7-07s
- kc705_led.v:
- [latency_checker.v](latency_checker.v): A module for measuring delay. The measurement accuracy depends on the input clock.
- [loopback_swpmbus.v](loopback_swpmbus.v):
- [loopback.v](loopback.v):
- [mmcm_wrapper.v](mmcm_wrapper.v):
- [pmbus_io.v](pmbus_io.v): PMBus GPIO interface module
- [rx_swpmbus.v](rx_swpmbus.v):
- [rx.v](rx.v):
- [sync_reg.v](sync_reg.v):A module that implements a two-stage FF.
- [tx_swpmbus.v](tx_swpmbus.v):
- [tx.v](tx.v):
- [voltage_loopback_swpmbus.v](voltage_loopback_swpmbus.v):
- [voltage_loopback.v](voltage_loopback.v):
- voltage_n000_swpmbus.v:
- [voltage_rx_swpmbus.v](voltage_rx_swpmbus.v):
- [voltage_rx.v](voltage_rx.v):
- [voltage_tx_swpmbus.v](voltage_tx_swpmbus.v):
- [voltage_tx.v](voltage_tx.v):

