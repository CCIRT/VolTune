# fpga-power-control

## What is this repository?

This repository is created for "FPGA Fine-grained On-chip Power Control" project with AIST.

## Build Device

⚠️ A licence is required to synthesise Vivado project for KC705.

Please set `XILINXD_LICENSE_FILE` environment variables.

Run [`build_device.sh`](./build_device.sh) script on Ubuntu 20.04.

```sh
cd <Repository top>
./build_device.sh all
```


## Build Host

[MSYS2](https://www.msys2.org/) is required to build the program. URL: https://www.msys2.org/

After installing MSYS2, launch `C:\msys64\mingw64.exe`, and goto this repository directory, and run [`./msys_install.sh`](./msys_install.sh) to install the necessary tools for building, such as GCC and CMake.

To build the program, run [`msys_build.sh`](./msys_build.sh) script on the MSYS2(`C:\msys64\mingw64.exe`).

```bat
cd <Repository top>
.\msys_build.sh
```

An executable file will be generated in the `build/bin` directory.


## Directories

```
├── cmake      : Common CMake files
├── device     : Source code for device including FPGA
├── docs       : Documentation
├── experiments: Files for experiments
└── host       : Source code for host application
```

## Files

- [.editorconfig](./.editorconfig): Configuration file for editors
- [.gitignore](./.gitignore): Git ignore
- [build_device.sh](./build_device.sh): Build device script
- [build_host.bat](./build_host.bat): Build host program for Windows
- [msys_build.sh](./msys_build.sh): Install script for MSYS
- [msys_install.sh](./msys_build.sh): Install script for MSYS
- [CMakeLists.txt](./CMakeLists.txt): A dummy CMake root file for use in editors such as VSCode. This is not used for build.
- [README.md](./README.md): This file

