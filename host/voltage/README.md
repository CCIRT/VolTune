# Volatage

This directory provides a test program that change the voltage and measures the time it takes for the voltage to stabilize.

## How to build

Execute the following command on MSYS2 (`C:\msys64\mingw64.exe`).

```
cd <Reposigory top>
./msys_build.sh
```

If you want to build them individually, execute the command as follows:

```
cd <Reposigory top>
mkdir build
cd build
cmake ../host
make -j voltage-measure
```

## Input File Format

The input file is a configuration file in a CSV-like format. Lines beginning with # and empty lines are ignored.

Each line should contain the following six pieces of information.

- Lane: Test voltage lane name.
    - VCCINT
    - VCCAUX
    - VCC3V3
    - VADF
    - VCC2V5
    - VCC1V5
    - MGTAVCC
    - MGTAVTT
    - ACCAUX_IO
    - VCCBRAM
    - MGTVCCAUX
- Speed: Transceiver speed
    - none: No Transceiver
    - 2.5g: 2.5Gbps Transceiver
    - 5g: 5Gbps Transceiver
    - 7.5g: 7.5Gbps Transceiver
    - 10g: 10Gbps Transceiver
- Device 0 Initial voltage
- Device 0 Target voltage
- Device 1 Initial voltage
- Device 1 Target voltage

### Example

- [sample_voltage.csv](../../experiments/sample_voltage.csv)
- [sample_voltage_7p5.csv](../../experiments/sample_voltage_7p5.csv)

## Output File

When the program is executed, two types of CSV files are output.

- Result CSV: A file containing the time taken for each test to change and information about the settings. The output file name can be changed with the `-o` option.
- Voltage Monitoring CSV: Information file of the voltage monitored by the FPGA. One CSV file is generated for each test. The output directory can be changed with the `-O` option.


### Srouce file

- [args.hpp](./src/args.hpp): CLI Argument header
- [config.hpp](./src/config.hpp): CSV like input file header
- [regs.hpp](./src/regs.hpp): Register access utility header
- [result.hpp](./src/result.hpp): Detect settling time function
- [args.cpp](./src/args.cpp): implements of args.hpp
- [config.cpp](./src/config.cpp): implements of config.cpp
- [result.cpp](./src/result.cpp): implements of result.hpp
- [main.cpp](./src/main.cpp): main program
