# Experiment Assets and Helper Scripts

This directory contains helper scripts, sample configuration files, and clock-board project files used for the VolTune experiments.

These files support the host-side measurement tools and automate common experiment flows for voltage-transition measurement and BER/power measurement.

## Directory structure

```text
experiments/
├── c/                      # Small experiment-related C/C++ utilities
├── 117_188MHz.reg.txt      # 117.188 MHz clock-board register file
├── 125_000MHz.reg.txt      # 125.000 MHz clock-board register file
├── 117_188MHz.slabtimeproj # 117.188 MHz clock-board project file
├── 125_000MHz.slabtimeproj # 125.000 MHz clock-board project file
├── msys_voltage_run.sh     # Helper script for voltage-transition measurement
├── msys_power_run.sh       # Helper script for BER/power measurement
├── sample_voltage.csv      # Sample voltage configuration for none, 2.5G, 5G, and 10G
├── sample_voltage_7p5.csv  # Sample voltage configuration for 7.5G
├── sample_power.csv        # Sample power configuration for 2.5G, 5G, and 10G
└── sample_power_7p5.csv    # Sample power configuration for 7.5G
```

## `c/` folder

The C/C++ code created for small experiment-related utilities is stored here.


## CBPro files

- `117_188MHz.reg.txt`: 117.188 MHz register file
- `125_000MHz.reg.txt`: 125.000 MHz register file
- `117_188MHz.slabtimeproj`: 117.188 MHz project file
- `125_000MHz.slabtimeproj`: 125.000 MHz project file

These files are used to configure the Skyworks clock board for the required transceiver reference-clock settings.

## Voltage Measure

Files:

- `msys_voltage_run.sh`
- `sample_voltage.csv`: none, 2.5 Gbps, 5 Gbps, and 10 Gbps test configuration file
- `sample_voltage_7p5.csv`: 7.5 Gbps test configuration file

It can be executed in MSYS2 with the following command.

```sh
./msys_voltage_run.sh <Bitstream Dir>
```

The clock board is automatically configured using CBPro.

## BER/Power Measure

Files:

- `msys_power_run.sh`
- `sample_power.csv`: 2.5 Gbps, 5 Gbps, and 10 Gbps test configuration file
- `sample_power_7p5.csv`: 7.5 Gbps test configuration file

It can be executed in MSYS2 with the following command.

```sh
./msys_power_run.sh <Bitstream Dir>
```

The clock board is automatically configured using CBPro.

## Related files

- [`../host/voltage/README.md`](../host/voltage/README.md), voltage measurement tool
- [`../host/power/README.md`](../host/power/README.md), BER/power measurement tool
- [`../device/vivado/power/README.md`](../device/vivado/power/README.md), FPGA-side power-oriented designs
- [`../device/vivado/voltage/README.md`](../device/vivado/voltage/README.md), FPGA-side voltage-control designs
