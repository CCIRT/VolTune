# XSDB Wrapper Library

This directory provides a C++ wrapper library for XSDB, Xilinx System Debugger, used by the host-side tools in the VolTune repository.

## Reference

- [XSDB (Xilinx® System Debugger)](https://docs.xilinx.com/v/u/en-US/ug1208-xsct-reference-guide)

## Language

- C++17

## Environment

- OS: Windows11
- OS: Ubuntu 20.04
- CMake 3.0.0

## How to build

```sh
cd <Repository Top Directory>
mkdir build
cd build
cmake ..
cmake --build .
```

## CMake targets

- `xsdb_lib`: library target
- `sample_xsdb_program` : build [src/sample.cpp](src/sample.cpp).

## Related files

- [`../README.md`](../README.md), parent host-side overview

## Notes

- This library is used as a support layer for host-side interaction with FPGA targets through XSDB.
- Detailed measurement flows are documented in the sibling READMEs under `../power/` and `../voltage/`.
