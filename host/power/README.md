# power

This directory contains a Windows program that performs power and error rate measurements.

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
make -j power-measure
```

## Input File Format

The input file is a configuration file in a CSV-like format. Lines beginning with # and empty lines are ignored.

The following is a description of the test settings to be described in each line.

- Speed: Transceiver speed
    - 2.5g: 2.5Gbps Transceiver
    - 5g: 5Gbps Transceiver
    - 7.5g: 7.5Gbps Transceiver
    - 10g: 10Gbps Transceiver
- Device0 initial voltage.
- Device0 last voltage.
- Device1 initial voltage.
- Device1 last voltage.
- Step: voltage step
- parameter n

### Sample file

- [experiments/sample_power.csv](../../experiments/sample_power.csv)
- [experiments/sample_power_7p5.csv](../../experiments/sample_power_7p5.csv)

### Voltage and step

Device x initial voltage and Device x last voltage specify the range within which the MGTAVCC voltage is automatically set. The number of times the test is run is determined by the value of Step: Step + 1 time.

The MGTAVCC voltage for the nth test with this setting is expressed by the following equation.

$V_n = \frac{(\text{step} - (n - 1))V_{\text{init}} + (n - 1) V_{\text{last}}}{\text{step}}$


For example, if initial voltage = 0.6 V, last voltage = 0.8 V, and Step = 2, the first test will test with MGTAVCC set at 0.6 V, the second test at 0.7 V, and the third test at 0.8 V.

### Output file

When a test is run, a single results CSV file is generated.

The CSV file is appended with results as each test is completed.

Each line describes the line number of the configuration file, the set voltage, the number of data received, the number of bits and BER with errors, the maximum, minimum, average, standard deviation and latency of the power.

### Srouce file

- [args.hpp](./src/args.hpp): CLI Argument header
- [config.hpp](./src/config.hpp): CSV like input file header
- [regs.hpp](./src/regs.hpp): Register access utility header
- [args.cpp](./src/args.cpp): implements of args.hpp
- [config.cpp](./src/config.cpp): implements of config.cpp
- [main.cpp](./src/main.cpp): main program



