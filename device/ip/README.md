# Transceiver IP Generation Scripts

This directory contains Tcl scripts used to generate transceiver IP for the VolTune repository.

These scripts define speed-specific transceiver configurations for loopback, TX, and RX designs, and are used by the Vivado build flow.

## File naming rules

For naming files, add `loopback` at the beginning for loopback, `rx` for RX, and `tx` for TX. Add `_gt_` next, and then the transfer speed:

- `10g` for 10 Gbps
- `5g` for 5 Gbps
- `7p5g` for 7.5 Gbps
- `2p5g` for 2.5 Gbps

Examples:

- `rx_gt_2p5g.tcl`: 2.5 Gbps RX transceiver
- `loopback_gt_10g.tcl`: 10 Gbps loopback transceiver

## Files

- `gt_base.tcl`: base configuration of the transceiver
- `XXX_gt_YYYg.tcl`: see the **File naming rules** section
- `README.md`: this file

## Related files

- [`../README.md`](../README.md), parent device-side overview
- [`../vivado/README.md`](../vivado/README.md), Vivado design hierarchy

## Notes

- These scripts are build-critical inputs to the transceiver design flow.
- Do not rename these files unless the dependent Vivado build flow has been revalidated.
