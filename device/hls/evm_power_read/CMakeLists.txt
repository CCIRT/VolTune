project(evm_power_read)

define_hls_project(evm_power_reader
  TOP     EvmPowerReader
  NAME    EvmPowerReader
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/evm_power_reader.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
)
