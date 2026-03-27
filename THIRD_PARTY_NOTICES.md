# Third-Party Notices

This repository includes certain third-party design flows and documentation references that are not covered by the repository-wide MIT License.

## AMD / Xilinx generated design-example support files

Some support HDL files originally associated with the transceiver design examples under `device/vivado/design-example/` were removed from the public release because they are generated or provided as part of the AMD/Xilinx 7 Series FPGAs Transceivers Wizard example/support flow.

The removed files were associated with:

- `device/vivado/design-example/loopback-5g/rtl/support/txrx5g_support.v`
- `device/vivado/design-example/loopback-5g/rtl/support/txrx5g_gt_usrclk_source.v`
- `device/vivado/design-example/loopback-5g/rtl/support/txrx5g_clock_module.v`
- `device/vivado/design-example/loopback-5g/rtl/support/txrx5g_common.v`
- `device/vivado/design-example/loopback-5g/rtl/support/txrx5g_common_reset.v`
- `device/vivado/design-example/dual-board-10g/rtl/support/txrx5g_gt_usrclk_source.v`

These removed files are not included in the public repository and are not covered by the repository-wide MIT License.

Users who independently regenerate or otherwise obtain such files must follow the original AMD/Xilinx license terms applicable to their own Vivado installation and related design-example materials.

## Repository MIT License scope

Unless otherwise stated, original VolTune repository files authored for this project are released under the MIT License in the root `LICENSE` file.

Third-party files, generated files, vendor IP outputs, reference-design materials, and files carrying their own notice headers are not relicensed by the repository-wide MIT License.

## User responsibility

Users are responsible for reviewing and complying with the licensing terms applicable to any AMD/Xilinx-generated or third-party materials that are used together with this repository.
EOF
