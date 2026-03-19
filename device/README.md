# device

This direcotry contains RTL, HLS, and vivado design projects.

## Environment

- OS: Ubuntu 20.04
- Tools:
    - Xilinx Vivado/Vitis 2022.1
    - CMake 3.XX.X
    - GNU Make 4.2.1
- Board: [Xilinx Kintex-7 FPGA KC705 Evaluation Kit](https://www.xilinx.com/products/boards-and-kits/ek-k7-kc705-g.html)

## How to build

Use [`../build_device.sh`](../build_device.sh) script.

```sh
cd <Repository top>
./build_device.sh all
```

### Build vivado/design-example

[`vivado/design-example`](./vivado/design-example) directory contains design example which received from AIST and modified design.

Please refer to [`vivado/design-example/README.md`](./vivado/design-example/README.md) to build this.

## Directories

```
├── constraint : Constraint files
├── hls        : Vitis HLS modules
├── ip         : IPs
├── rtl        : RTL files
├── vitis_src  : Source files for Microblaze
└── vivado     : FPGA designs
```

## Files

- [CMakeLists.txt](./CMakeLists.txt): CMake root file for building FPGA design
- [README.md](./README.md): This file
