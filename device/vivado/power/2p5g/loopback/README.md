# Loopback Design, 2.5 Gbps

This directory contains the loopback design with transceivers set to 2.5 Gbps.

This design is used for BER, latency, and power measurement in the VolTune power-oriented transceiver case study.

## Overview

This is the single-board loopback design for the test flow at 2.5 Gbps.

## Details

The structure and usage of this design follow the same pattern as the 10 Gbps loopback design.

Please refer to [`../../10g/loopback/README.md`](../../10g/loopback/README.md) for the detailed design description, test flow, register map, LED behavior, and typical error codes.

## Related files

- [`../README.md`](../README.md), parent 2.5 Gbps design overview
- [`../../README.md`](../../README.md), parent power-oriented designs
- [`../tx/README.md`](../tx/README.md), corresponding TX-side design
- [`../rx/README.md`](../rx/README.md), corresponding RX-side design
