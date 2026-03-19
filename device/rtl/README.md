# RTL Modules

This directory contains the handwritten Verilog RTL modules used in the VolTune repository.

These modules include common utility blocks, PMBus-related logic, measurement/test-support modules, and top-level RTL wrappers used by the FPGA designs. 

For PMBus concepts, data formats, command usage, and the subset of PMBus behavior relevant to this repository, see [`../../docs/PMBUS.md`](../../docs/PMBUS.md).

## RTL files

### Utility modules

- `ap_start_only_once.v`
- `axis_empty.v`: An AXI Stream Master that does not produce any output.
- `axis_led.v`: A module that converts a simple signal to output to the LED by receiving the test state from AXI Stream.
- `axis_sink.v`: A module that continuously reads AXI Stream. The data read is discarded.
- `coraz707s_led.v`: LED for Cora Z7-07s
- `kc705_led.v`
- `mmcm_wrapper.v`
- `sync_reg.v`: A module that implements a two-stage FF.

### PMBus-related modules

- `axis_pmbus.v`: A module that receives commands and data from AXI Stream to generate PMBus signals. PEC is not implemented.
- `axis_pmbus_with_pec.v`: A module that receives commands and data from AXI Stream to generate PMBus signals. PEC checking is always issued.
- `axis_pmbus_wrapper.v`: `axis_pmbus_with_pec.v` wrapper. This wrapper module outputs signals to the terminals from IOBUF. Connect it directly to the device's terminals.
- `pmbus_io.v`: PMBus GPIO interface module

### Measurement and test-support modules

- `axis_counter.v`: A counter module that outputs the counter value via AXI Stream ignoring TREADY for use in HLS. It also receives a clear signal from AXI Stream.
- `clk_counter.v`: A module that uses the system clock to measure the frequency of an external clock. It cannot measure if the external clock is not operating.
- `latency_checker.v`: A module for measuring delay. The measurement accuracy depends on the input clock.

### Top-level design wrappers

#### Power-oriented designs

- `loopback.v`
- `loopback_swpmbus.v`
- `rx.v`
- `rx_swpmbus.v`
- `tx.v`
- `tx_swpmbus.v`

#### Voltage-control designs

- `voltage_loopback.v`
- `voltage_loopback_swpmbus.v`
- `voltage_n000_swpmbus.v`
- `voltage_rx.v`
- `voltage_rx_swpmbus.v`
- `voltage_tx.v`
- `voltage_tx_swpmbus.v`

## Related files

- [`../README.md`](../README.md), parent device-side overview
- [`../vivado/README.md`](../vivado/README.md), Vivado design hierarchy
- [`../vivado/power/README.md`](../vivado/power/README.md), power-oriented designs
- [`../vivado/voltage/README.md`](../vivado/voltage/README.md), voltage-control designs

## Notes

- This directory contains build-critical RTL sources used by the Vivado design hierarchies.
- Several top-level wrappers exist in both hardware-control and software-control variants.
- Do not rename these modules unless the dependent Vivado, Vitis, and HLS integration flow has been revalidated.
