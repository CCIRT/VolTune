
################################################################
# This is a generated script based on design: design_2
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2022.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source design_2_script.tcl


# The design that will be created by this Tcl script contains the following 
# module references:
# axis_counter, axis_led_adaptor, axis_pmbus_wrapper, coraz707s_led

# Please add the sources of those modules before sourcing this Tcl script.

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7z007sclg400-1
   set_property BOARD_PART digilentinc.com:cora-z7-07s:part0:1.0 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name design_2

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:xlconstant:1.1\
xilinx.com:ip:clk_wiz:6.0\
Fixstars:hls:evm_power_reader:1.0\
xilinx.com:ip:jtag_axi:1.2\
Fixstars:hls:power_manager:1.0\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:smartconnect:1.0\
xilinx.com:ip:system_ila:1.1\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

##################################################################
# CHECK Modules
##################################################################
set bCheckModules 1
if { $bCheckModules == 1 } {
   set list_check_mods "\ 
axis_counter\
axis_led_adaptor\
axis_pmbus_wrapper\
coraz707s_led\
"

   set list_mods_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2020 -severity "INFO" "Checking if the following modules exist in the project's sources: $list_check_mods ."

   foreach mod_vlnv $list_check_mods {
      if { [can_resolve_reference $mod_vlnv] == 0 } {
         lappend list_mods_missing $mod_vlnv
      }
   }

   if { $list_mods_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2021 -severity "ERROR" "The following module(s) are not found in the project: $list_mods_missing" }
      common::send_gid_msg -ssname BD::TCL -id 2022 -severity "INFO" "Please add source files for the missing module(s) above."
      set bCheckIPsPassed 0
   }
}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports

  # Create ports
  set SCL [ create_bd_port -dir IO SCL ]
  set SDA [ create_bd_port -dir IO SDA ]
  set led0_b [ create_bd_port -dir O led0_b ]
  set led0_g [ create_bd_port -dir O led0_g ]
  set led0_r [ create_bd_port -dir O led0_r ]
  set led1_b [ create_bd_port -dir O led1_b ]
  set led1_g [ create_bd_port -dir O led1_g ]
  set led1_r [ create_bd_port -dir O led1_r ]
  set reset [ create_bd_port -dir I -type rst reset ]
  set_property -dict [ list \
   CONFIG.POLARITY {ACTIVE_HIGH} \
 ] $reset
  set sys_clock [ create_bd_port -dir I -type clk -freq_hz 125000000 sys_clock ]
  set_property -dict [ list \
   CONFIG.PHASE {0.0} \
 ] $sys_clock

  # Create instance: ap_start_1, and set properties
  set ap_start_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 ap_start_1 ]

  # Create instance: axis_counter_0, and set properties
  set block_name axis_counter
  set block_cell_name axis_counter_0
  if { [catch {set axis_counter_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $axis_counter_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: axis_led_adaptor_0, and set properties
  set block_name axis_led_adaptor
  set block_cell_name axis_led_adaptor_0
  if { [catch {set axis_led_adaptor_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $axis_led_adaptor_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: axis_pmbus_wrapper_0, and set properties
  set block_name axis_pmbus_wrapper
  set block_cell_name axis_pmbus_wrapper_0
  if { [catch {set axis_pmbus_wrapper_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $axis_pmbus_wrapper_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.CLK_IN1_BOARD_INTERFACE {sys_clock} \
   CONFIG.USE_BOARD_FLOW {true} \
 ] $clk_wiz_0

  # Create instance: coraz707s_led_0, and set properties
  set block_name coraz707s_led
  set block_cell_name coraz707s_led_0
  if { [catch {set coraz707s_led_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $coraz707s_led_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: evm_power_reader_0, and set properties
  set evm_power_reader_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:evm_power_reader:1.0 evm_power_reader_0 ]

  # Create instance: jtag_axi_0, and set properties
  set jtag_axi_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:jtag_axi:1.2 jtag_axi_0 ]

  # Create instance: power_manager_0, and set properties
  set power_manager_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:power_manager:1.0 power_manager_0 ]

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]

  # Create instance: smartconnect_0, and set properties
  set smartconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {1} \
   CONFIG.NUM_SI {1} \
 ] $smartconnect_0

  # Create instance: system_ila_0, and set properties
  set system_ila_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 system_ila_0 ]
  set_property -dict [ list \
   CONFIG.C_MON_TYPE {MIX} \
   CONFIG.C_NUM_MONITOR_SLOTS {4} \
   CONFIG.C_NUM_OF_PROBES {2} \
   CONFIG.C_PROBE0_TYPE {0} \
   CONFIG.C_PROBE1_TYPE {0} \
   CONFIG.C_PROBE2_TYPE {0} \
   CONFIG.C_PROBE3_TYPE {0} \
   CONFIG.C_PROBE4_TYPE {0} \
   CONFIG.C_PROBE5_TYPE {0} \
   CONFIG.C_SLOT_0_APC_EN {0} \
   CONFIG.C_SLOT_0_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_0_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_1_APC_EN {0} \
   CONFIG.C_SLOT_1_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_1_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_1_TYPE {0} \
   CONFIG.C_SLOT_2_APC_EN {0} \
   CONFIG.C_SLOT_2_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_2_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_2_TYPE {0} \
   CONFIG.C_SLOT_3_APC_EN {0} \
   CONFIG.C_SLOT_3_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_3_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_3_TYPE {0} \
   CONFIG.C_SLOT_4_APC_EN {0} \
   CONFIG.C_SLOT_4_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_4_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_4_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_5_APC_EN {0} \
   CONFIG.C_SLOT_5_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_5_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_5_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_6_APC_EN {0} \
   CONFIG.C_SLOT_6_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_6_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_6_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 ] $system_ila_0

  # Create interface connections
  connect_bd_intf_net -intf_net ack [get_bd_intf_pins evm_power_reader_0/s_axis_ack] [get_bd_intf_pins power_manager_0/m_axis_ack]
connect_bd_intf_net -intf_net [get_bd_intf_nets ack] [get_bd_intf_pins power_manager_0/m_axis_ack] [get_bd_intf_pins system_ila_0/SLOT_3_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets ack]
  connect_bd_intf_net -intf_net axis_counter_0_m_axis_cnt [get_bd_intf_pins axis_counter_0/m_axis_cnt] [get_bd_intf_pins evm_power_reader_0/s_axis_counter]
  connect_bd_intf_net -intf_net cmd [get_bd_intf_pins evm_power_reader_0/m_axis_cmd] [get_bd_intf_pins power_manager_0/s_axis_cmd]
connect_bd_intf_net -intf_net [get_bd_intf_nets cmd] [get_bd_intf_pins evm_power_reader_0/m_axis_cmd] [get_bd_intf_pins system_ila_0/SLOT_0_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets cmd]
  connect_bd_intf_net -intf_net evm_power_reader_0_m_axis_clear_counter [get_bd_intf_pins axis_counter_0/s_axis_clear] [get_bd_intf_pins evm_power_reader_0/m_axis_clear_counter]
  connect_bd_intf_net -intf_net evm_power_reader_0_m_axis_led [get_bd_intf_pins axis_led_adaptor_0/m_axis_run] [get_bd_intf_pins evm_power_reader_0/m_axis_led]
  connect_bd_intf_net -intf_net jtag_axi_0_M_AXI [get_bd_intf_pins jtag_axi_0/M_AXI] [get_bd_intf_pins smartconnect_0/S00_AXI]
  connect_bd_intf_net -intf_net pmbus_ack [get_bd_intf_pins axis_pmbus_wrapper_0/m_axis] [get_bd_intf_pins power_manager_0/s_axis_pmbus_ack]
connect_bd_intf_net -intf_net [get_bd_intf_nets pmbus_ack] [get_bd_intf_pins axis_pmbus_wrapper_0/m_axis] [get_bd_intf_pins system_ila_0/SLOT_2_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets pmbus_ack]
  connect_bd_intf_net -intf_net pmbus_cmd [get_bd_intf_pins axis_pmbus_wrapper_0/s_axis] [get_bd_intf_pins power_manager_0/m_axis_pmbus_cmd]
connect_bd_intf_net -intf_net [get_bd_intf_nets pmbus_cmd] [get_bd_intf_pins power_manager_0/m_axis_pmbus_cmd] [get_bd_intf_pins system_ila_0/SLOT_1_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets pmbus_cmd]
  connect_bd_intf_net -intf_net smartconnect_0_M00_AXI [get_bd_intf_pins evm_power_reader_0/s_axi_regs] [get_bd_intf_pins smartconnect_0/M00_AXI]

  # Create port connections
  connect_bd_net -net Net [get_bd_ports SCL] [get_bd_pins axis_pmbus_wrapper_0/SCL]
  connect_bd_net -net Net1 [get_bd_ports SDA] [get_bd_pins axis_pmbus_wrapper_0/SDA]
  connect_bd_net -net axis_counter_0_counter [get_bd_pins axis_counter_0/counter] [get_bd_pins coraz707s_led_0/counter] [get_bd_pins power_manager_0/counter]
  connect_bd_net -net clkSelect [get_bd_pins axis_pmbus_wrapper_0/i2c_clock_select] [get_bd_pins evm_power_reader_0/i2cClockSelect] [get_bd_pins system_ila_0/probe0]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets clkSelect]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins axis_counter_0/clock] [get_bd_pins axis_led_adaptor_0/clock] [get_bd_pins axis_pmbus_wrapper_0/clock] [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins coraz707s_led_0/clock] [get_bd_pins evm_power_reader_0/ap_clk] [get_bd_pins jtag_axi_0/aclk] [get_bd_pins power_manager_0/ap_clk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] [get_bd_pins smartconnect_0/aclk] [get_bd_pins system_ila_0/clk]
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked]
  connect_bd_net -net coraz707s_led_0_led0_b [get_bd_ports led0_b] [get_bd_pins coraz707s_led_0/led0_b]
  connect_bd_net -net coraz707s_led_0_led0_g [get_bd_ports led0_g] [get_bd_pins coraz707s_led_0/led0_g]
  connect_bd_net -net coraz707s_led_0_led0_r [get_bd_ports led0_r] [get_bd_pins coraz707s_led_0/led0_r]
  connect_bd_net -net coraz707s_led_0_led1_b [get_bd_ports led1_b] [get_bd_pins coraz707s_led_0/led1_b]
  connect_bd_net -net coraz707s_led_0_led1_g [get_bd_ports led1_g] [get_bd_pins coraz707s_led_0/led1_g]
  connect_bd_net -net coraz707s_led_0_led1_r [get_bd_ports led1_r] [get_bd_pins coraz707s_led_0/led1_r]
  connect_bd_net -net failLED [get_bd_pins axis_led_adaptor_0/failLED] [get_bd_pins coraz707s_led_0/failLED]
  connect_bd_net -net fatalErrorLED [get_bd_pins axis_led_adaptor_0/fatalErrorLED] [get_bd_pins coraz707s_led_0/fatalErrorLED]
  connect_bd_net -net monitoring [get_bd_pins power_manager_0/monitoring] [get_bd_pins system_ila_0/probe1]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets monitoring]
  connect_bd_net -net okLED [get_bd_pins axis_led_adaptor_0/okLED] [get_bd_pins coraz707s_led_0/okLED]
  connect_bd_net -net power_manager_0_error [get_bd_pins axis_led_adaptor_0/error] [get_bd_pins power_manager_0/error]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins axis_counter_0/resetn] [get_bd_pins axis_led_adaptor_0/resetn] [get_bd_pins axis_pmbus_wrapper_0/resetn] [get_bd_pins coraz707s_led_0/resetn] [get_bd_pins evm_power_reader_0/ap_rst_n] [get_bd_pins jtag_axi_0/aresetn] [get_bd_pins power_manager_0/ap_rst_n] [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins smartconnect_0/aresetn] [get_bd_pins system_ila_0/resetn]
  connect_bd_net -net reset_1 [get_bd_ports reset] [get_bd_pins clk_wiz_0/reset] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net runLED [get_bd_pins axis_led_adaptor_0/runLED] [get_bd_pins coraz707s_led_0/runLED]
  connect_bd_net -net sys_clock_1 [get_bd_ports sys_clock] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins ap_start_1/dout] [get_bd_pins power_manager_0/ap_start]

  # Create address segments
  assign_bd_address -offset 0x00000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces jtag_axi_0/Data] [get_bd_addr_segs evm_power_reader_0/s_axi_regs/Reg] -force


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


