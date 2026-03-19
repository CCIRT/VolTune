# voltage

Provides a design for voltage measurement.

## How to build

⚠️ A licence is required to synthesise this project.

Please set `XILINXD_LICENSE_FILE` environment variables.


```sh
cd <repository top>
./build_device.sh voltage
```

If you want to build them individually, execute the command as follows

```sh
cd <repository top>
mkdir build-evm
cd build-evm
cmake ../device \
  -DVITIS_HLS_ROOT=<VitisHLS Install Directory> \
  -DVIVADO_ROOT=<VivadoHLS Install Directory> \
  -DVITIS_ROOT=<Vitis Install Directory>
make impl_voltage_n000
```

#### Make targets

- impl_voltage_n000
- impl_voltage_tx_10g
- impl_voltage_rx_10g
- impl_voltage_loopback_10g
- impl_voltage_tx_7p5g
- impl_voltage_rx_7p5g
- impl_voltage_loopback_7p5g
- impl_voltage_tx_5g
- impl_voltage_rx_5g
- impl_voltage_loopback_5g
- impl_voltage_tx_2p5g
- impl_voltage_rx_2p5g
- impl_voltage_loopback_2p5g
- bit_voltage_n000_swpmbus_vitis
- bit_voltage_tx_10g_swpmbus_vitis
- bit_voltage_rx_10g_swpmbus_vitis
- bit_voltage_loopback_10g_swpmbus_vitis
- bit_voltage_tx_7p5g_swpmbus_vitis
- bit_voltage_rx_7p5g_swpmbus_vitis
- bit_voltage_loopback_7p5g_swpmbus_vitis
- bit_voltage_tx_5g_swpmbus_vitis
- bit_voltage_rx_5g_swpmbus_vitis
- bit_voltage_loopback_5g_swpmbus_vitis
- bit_voltage_tx_2p5g_swpmbus_vitis
- bit_voltage_rx_2p5g_swpmbus_vitis
- bit_voltage_loopback_2p5g_swpmbus_vitis


Note: impl_voltage_xxx are HW PowerManager designs. bit_voltage_xxx are SW PowerManager designs.


### Bitstream List

- hw_n000.bit
- hw_l025_c125_000.bit
- hw_l050_c125_000.bit
- hw_l075_c117_188.bit
- hw_l100_c125_000.bit
- hw_r025_c125_000.bit
- hw_r050_c125_000.bit
- hw_r075_c117_188.bit
- hw_r100_c125_000.bit
- hw_t025_c125_000.bit
- hw_t050_c125_000.bit
- hw_t075_c117_188.bit
- hw_t100_c125_000.bit
- sw_n000.bit
- sw_l025_c125_000.bit
- sw_l050_c125_000.bit
- sw_l075_c117_188.bit
- sw_l100_c125_000.bit
- sw_r025_c125_000.bit
- sw_r050_c125_000.bit
- sw_r075_c117_188.bit
- sw_r100_c125_000.bit
- sw_t025_c125_000.bit
- sw_t050_c125_000.bit
- sw_t075_c117_188.bit
- sw_t100_c125_000.bit

## Open Vivado GUI

```sh
make open_voltage_n000
```

## Directories

```
└── sw_pmbus: Design which using SW based PMBus
```

## File

- [design_1.tcl](./design_1.tcl): IP Integrator file
- [design_1_with_transceiver.tcl](./design_1_with_transceiver.tcl): IP Integrator file with transceiver
- [CMakeLists.txt](./CMakeLists.txt): CMake file
- [README.md](./README.md): This file
