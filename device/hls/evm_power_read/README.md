# EVM Power Monitoring IP

This directory contains the HLS IP used for power monitoring experiments in the VolTune repository.

This IP is used in the [`evm_power_test`]  design(../../vivado/evm/).

It is not used by the KC705-based VolTune evaluation flow.

## Related files

- [`../../vivado/evm/README.md`](../../vivado/evm/README.md), corresponding EVM experimental designs
- [`../README.md`](../README.md), parent device-side overview

## Notes

- This IP is intended for EVM-specific power monitoring experiments.
- It is separate from the main KC705 transceiver case-study flow.
