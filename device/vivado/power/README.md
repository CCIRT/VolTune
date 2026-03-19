# Power-oriented Designs

This directory contains the Vivado designs and supporting project files used for BER, latency, and rail-power measurements in the VolTune repository.

These designs form the main FPGA-side implementation for the representative transceiver case study. They are organized by transceiver line rate and test direction, and support both the hardware-based control path and the software-based PMBus control path.

## Scope

The `power/` hierarchy contains designs for:

- **single-board loopback tests**
- **dual-board TX/RX tests**
- **multiple transceiver line rates**, 2.5 Gbps, 5 Gbps, 7.5 Gbps, and 10 Gbps
- **two control implementations**:
  - hardware-based PMBus control
  - software-based PMBus control, under `swpmbus/`

## Build

These designs can be built from the repository root with the top-level build flow:

```bash
./build_device.sh all
```

For manual Vivado build control, use the following commands:

```bash
cd <Repository_top>
mkdir build && cd build
cmake -DVIVADO_ROOT=<Path_to_Vivado_2022.1> -DVITIS_HLS_ROOT=<Path_to_Vitis_HLS_2022.1> -DVITIS_ROOT=<Path_to_Vitis_2022.1> ../device
make impl_<Target_direction>_<Target_speed>
```

Example:

```bash
cmake -DVIVADO_ROOT=/opt/Xilinx/Vivado/2022.1 -DVITIS_HLS_ROOT=/opt/Xilinx/Vitis_HLS/2022.1 -DVITIS_ROOT=/opt/Xilinx/Vitis/2022.1 ../device
make impl_loopback_10g
```

After a successful build, bitstreams are generated under:

```text
<Repository_top>/build/bitstream/power
```

## Target naming

### `<Target_direction>`

- `loopback`, single-board test
- `tx`, TX-side design for dual-board test
- `rx`, RX-side design for dual-board test

### `<Target_speed>`

- `2p5g`, transceivers set to 2.5 Gbps
- `5g`, transceivers set to 5 Gbps
- `7p5g`, transceivers set to 7.5 Gbps
- `10g`, transceivers set to 10 Gbps

## Build targets

### Hardware-control designs

- `impl_tx_10g`
- `impl_rx_10g`
- `impl_loopback_10g`
- `impl_tx_7p5g`
- `impl_rx_7p5g`
- `impl_loopback_7p5g`
- `impl_tx_5g`
- `impl_rx_5g`
- `impl_loopback_5g`
- `impl_tx_2p5g`
- `impl_rx_2p5g`
- `impl_loopback_2p5g`

### Software-control designs

- `bit_tx_10g_swpmbus_vitis`
- `bit_rx_10g_swpmbus_vitis`
- `bit_loopback_10g_swpmbus_vitis`
- `bit_tx_7p5g_swpmbus_vitis`
- `bit_rx_7p5g_swpmbus_vitis`
- `bit_loopback_7p5g_swpmbus_vitis`
- `bit_tx_5g_swpmbus_vitis`
- `bit_rx_5g_swpmbus_vitis`
- `bit_loopback_5g_swpmbus_vitis`
- `bit_tx_2p5g_swpmbus_vitis`
- `bit_rx_2p5g_swpmbus_vitis`
- `bit_loopback_2p5g_swpmbus_vitis`

## Generated bitstreams

### Using hardware-based PMBus control

- `hw_l025_c125_000.bit`, Loopback, 2.5 Gbps
- `hw_t025_c125_000.bit`, TX, 2.5 Gbps
- `hw_r025_c125_000.bit`, RX, 2.5 Gbps
- `hw_l050_c125_000.bit`, Loopback, 5 Gbps
- `hw_t050_c125_000.bit`, TX, 5 Gbps
- `hw_r050_c125_000.bit`, RX, 5 Gbps
- `hw_l075_c117_188.bit`, Loopback, 7.5 Gbps
- `hw_t075_c117_188.bit`, TX, 7.5 Gbps
- `hw_r075_c117_188.bit`, RX, 7.5 Gbps
- `hw_l100_c125_000.bit`, Loopback, 10 Gbps
- `hw_t100_c125_000.bit`, TX, 10 Gbps
- `hw_r100_c125_000.bit`, RX, 10 Gbps

### Using software-based PMBus control

- `sw_l025_c125_000.bit`, Loopback, 2.5 Gbps
- `sw_t025_c125_000.bit`, TX, 2.5 Gbps
- `sw_r025_c125_000.bit`, RX, 2.5 Gbps
- `sw_l050_c125_000.bit`, Loopback, 5 Gbps
- `sw_t050_c125_000.bit`, TX, 5 Gbps
- `sw_r050_c125_000.bit`, RX, 5 Gbps
- `sw_l075_c117_188.bit`, Loopback, 7.5 Gbps
- `sw_t075_c117_188.bit`, TX, 7.5 Gbps
- `sw_r075_c117_188.bit`, RX, 7.5 Gbps
- `sw_l100_c125_000.bit`, Loopback, 10 Gbps
- `sw_t100_c125_000.bit`, TX, 10 Gbps
- `sw_r100_c125_000.bit`, RX, 10 Gbps

