project(data_aligner)

define_hls_project(data_aligner
  TOP     DataAligner
  NAME    DataAligner
  VERSION 1.0
  PERIOD  ${FPGA_PERIOD}
  PART    ${FPGA_PART}
  SOURCES src/data_aligner.cpp
  LINK    hls_common
  DEFINE  ${FPGA_MACRO}
  TB_SOURCES src/tb_data_aligner.cpp
  TB_LINK tbutil
)

set_target_properties(test_data_aligner
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
