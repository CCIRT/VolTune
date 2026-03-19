# Vivado Design Hierarchy

This directory contains the main Vivado-side design hierarchy for VolTune. It organizes the FPGA build targets, design variants, and supporting project files used to instantiate the runtime voltage-control framework on the target platform.

In the current repository, these designs target the Xilinx Kintex-7 KC705 platform and are organized around the main VolTune use cases, including runtime voltage control, power-oriented transceiver experiments, and auxiliary evaluation designs.

## Directory structure

```text
vivado/
├── power/           # Power-oriented transceiver experiment designs
├── voltage/         # Voltage-control and transition-measurement designs
├── evm/             # EVM-related designs and support files
├── design-example/  # Reference transceiver design examples
├── vitis_src/       # Sources used by Vivado/Vitis-integrated designs
└── CMakeLists.txt   # Vivado-side build integration
```

## Main components

### `power/`

This directory contains the main power-oriented design family used for the representative transceiver case study. It includes design variants for different line rates and operation modes, such as TX, RX, and loopback configurations.

It is the main entry point for BER, latency, and rail-power characterization with VolTune.

See also:

- [`power/README.md`](power/README.md)

### `voltage/`

This directory contains the designs used for runtime voltage-control experiments and settling-time measurement. These designs focus on issuing voltage-control commands, measuring voltage transitions, and comparing different control paths.

See also:

- [`voltage/README.md`](voltage/README.md)

### `evm/`

This directory contains EVM-related designs and support files used for auxiliary experiments and board-level evaluation flows.

### `design-example/`

This directory contains reference transceiver design examples used as supporting design material. These examples are useful for understanding the baseline communication structure and transceiver integration style used in the repository.

See also:

- [`design-example/README.md`](design-example/README.md)

### `vitis_src/`

This directory contains software-side sources used by Vivado/Vitis-integrated designs, including designs that rely on embedded software components.

## Design organization

The Vivado hierarchy is organized around distinct design families rather than a single monolithic top-level design.

The two main VolTune-related branches are:

- **power-oriented designs**, used for BER, latency, and rail-power experiments
- **voltage-oriented designs**, used for runtime voltage-control and transition-measurement experiments

This separation keeps controller characterization and application-level case studies distinct, while still sharing the same underlying runtime voltage-control framework.

## Build

Vivado designs are built from the repository root through the main build script:

```bash
./build_device.sh all
```

Design-specific build behavior is controlled by the CMake and Tcl files under this hierarchy and its subdirectories.

For design-specific details, see:

- [`power/README.md`](power/README.md)
- [`voltage/README.md`](voltage/README.md)
- [`design-example/README.md`](design-example/README.md)

## Notes

- This directory contains build-critical Vivado project files, Tcl scripts, and design references.
- Do not rename packaged IP identifiers, design Tcl references, or project integration files unless the full Vivado build flow has been revalidated.
- The `power/` and `voltage/` hierarchies are the most important design branches for understanding and reproducing the VolTune evaluation flow.
