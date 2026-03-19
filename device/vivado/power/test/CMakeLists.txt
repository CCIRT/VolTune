add_custom_target(power_test_loopback
  COMMAND
    ${VIVADO_BIN_DIR}/xsdb ${CMAKE_CURRENT_SOURCE_DIR}/power_test_loopback.tcl
)
add_custom_target(power_test_dual
  COMMAND
    ${VIVADO_BIN_DIR}/xsdb ${CMAKE_CURRENT_SOURCE_DIR}/power_test_dual.tcl
)
