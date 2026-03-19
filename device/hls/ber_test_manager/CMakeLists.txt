project(ber_test_manager)

define_hls_project(ber_test_manager
  TOP     BerTestManager
  NAME    BerTestManager
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/ber_test_manager.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
  TB_SOURCES src/tb_ber_test_manager.cpp
  TB_LINK tbutil
)

set_target_properties(test_ber_test_manager
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
