
# EVM Experimental Designs

This directory contains Vivado designs for experimental use with the [UCD9224EVM evaluation board](https://www.ti.com/tool/ja-jp/UCD9224EVM-464) from [Cora-Z7](https://digilent.com/reference/programmable-logic/cora-z7/start).

⚠️ This design can **NOT** be written to KC705.

## Pin assign

- PMBus CLK: SCL
- PMBus DATA: SDA

## How to build

```sh
cd <repository top>
./build_device -e -b build-evm
cd build-evm
make impl_evm_voltage_test impl_evm_power_test
```

## Bitstreams

- `<build-directory>/bitstream/evm_voltage_test/top.bit`
- `<build-directory>/bitstream/evm_power_test/top.bit`

## Open Vivado GUI

```sh
make open_evm_voltage_test
make open_evm_power_test
```

## Host sample program

The [`host/evm`](../../../host/evm) directory contains host sample programs to operate this design.

## Sample Software PMBus

### Build

```sh
make clear_evm_vitis evm_vitis
```

The cache will get in the way and prevent you from building, for example if you have played with the source code or CMake.

Run `clear_evm_vitis` first before building to delete a Vitis project.

### Open Vivado

```sh
make open_evm_vitis_design
```

### Open Vitis

```sh
make open_evm_vitis
```

## File

- [design_1.tcl](./design_1.tcl): evm_voltage_test design
- [design_2.tcl](./design_2.tcl): evm_power_test design
- [CMakeLists.txt](./CMakeLists.txt): CMake file
- [vitis_src/main.cpp](./vitis_src/main.cpp): main function of Software PMBus
- [vitis_design.tcl](./vitis_design.tcl): Software PMBus sample design
- [vitis_design.xdc](vitis_design.xdc): XDC for vitis_design.tcl
- [evm_coraz707s.xdc](./evm_coraz707s.xdc): XDC for design_1/2.tcl
- [set_board_path.config.tcl](./set_board_path.config.tcl): Tcl script to set board part.

## Related files

- [`../README.md`](../README.md), parent Vivado hierarchy
- [`../../../host/evm/README.md`](../../../host/evm/README.md), host-side EVM utilities

## Notes

- These designs are experimental and separate from the main KC705-based VolTune evaluation flow.
- The Software PMBus sample flow depends on Vitis project generation and may require clearing cached build state before rebuilding.
