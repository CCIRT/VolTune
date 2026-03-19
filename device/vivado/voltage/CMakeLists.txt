add_subdirectory(sw_pmbus)

# non transceiver
define_vivado(voltage_n000
  TOP        design_1_wrapper
  BOARD      *kc705*
  RTL        ${RTL_DIR}
  CONSTRAINT ${CONSTRAINT_DIR}/kc705.xdc
  IP         ${BUILD_HLS_DIR}
  DESIGN     design_1.tcl
  DEPENDS    csynth_power_manager
             csynth_voltage_test_manager
)

# copy bitstream and ltx
set(OUTDIR ${CMAKE_BINARY_DIR}/bitstream/voltage)
after_copy_bitstream(voltage_n000 ${OUTDIR} hw_n000)

# with transceiver
function(add_voltage_prj direction speed id clk)
  # Set constraint name
  set (CONSTRAINT ${CONSTRAINT_DIR}/voltage_${direction}.xdc)
  if(${speed} STREQUAL 7p5g)
    set (CONSTRAINT ${CONSTRAINT_DIR}/voltage_${direction}_${speed}.xdc)
  endif()

  set (prj voltage_${direction}_${speed})
  define_vivado(${prj}
    TOP        voltage_${direction}
    BOARD      *kc705*
    TCL1       ${IP_DIR}/${direction}_gt_${speed}.tcl
    RTL        ${RTL_DIR}
    CONSTRAINT ${CONSTRAINT_DIR}/kc705.xdc
               ${CONSTRAINT}
    IP         ${BUILD_HLS_DIR}
    DESIGN     design_1_with_transceiver.tcl
    DEPENDS    csynth_power_manager
               csynth_voltage_test_manager
  )
  after_copy_bitstream(${prj} ${OUTDIR} hw_${id}_c${clk})
endfunction()

add_voltage_prj(tx 10g  t100 125_000)
add_voltage_prj(tx 7p5g t075 117_188)
add_voltage_prj(tx 5g   t050 125_000)
add_voltage_prj(tx 2p5g t025 125_000)

add_voltage_prj(rx 10g  r100 125_000)
add_voltage_prj(rx 7p5g r075 117_188)
add_voltage_prj(rx 5g   r050 125_000)
add_voltage_prj(rx 2p5g r025 125_000)

add_voltage_prj(loopback 10g  l100 125_000)
add_voltage_prj(loopback 7p5g l075 117_188)
add_voltage_prj(loopback 5g   l050 125_000)
add_voltage_prj(loopback 2p5g l025 125_000)
