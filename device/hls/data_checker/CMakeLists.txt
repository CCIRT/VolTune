project(data_checker)

define_hls_project(data_checker
  TOP     DataChecker
  NAME    DataChecker
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/data_checker.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
  TB_SOURCES src/tb_data_checker.cpp
  TB_LINK tbutil
)

set_target_properties(test_data_checker
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
