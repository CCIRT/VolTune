# Device-side Design

This directory contains the FPGA-side sources for VolTune, including the hardware control logic, software-assisted control components, Vivado design configurations, and supporting IP generation scripts.

The design targets PMBus-controlled FPGA platforms and, in the current repository, is instantiated on the Xilinx Kintex-7 KC705 platform with a TI UCD9248 programmable power controller. The architectural structure is portable, but reuse on another platform requires board-specific adaptation of rail mapping, voltage handling, and operating limits.

For PMBus concepts, data formats, command usage, and the subset of PMBus behavior relevant to this repository, see [`../docs/PMBUS.md`](../docs/PMBUS.md).

## Directory structure

```text
device/
├── hls/         # Vitis HLS modules used in the control and test infrastructure
├── rtl/         # Handwritten RTL modules, including PMBus-related logic
├── ip/          # IP generation scripts and wrapper files
├── vitis_src/   # Software-side sources for the MicroBlaze-based control path
├── vivado/      # Vivado project configurations and design variants
└── constraint/  # XDC constraint files for supported design targets
```

## Main components

### `hls/`

This directory contains HLS-based modules used in the VolTune control and evaluation flow. These include controller-side modules, test managers, and utility blocks used by the FPGA designs.

Representative modules include:

- [`power_manager/`](hls/power_manager/), hardware control path support
- [`voltage_test_manager/`](hls/voltage_test_manager/), runtime voltage control and measurement support
- [`ber_test_manager/`](hls/ber_test_manager/), BER and power experiment support
- [`test_app_base/`](hls/test_app_base/), common test application infrastructure

### `rtl/`

This directory contains handwritten Verilog modules used by the FPGA designs. It includes PMBus transaction logic, design wrappers, test support modules, and top-level RTL blocks for different runtime control configurations.

Representative modules include:

- PMBus interface modules
- loopback, RX, and TX top-level variants
- hardware wrappers for voltage-control experiments

### `ip/`

This directory contains IP generation scripts and wrapper files for transceiver-related designs and supporting infrastructure. These files are used by the Vivado build flow and should be treated as build-critical.

### `vitis_src/`

This directory contains software-side sources used by the MicroBlaze-based control path. It is used when the PowerManager is implemented as software running on an embedded processor rather than as dedicated FPGA logic.

### `vivado/`

This directory contains the main Vivado design hierarchy, including:

- power-oriented designs
- voltage-control designs
- EVM-related designs
- reference design examples

This is the main entry point for design-specific FPGA build targets.

### `constraint/`

This directory contains XDC files for the supported design variants and operating modes.

## Control-path view

The FPGA-side implementation supports two control paths:

- a **hardware control path**, where the PowerManager is implemented directly in FPGA logic
- a **software control path**, where the PowerManager is implemented on **MicroBlaze**

The hardware path is more lightweight and deterministic. The software path is more flexible and easier to extend.

## Build

From the repository root, FPGA designs can be built with:

```bash
./build_device.sh all
```

Generated outputs are placed under the repository build directory, including bitstreams for supported targets.

For design-specific details, see the README files under:

- [`device/vivado/`](vivado/)
- [`device/vivado/power/`](vivado/power/)
- [`device/vivado/voltage/`](vivado/voltage/)

## Notes

- VolTune should be interpreted here as a runtime actuation mechanism, while safety limits and operating-point policy remain platform-specific.
- This directory contains build-critical sources. Do not rename IP identifiers, packaged-module names, or Vivado design references unless you have verified that the full build flow still works.
- Some identifiers are intentionally preserved for compatibility with the existing Vivado, Vitis, and HLS build flow.
- The transceiver-related designs in this repository are a representative case study used to validate the VolTune control architecture.
