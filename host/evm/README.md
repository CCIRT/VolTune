# EVM Host Utilities

This directory contains sample host-side programs used to manipulate the [experimental designs](../../device/vivado/evm) for the [UCD9224EVM evaluation board](https://www.ti.com/tool/ja-jp/UCD9224EVM-464).

## Design

- [experimental designs](../../device/vivado/evm)

## Targets

- `evm_voltage`
- `evm_power`

## Generated binaries

- `build/bin/evm_voltage`
- `build/bin/evm_power`

## Specifying a bitstream

There is no mechanism implemented to specify the bitstream path from the command line.

To change the bitstream, you must edit the corresponding C++ source file directly.

## Related files

- [`../README.md`](../README.md), parent host-side overview
- [`../../device/vivado/evm/README.md`](../../device/vivado/evm/README.md), corresponding FPGA-side EVM designs
