project(voltage_test_manager)

define_hls_project(voltage_test_manager
  TOP     VTestManager
  NAME    VoltageTestManager
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/vtest_manager.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
  TB_SOURCES src/tb_vtest_manager.cpp
  TB_LINK tbutil
)

set_target_properties(test_voltage_test_manager
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
