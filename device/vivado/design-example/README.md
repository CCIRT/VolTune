# Design Examples

This directory contains reference transceiver design examples and modified designs used in the VolTune repository.

In `loopback-<speed>` directories, loopback test designs are contained. In `dual-board-<speed>` directories, dual-board test designs are contained. The `<speed>` value represents the speed of the Gigabit Transceiver (GT).

Examples:

- `5g`: 5 GT/s
- `10g`: 10 GT/s

## Scope

These designs are useful as reference material for understanding the baseline transceiver setup and validation flow used in this repository.

The currently provided examples are:

- loopback test designs
- dual-board test designs

## How to test

### Hardware connection

#### Loopback test

![](./img/loopback-tests-connection.drawio.svg)

#### Dual-board test

![](./img/dual-board-tests-connection.drawio.svg)

### Clock board setup (Dual-board tests only)

Please refer to [`../power/clock-board-quick-setup.md`](../power/clock-board-quick-setup.md).

### Write bitstream and set up ILA

For writing bitstream, please refer to [Programming the Device](https://docs.xilinx.com/r/2022.1-English/ug908-vivado-programming-debugging/Programming-the-Device) or [Embedded System Tools Reference Manual P.58](https://docs.xilinx.com/v/u/2015.2-English/ug1043-embedded-system-tools#page=58).

For setting up ILA, please refer to [Setting Up the ILA Core to Take a Measurement](https://docs.xilinx.com/r/2022.1-English/ug908-vivado-programming-debugging/Setting-Up-the-ILA-Core-to-Take-a-Measurement).

For dual-board tests, set up the ILA on the receive-side board, `KC705-1`.

### Start tests

Watch the ILA, press the `SW5` button, and continue watching the ILA.

For dual-board tests, press the `SW5` button on the send-side board, `KC705-0`.

A few minutes later, the `complete` signal will be asserted.

You can observe BER using the `error_count_r` value.

![](./img/ila.png)


## Public release note

Some support HDL files associated with the transceiver design examples were removed from the public repository because they appear to be generated or provided as part of the AMD/Xilinx wizard example flow and may be subject to separate vendor licensing terms.

The surrounding example structure and documentation are preserved, but some generated support files are not included in the public release.

## Directories

```text
design-example/
├── dual-board-5g   # Dual-board test at 5 GT/s
├── dual-board-10g  # Dual-board test at 10 GT/s
└── loopback-5g     # Loopback test at 5 GT/s
```

## Related files

- [`../README.md`](../README.md), parent Vivado hierarchy
- [`../power/README.md`](../power/README.md), power-oriented designs
