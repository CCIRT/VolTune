# ip

TCL scripts for generating Transceiver IP.

## File naming rules

For naming files, add 'loopback' at the beginning for loopback, 'rx' for Rx, and 'tx' for Tx. Add '_gt_' next, and then the transfer speed: '10g' for 10Gbps, '5g' for 5Gbps, '7p5g' for 7.5Gbps, and '2p5g' for 2.5Gbps.

- `rx_gt_2p5g.tcl`: 2.5Gbps Rx Transceiver.
- `loopback_gt_10g.tcl`: 10Gbps loopback Transceiver.

## Files

- gt_base.tcl: Base configuration of the Transceiver.
- XXX_gt_YYYg.tcl: see "File naming rules" section
- README.md: This file
