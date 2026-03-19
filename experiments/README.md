# experiments

In this folder is placed the code created for the experiment.

## c folder

The C language code created in the CRC8 experiment is stored.

## CBPro File

- 117_188MHz.reg.txt : 117.188MHz register file
- 125_000MHz.reg.txt : 125.000MHz register file
- 117_188MHz.slabtimeproj: 117.188 MHz project file
- 125_000MHz.slabtimeproj: 125.000 MHz project file

## Voltage Measure

- msys_voltage_run.sh
- sample_voltage.csv : Non, 2.5Gbps, 5Gbps, 10Gbps test configuration file
- sample_voltage_7p5.csv: 7.5 Gbps configuration file

It can be executed in MSYS2 with the following command.

```sh
./msys_voltage_run <Bitstream Dir>
```

The clock board is automatically configured using CBPro. 

## BER/Power Measure

- msys_power_run.sh
- sample_power.csv: 2.5Gbps, 5Gbps, 10Gbps test configuration file
- sample_power_7p5.csv: 7.5 Gbps configuration file

It can be executed in MSYS2 with the following command.

```sh
./msys_voltage_run <Bitstream Dir>
```

The clock board is automatically configured using CBPro. 
