project(power_manager)

define_hls_project(power_manager
  TOP     PowerManager
  NAME    PowerManager
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/power_manager.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
  TB_SOURCES src/tb_power_manager.cpp
  TB_LINK tbutil
)
