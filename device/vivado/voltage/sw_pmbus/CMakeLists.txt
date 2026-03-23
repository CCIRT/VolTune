# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php

# non transceiver
define_vivado(voltage_n000_swpmbus
  TOP        voltage_n000_swpmbus
  BOARD      *kc705*
  RTL        ${RTL_DIR}
  CONSTRAINT ${CONSTRAINT_DIR}/kc705.xdc
  IP         ${BUILD_HLS_DIR}
  DESIGN     design_1.tcl
  DEPENDS    csynth_voltage_test_manager
)

define_vitis(voltage_n000_swpmbus_vitis
  DEBUG
  XSA voltage_n000_swpmbus
  PROC pmbus_microblaze_0
  PROCPATH design_1_i/pmbus/microblaze_0
  SRC ${VITIS_SRC}/pmbus.cpp
      ${VITIS_SRC}/power_manager.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/../../vitis_src/main.cpp
  INCDIR ${VITIS_SRC}
  DEFS KC705 # for pm_common_host.hpp
)

set(OUTDIR ${CMAKE_BINARY_DIR}/bitstream/voltage)
vitis_after_copy_bitstream(
  voltage_n000_swpmbus
  voltage_n000_swpmbus_vitis
  ${OUTDIR}
  sw_n000
)

# with transceiver
function(add_voltage_prj_swpmbus direction speed id clk)
  # Set constraint name
  set (CONSTRAINT ${CONSTRAINT_DIR}/voltage_${direction}.xdc)
  if(${speed} STREQUAL 7p5g)
    set (CONSTRAINT ${CONSTRAINT_DIR}/voltage_${direction}_${speed}.xdc)
  endif()

  set (prj voltage_${direction}_${speed}_swpmbus)
  define_vivado(${prj}
    TOP        voltage_${direction}_swpmbus
    BOARD      *kc705*
    TCL1       ${IP_DIR}/${direction}_gt_${speed}.tcl
    RTL        ${RTL_DIR}
    CONSTRAINT ${CONSTRAINT_DIR}/kc705.xdc
               ${CONSTRAINT}
    IP         ${BUILD_HLS_DIR}
    DESIGN     design_1_with_transceiver.tcl
    DEPENDS    csynth_voltage_test_manager
  )

  define_vitis(${prj}_vitis
    DEBUG
    XSA ${prj}
    PROC pmbus_microblaze_0
    PROCPATH design_1_i/pmbus/microblaze_0
    SRC ${VITIS_SRC}/pmbus.cpp
        ${VITIS_SRC}/power_manager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../vitis_src/main.cpp
    INCDIR ${VITIS_SRC}
    DEFS KC705 # for pm_common_host.hpp
  )
  vitis_after_copy_bitstream(
    ${prj}
    ${prj}_vitis
    ${OUTDIR}
    sw_${id}_c${clk}
  )
endfunction()

add_voltage_prj_swpmbus(tx 10g  t100 125_000)
add_voltage_prj_swpmbus(tx 7p5g t075 117_188)
add_voltage_prj_swpmbus(tx 5g   t050 125_000)
add_voltage_prj_swpmbus(tx 2p5g t025 125_000)

add_voltage_prj_swpmbus(rx 10g  r100 125_000)
add_voltage_prj_swpmbus(rx 7p5g r075 117_188)
add_voltage_prj_swpmbus(rx 5g   r050 125_000)
add_voltage_prj_swpmbus(rx 2p5g r025 125_000)

add_voltage_prj_swpmbus(loopback 10g  l100 125_000)
add_voltage_prj_swpmbus(loopback 7p5g l075 117_188)
add_voltage_prj_swpmbus(loopback 5g   l050 125_000)
add_voltage_prj_swpmbus(loopback 2p5g l025 125_000)
