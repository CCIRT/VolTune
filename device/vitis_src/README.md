# Software PowerManager Sources

This directory stores the common implementation code for the software control path in VolTune.

It contains the shared source files used by the Software PowerManager implementation.

## Files

- [pmbus.hpp](./pmbus.hpp): PMBus header file
- [power_manager.hpp](./power_manager.hpp): Software PowerManager header file
- [pmbus.cpp](./pmbus.cpp): implementation of `pmbus.hpp`
- [power_manager.cpp](./power_manager.cpp): implementation of `power_manager.hpp`
- [README.md](./README.md): This file

## Related files

- [`../README.md`](../README.md), parent device-side overview
- [`../vivado/voltage/README.md`](../vivado/voltage/README.md), voltage-control designs
- [`../vivado/power/README.md`](../vivado/power/README.md), power-oriented designs

## Notes

- These files are shared software-side sources for the MicroBlaze-based control path.
- Do not rename these files unless the dependent Vitis and Vivado integration flow has been revalidated.

