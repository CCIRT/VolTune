set(BOARD_CONFIG ${CMAKE_CURRENT_BINARY_DIR}/set_board_path.tcl)
configure_file(set_board_path.config.tcl ${BOARD_CONFIG})

define_vivado(evm_voltage_test
  TOP        design_1_wrapper
  BOARD      "See set_board_path.config.tcl file"
  TCL0       ${BOARD_CONFIG}
  RTL        ${RTL_DIR}
  CONSTRAINT evm_coraz707s.xdc
  IP         ${BUILD_HLS_DIR}
  DESIGN     design_1.tcl
  DEPENDS    csynth_power_manager
             csynth_voltage_test_manager
)

define_vivado(evm_power_test
  TOP        design_2_wrapper
  BOARD      "See set_board_path.config.tcl file"
  TCL0       ${BOARD_CONFIG}
  RTL        ${RTL_DIR}
  CONSTRAINT evm_coraz707s.xdc
  IP         ${BUILD_HLS_DIR}
  DESIGN     design_2.tcl
  DEPENDS    csynth_power_manager
             csynth_evm_power_reader
)


# copy bitstream and ltx
add_custom_command(
  TARGET impl_evm_voltage_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bitstream/evm_voltage_test
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:evm_voltage_test,BITSTREAM> ${CMAKE_BINARY_DIR}/bitstream/evm_voltage_test/top.bit
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:evm_voltage_test,LTX> ${CMAKE_BINARY_DIR}/bitstream/evm_voltage_test/top.ltx
)

add_custom_command(
  TARGET impl_evm_power_test POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bitstream/evm_power_test
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:evm_power_test,BITSTREAM> ${CMAKE_BINARY_DIR}/bitstream/evm_power_test/top.bit
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:evm_power_test,LTX> ${CMAKE_BINARY_DIR}/bitstream/evm_power_test/top.ltx
)


define_vivado(evm_vitis_design
  TOP        vitis_design_wrapper
  DESIGN     vitis_design.tcl
  BOARD      "See set_board_path.config.tcl file"
  TCL0       ${BOARD_CONFIG}
  RTL        ${RTL_DIR}
  CONSTRAINT vitis_design.xdc
)

define_vitis(evm_vitis
  XSA evm_vitis_design
  PROC  ps7_cortexa9_0
  SRC ${VITIS_SRC}/pmbus.cpp
      ${VITIS_SRC}/power_manager.cpp
      vitis_src/main.cpp
  INCDIR ${VITIS_SRC}
  DEFS UCD9224EVM # for pm_common_host.hpp
)