## Test flows

### Single-board test

#### Hardware connection

![](./img/single-board-tests-connection.drawio.svg)

#### Clock board setup

Please refer to [`clock-board-quick-setup.md`](./clock-board-quick-setup.md).

#### Write bitstream

Write a loopback design to the KC705 board.

For bitstream programming, refer to:

- [Programming the Device](https://docs.xilinx.com/r/2022.1-English/ug908-vivado-programming-debugging/Programming-the-Device)
- [Embedded System Tools Reference Manual, p.58](https://docs.xilinx.com/v/u/2015.2-English/ug1043-embedded-system-tools#page=58)

#### Run test

Please refer to [`test/README.md`](./test/README.md) for detailed test behavior.

```bash
cd <Repository_top>/build
make power_test_loopback
```

The test reports measured data size, error-bit count, latency, and power statistics.

### Dual-board test

#### Hardware connection

![](./img/dual-board-tests-connection.drawio.svg)

#### Clock board setup

Please refer to [`clock-board-quick-setup.md`](./clock-board-quick-setup.md).

#### Write bitstream

Write the TX design to KC705-0 and the RX design to KC705-1.

For bitstream programming, refer to:

- [Programming the Device](https://docs.xilinx.com/r/2022.1-English/ug908-vivado-programming-debugging/Programming-the-Device)
- [Embedded System Tools Reference Manual, p.58](https://docs.xilinx.com/v/u/2015.2-English/ug1043-embedded-system-tools#page=58)

#### Run test

Please refer to [`test/README.md`](./test/README.md) for detailed test behavior.

```bash
cd <Repository_top>/build
make power_test_dual
```

The test reports BER-related results, latency, and separate RX/TX power statistics.

## Directory map

```text
power/
├── 2p5g/
│   ├── loopback/   # Loopback design with transceivers set to 2.5 Gbps
│   ├── tx/         # TX-side design with transceivers set to 2.5 Gbps
│   └── rx/         # RX-side design with transceivers set to 2.5 Gbps
├── 5g/
│   ├── loopback/   # Loopback design with transceivers set to 5 Gbps
│   ├── tx/         # TX-side design with transceivers set to 5 Gbps
│   └── rx/         # RX-side design with transceivers set to 5 Gbps
├── 7p5g/
│   ├── loopback/   # Loopback design with transceivers set to 7.5 Gbps
│   ├── tx/         # TX-side design with transceivers set to 7.5 Gbps
│   └── rx/         # RX-side design with transceivers set to 7.5 Gbps
├── 10g/
│   ├── loopback/   # Loopback design with transceivers set to 10 Gbps
│   ├── tx/         # TX-side design with transceivers set to 10 Gbps
│   └── rx/         # RX-side design with transceivers set to 10 Gbps
├── swpmbus/
│   ├── 2p5g/
│   │   ├── loopback/   # 2.5 Gbps loopback design with software-based PMBus control
│   │   ├── tx/         # 2.5 Gbps TX-side design with software-based PMBus control
│   │   └── rx/         # 2.5 Gbps RX-side design with software-based PMBus control
│   ├── 5g/
│   │   ├── loopback/   # 5 Gbps loopback design with software-based PMBus control
│   │   ├── tx/         # 5 Gbps TX-side design with software-based PMBus control
│   │   └── rx/         # 5 Gbps RX-side design with software-based PMBus control
│   ├── 7p5g/
│   │   ├── loopback/   # 7.5 Gbps loopback design with software-based PMBus control
│   │   ├── tx/         # 7.5 Gbps TX-side design with software-based PMBus control
│   │   └── rx/         # 7.5 Gbps RX-side design with software-based PMBus control
│   └── 10g/
│       ├── loopback/   # 10 Gbps loopback design with software-based PMBus control
│       ├── tx/         # 10 Gbps TX-side design with software-based PMBus control
│       └── rx/         # 10 Gbps RX-side design with software-based PMBus control
├── img/            # Images used in this README
└── test/           # Test scripts
```

## Related files

- [`clock-board-quick-setup.md`](clock-board-quick-setup.md), clock-board setup guide
- [`test/README.md`](test/README.md), test procedure details
- [`CMakeLists.txt`](./CMakeLists.txt), CMake integration for this hierarchy
- [`README.md`](./README.md), this file

## Notes

- The `power/` hierarchy is the main design family for reproducing the VolTune transceiver case study.
- The `swpmbus/` subtree corresponds to the software control path.
- These designs are build-critical. Do not rename packaged IP identifiers, Tcl targets, or integrated design references unless the full build flow has been revalidated.
