# Voltage Measurement Tool

This directory contains the Windows host-side program used for voltage-transition measurement in the VolTune repository.

The program is intended to run from the MSYS2 environment and is used together with the FPGA-side voltage-control designs. It automatically writes the appropriate bitstream, configures the voltage-change parameters, records monitored voltage data, and saves settling-time-related results.

## CAUTION

If an error occurs unexpectedly while running the test program, there is most likely a fatal problem with PMBus communication. If the test program is run again without powering off the KC705, the power settings of the KC705 may be corrupted, potentially causing fatal damage to the device.

Always restart the KC705 power before re-running the test.

## How to build

Execute the following command on MSYS2 (`C:\msys64\mingw64.exe`).

```bash
cd <Repository top>
./msys_build.sh
```

If you want to build them individually, execute the command as follows:

```bash
cd <Repository top>
mkdir build
cd build
cmake ../host
make -j voltage-measure
```

If the build is successful, the executable is generated under:

```text
build/bin/voltage-measure.exe
```

## Input file format

The input file is a configuration file in a CSV-like format. Lines beginning with `#` and empty lines are ignored.

Each line should contain the following six pieces of information.

- Lane: Test voltage lane name
  - `vccint`
  - `vccaux`
  - `vcc3v3`
  - `vadf`
  - `vcc2v5`
  - `vcc1v5`
  - `mgtavcc`
  - `mgtavtt`
  - `accaux_io`
  - `vccbram`
  - `mgtvccaux`
- Speed: Transceiver speed
  - `none`: no transceiver
  - `2.5g`: 2.5 Gbps transceiver
  - `5g`: 5 Gbps transceiver
  - `7.5g`: 7.5 Gbps transceiver
  - `10g`: 10 Gbps transceiver
- Device 0 initial voltage
- Device 0 target voltage
- Device 1 initial voltage
- Device 1 target voltage

### Sample files

- [experiments/sample_voltage.csv](../../experiments/sample_voltage.csv)
- [experiments/sample_voltage_7p5.csv](../../experiments/sample_voltage_7p5.csv)

For example, the following line means that, using bitstreams without transceivers, Device0 changes `MGTAVCC` from 1.0 V to 0.7 V, and Device1 changes `MGTAVCC` from 0.8 V to 1.0 V.

```text
mgtavcc, none, 1.0, 0.7, 0.8, 1.0
```

## Output file format

When the program is executed, two types of CSV files are output.

- Result CSV: a file containing the time taken for each test to change and information about the settings. The output file name can be changed with the `-o` option.
- Voltage Monitoring CSV: an information file of the voltage monitored by the FPGA. One CSV file is generated for each test. The output directory can be changed with the `-O` option.

## Command line interface

```text
voltage-measure.exe <CONFIG FILE> [OPTIONS]
```

### Arguments

- `<CONFIG FILE>`: test configuration CSV file

### Options

- `-h`: show help
- `-b <directory>`: bitstream directory path, default is `./bitstream`
- `-c <MHz>`: external clock frequency in MHz, default is `125.000`
- `-e <error margin>`: error margin of voltage to detect settling times, default is `0.04` (= 4%)
- `-f <MHz>`: FPGA base clock in MHz, default is `100`
- `-n <size>`: number of monitored voltage values to be read from the device, maximum is `2048`, default is `100`
- `-o <file>`: output result CSV file, default is `v_result.csv`
- `-O <directory>`: output voltage CSV directory
- `-p <port>`: `hw_server` port
- `-r <repeat>`: number of times the same test is run repeatedly
- `-s <speed>`: PMBus speed, value is `100k`, `400k`, or `1m`, default is `400k`
- `-u <URL>`: `hw_server` URL
- `-x <xsdb path>`: `xsdb` path
- `-y`: start test without warning message
- `-w <wait count>`: time from setting the initial voltage and setting the target voltage, default is `0.5[s]`
- `-sw`: test software PowerManager only
- `-hw`: test hardware PowerManager only
- `--log`: show information-level log

## Execution method

This tool supports testing at no transceiver, 2.5 Gbps, 5 Gbps, 7.5 Gbps, and 10 Gbps.

1. Connect the Windows PC, clock board Si5391, and KC705 boards.
2. Create test input file `input1.csv` for no transceiver, 2.5 Gbps, 5 Gbps, and 10 Gbps.
   - The external clock for 2.5 Gbps, 5 Gbps, and 10 Gbps is 125 MHz, so you can test with only one configuration file.
3. Create test input file `input2.csv` for 7.5 Gbps.
4. Set the clock output to 125 MHz in CBPro.
5. Run:

```bash
voltage-measure.exe -b <Bitstream Directory> -o result1.csv -O result1 input1.csv
```

6. Set the clock output to 117.188 MHz in CBPro.
7. Run:

```bash
voltage-measure.exe -b <Bitstream Directory> -o result2.csv -O result2 -c 117.188 input2.csv
```

To run one setting multiple times, use the `-r` option to specify the number of times to repeat the setting.

### Helper script

The following script automates the above steps:

- [experiments/msys_voltage_run.sh](../../experiments/msys_voltage_run.sh)

It uses:

- [experiments/sample_voltage.csv](../../experiments/sample_voltage.csv)
- [experiments/sample_voltage_7p5.csv](../../experiments/sample_voltage_7p5.csv)

Results are output to the `results` folder.

## Source files

- [args.hpp](./src/args.hpp): CLI Argument header
- [config.hpp](./src/config.hpp): CSV like input file header
- [regs.hpp](./src/regs.hpp): Register access utility header
- [result.hpp](./src/result.hpp): Detect settling time function
- [args.cpp](./src/args.cpp): implementation of `args.hpp`
- [config.cpp](./src/config.cpp): implementation of `config.hpp`
- [result.cpp](./src/result.cpp): implementation of `result.hpp`
- [main.cpp](./src/main.cpp): main program

## Related files

- [`../README.md`](../README.md), parent host-side overview
- [`../../experiments/README.md`](../../experiments/README.md), experiment inputs and helper files
- [`../../device/vivado/voltage/README.md`](../../device/vivado/voltage/README.md), corresponding FPGA-side voltage-control designs

## Notes

- This tool is intended for controller characterization and voltage-transition measurement rather than BER and power case-study evaluation.
- The host-side flow depends on the corresponding FPGA bitstreams and board setup being prepared correctly.
- Clock-board setup is not automated by this tool and must be prepared separately.
