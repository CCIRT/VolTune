# host/evm

This directory contains sample programs to manipulate [experimental designs](../../device/vivado/evm) for [UCD9224EVM evaluation board](https://www.ti.com/tool/ja-jp/UCD9224EVM-464) from the host.

## Design

- [experimental designs](../../device/vivado/evm)

## targets

- evm_voltage
- evm_power

## binary

- `build/bin/evm_voltage`
- `build/bin/evm_power`

## Specifying a Bitstream

There is no mechanism implemented to give the path of the bitstream from the command line.

You must edit the C++ file directly to change the bitstream.
