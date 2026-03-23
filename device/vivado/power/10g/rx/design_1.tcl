# Copyright (c) 2026 National Institute of Advanced Industrial Science and Technology (AIST)
# All rights reserved.
# This software is released under the MIT License.
# http://opensource.org/licenses/mit-license.php


################################################################
# This is a generated script based on design: design_1
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
# source design_1_script.tcl


# The design that will be created by this Tcl script contains the following 
# module references:
# axis_counter, axis_pmbus_wrapper, axis_counter, sync_reg, ap_start_only_once, clk_counter, sync_reg

# Please add the sources of those modules before sourcing this Tcl script.

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7k325tffg900-2
   set_property BOARD_PART xilinx.com:kc705:part0:1.6 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name design_1

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
xilinx.com:ip:c_counter_binary:12.0\
xilinx.com:ip:clk_wiz:6.0\
Fixstars:hls:data_aligner:1.0\
Fixstars:hls:data_checker:1.0\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:system_ila:1.1\
xilinx.com:ip:xlconstant:1.1\
xilinx.com:ip:xlslice:1.0\
Fixstars:hls:power_manager:1.0\
xilinx.com:ip:axis_clock_converter:1.1\
Fixstars:hls:test_app_base:1.0\
xilinx.com:ip:util_vector_logic:2.0\
xilinx.com:ip:axi_gpio:2.0\
Fixstars:hls:ber_test_manager:1.0\
xilinx.com:ip:jtag_axi:1.2\
xilinx.com:ip:smartconnect:1.0\
xilinx.com:ip:xlconcat:2.1\
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
axis_pmbus_wrapper\
axis_counter\
sync_reg\
ap_start_only_once\
clk_counter\
sync_reg\
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


# Hierarchical cell: test_manager
proc create_hier_cell_test_manager { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_test_manager() - Empty argument(s)!"}
     return
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

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 ack_in

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 cmd_out

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 error_out

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 running_out


  # Create pins
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -type clk ap_clk_jtag
  create_bd_pin -dir I -type rst ap_rst_n
  create_bd_pin -dir I -type rst ap_rst_n_jtag
  create_bd_pin -dir O -from 0 -to 0 cancel_pmbus
  create_bd_pin -dir O -from 0 -to 0 cancel_test

  # Create instance: ap_start_only_once_0, and set properties
  set block_name ap_start_only_once
  set block_cell_name ap_start_only_once_0
  if { [catch {set ap_start_only_once_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $ap_start_only_once_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
   CONFIG.C_GPIO_WIDTH {4} \
 ] $axi_gpio_0

  # Create instance: axi_gpio_1, and set properties
  set axi_gpio_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_1 ]
  set_property -dict [ list \
   CONFIG.C_ALL_INPUTS_2 {1} \
   CONFIG.C_ALL_OUTPUTS {1} \
   CONFIG.C_IS_DUAL {1} \
 ] $axi_gpio_1

  # Create instance: axi_gpio_2, and set properties
  set axi_gpio_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_2 ]
  set_property -dict [ list \
   CONFIG.C_ALL_OUTPUTS {1} \
   CONFIG.C_ALL_OUTPUTS_2 {1} \
   CONFIG.C_GPIO2_WIDTH {1} \
   CONFIG.C_GPIO_WIDTH {1} \
   CONFIG.C_IS_DUAL {1} \
 ] $axi_gpio_2

  # Create instance: axis_clock_converter_0, and set properties
  set axis_clock_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_0 ]

  # Create instance: axis_clock_converter_1, and set properties
  set axis_clock_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_1 ]

  # Create instance: ber_test_manager_0, and set properties
  set ber_test_manager_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:ber_test_manager:1.0 ber_test_manager_0 ]

  # Create instance: clk_counter_0, and set properties
  set block_name clk_counter
  set block_cell_name clk_counter_0
  if { [catch {set clk_counter_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $clk_counter_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: jtag_axi_0, and set properties
  set jtag_axi_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:jtag_axi:1.2 jtag_axi_0 ]
  set_property -dict [ list \
   CONFIG.PROTOCOL {2} \
 ] $jtag_axi_0

  # Create instance: smartconnect_0, and set properties
  set smartconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_CLKS {1} \
   CONFIG.NUM_MI {4} \
   CONFIG.NUM_SI {1} \
 ] $smartconnect_0

  # Create instance: sync_reg_0, and set properties
  set block_name sync_reg
  set block_cell_name sync_reg_0
  if { [catch {set sync_reg_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $sync_reg_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
  
  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]
  set_property -dict [ list \
   CONFIG.NUM_PORTS {4} \
 ] $xlconcat_0

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_WIDTH {4} \
 ] $xlslice_0

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins running_out] [get_bd_intf_pins ber_test_manager_0/running_out]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins error_out] [get_bd_intf_pins ber_test_manager_0/error_out]
  connect_bd_intf_net -intf_net ack_in_1 [get_bd_intf_pins ack_in] [get_bd_intf_pins axis_clock_converter_1/S_AXIS]
  connect_bd_intf_net -intf_net axis_clock_converter_0_M_AXIS [get_bd_intf_pins cmd_out] [get_bd_intf_pins axis_clock_converter_0/M_AXIS]
  connect_bd_intf_net -intf_net axis_clock_converter_1_M_AXIS [get_bd_intf_pins axis_clock_converter_1/M_AXIS] [get_bd_intf_pins ber_test_manager_0/ack_in]
  connect_bd_intf_net -intf_net ber_test_manager_0_cmd_out [get_bd_intf_pins axis_clock_converter_0/S_AXIS] [get_bd_intf_pins ber_test_manager_0/cmd_out]
  connect_bd_intf_net -intf_net jtag_axi_0_M_AXI [get_bd_intf_pins jtag_axi_0/M_AXI] [get_bd_intf_pins smartconnect_0/S00_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M00_AXI [get_bd_intf_pins ber_test_manager_0/s_axi_regs] [get_bd_intf_pins smartconnect_0/M00_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M01_AXI [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins smartconnect_0/M01_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M02_AXI [get_bd_intf_pins axi_gpio_1/S_AXI] [get_bd_intf_pins smartconnect_0/M02_AXI]
  connect_bd_intf_net -intf_net smartconnect_0_M03_AXI [get_bd_intf_pins axi_gpio_2/S_AXI] [get_bd_intf_pins smartconnect_0/M03_AXI]

  # Create port connections
  connect_bd_net -net ap_clk_1 [get_bd_pins ap_clk] [get_bd_pins axis_clock_converter_0/m_axis_aclk] [get_bd_pins axis_clock_converter_1/s_axis_aclk] [get_bd_pins clk_counter_0/clk2] [get_bd_pins sync_reg_0/clk]
  connect_bd_net -net ap_rst_n_1 [get_bd_pins ap_rst_n] [get_bd_pins axis_clock_converter_0/m_axis_aresetn] [get_bd_pins axis_clock_converter_1/s_axis_aresetn] [get_bd_pins clk_counter_0/rstn2]
  connect_bd_net -net ap_start_only_once_0_ap_start_out [get_bd_pins ap_start_only_once_0/ap_start_out] [get_bd_pins ber_test_manager_0/ap_start]
  connect_bd_net -net axi_gpio_0_gpio_io_o [get_bd_pins axi_gpio_0/gpio_io_o] [get_bd_pins xlslice_0/Din]
  connect_bd_net -net axi_gpio_1_gpio_io_o [get_bd_pins axi_gpio_1/gpio_io_o] [get_bd_pins clk_counter_0/run]
  connect_bd_net -net axi_gpio_2_gpio2_io_o [get_bd_pins cancel_pmbus] [get_bd_pins axi_gpio_2/gpio2_io_o]
  connect_bd_net -net axi_gpio_2_gpio_io_o [get_bd_pins axi_gpio_2/gpio_io_o] [get_bd_pins sync_reg_0/data_in]
  connect_bd_net -net ber_test_manager_0_ap_done [get_bd_pins ap_start_only_once_0/ap_done_in] [get_bd_pins ber_test_manager_0/ap_done] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net ber_test_manager_0_ap_idle [get_bd_pins ber_test_manager_0/ap_idle] [get_bd_pins xlconcat_0/In2]
  connect_bd_net -net ber_test_manager_0_ap_ready [get_bd_pins ber_test_manager_0/ap_ready] [get_bd_pins xlconcat_0/In3]
  connect_bd_net -net clk_counter_0_counter_with_valid [get_bd_pins axi_gpio_1/gpio2_io_i] [get_bd_pins clk_counter_0/counter_with_valid]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins ap_clk_jtag] [get_bd_pins ap_start_only_once_0/ap_clk] [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins axi_gpio_1/s_axi_aclk] [get_bd_pins axi_gpio_2/s_axi_aclk] [get_bd_pins axis_clock_converter_0/s_axis_aclk] [get_bd_pins axis_clock_converter_1/m_axis_aclk] [get_bd_pins ber_test_manager_0/ap_clk] [get_bd_pins clk_counter_0/clk1] [get_bd_pins jtag_axi_0/aclk] [get_bd_pins smartconnect_0/aclk]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins ap_rst_n_jtag] [get_bd_pins ap_start_only_once_0/ap_rst_n] [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_gpio_1/s_axi_aresetn] [get_bd_pins axi_gpio_2/s_axi_aresetn] [get_bd_pins axis_clock_converter_0/s_axis_aresetn] [get_bd_pins axis_clock_converter_1/m_axis_aresetn] [get_bd_pins ber_test_manager_0/ap_rst_n] [get_bd_pins clk_counter_0/rstn1] [get_bd_pins jtag_axi_0/aresetn] [get_bd_pins smartconnect_0/aresetn]
  connect_bd_net -net sync_reg_0_data_out [get_bd_pins cancel_test] [get_bd_pins sync_reg_0/data_out]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins axi_gpio_0/gpio_io_i] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net xlslice_0_Dout [get_bd_pins ap_start_only_once_0/ap_start_in] [get_bd_pins xlconcat_0/In0] [get_bd_pins xlslice_0/Dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: test_application
proc create_hier_cell_test_application { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_test_application() - Empty argument(s)!"}
     return
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

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 ack_out

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 cmd_in

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 cnt_in

  create_bd_intf_pin -mode Monitor -vlnv xilinx.com:interface:axis_rtl:1.0 counter_val_in

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 data_out

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 pmbus_ack_in

  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 pmbus_cmd_out


  # Create pins
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -type clk ap_clk_pmbus
  create_bd_pin -dir I -from 0 -to 0 -type rst ap_rst_n
  create_bd_pin -dir I -type rst ap_rst_n_pmbus
  create_bd_pin -dir I cancel_test
  create_bd_pin -dir O -from 1 -to 0 i2cClockSelect
  create_bd_pin -dir I -from 31 -to 0 -type data latency_result
  create_bd_pin -dir O -from 0 -to 0 reset_modules

  # Create instance: axis_clock_converter_0, and set properties
  set axis_clock_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_0 ]

  # Create instance: axis_clock_converter_1, and set properties
  set axis_clock_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_clock_converter:1.1 axis_clock_converter_1 ]

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
  
  # Create instance: sync_reg_0, and set properties
  set block_name sync_reg
  set block_cell_name sync_reg_0
  if { [catch {set sync_reg_0 [create_bd_cell -type module -reference $block_name $block_cell_name] } errmsg] } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2095 -severity "ERROR" "Unable to add referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   } elseif { $sync_reg_0 eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2096 -severity "ERROR" "Unable to referenced block <$block_name>. Please add the files for ${block_name}'s definition into the project."}
     return 1
   }
    set_property -dict [ list \
   CONFIG.DATA_WIDTH {2} \
 ] $sync_reg_0

  # Create instance: test_app_base_0, and set properties
  set test_app_base_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:test_app_base:1.0 test_app_base_0 ]

  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0 ]
  set_property -dict [ list \
   CONFIG.C_OPERATION {not} \
   CONFIG.C_SIZE {1} \
   CONFIG.LOGO_FILE {data/sym_notgate.png} \
 ] $util_vector_logic_0

  # Create instance: util_vector_logic_1, and set properties
  set util_vector_logic_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_1 ]
  set_property -dict [ list \
   CONFIG.C_SIZE {1} \
 ] $util_vector_logic_1

  # Create instance: xlconstant_val_1, and set properties
  set xlconstant_val_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_val_1 ]

  # Create interface connections
  connect_bd_intf_net -intf_net axis_clock_converter_0_M_AXIS [get_bd_intf_pins pmbus_cmd_out] [get_bd_intf_pins axis_clock_converter_0/M_AXIS]
  connect_bd_intf_net -intf_net axis_clock_converter_1_M_AXIS [get_bd_intf_pins axis_clock_converter_1/M_AXIS] [get_bd_intf_pins test_app_base_0/pmbus_ack_in]
  connect_bd_intf_net -intf_net axis_counter_0_m_axis_cnt [get_bd_intf_pins axis_counter_0/m_axis_cnt] [get_bd_intf_pins test_app_base_0/counter_val_in]
  connect_bd_intf_net -intf_net [get_bd_intf_nets axis_counter_0_m_axis_cnt] [get_bd_intf_pins counter_val_in] [get_bd_intf_pins test_app_base_0/counter_val_in]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets axis_counter_0_m_axis_cnt]
  connect_bd_intf_net -intf_net ber_test_manager_0_cmd_out [get_bd_intf_pins cmd_in] [get_bd_intf_pins test_app_base_0/cmd_in]
  connect_bd_intf_net -intf_net cnt_in_1 [get_bd_intf_pins cnt_in] [get_bd_intf_pins test_app_base_0/cnt_in]
  connect_bd_intf_net -intf_net pmbus_m_axis_ack [get_bd_intf_pins pmbus_ack_in] [get_bd_intf_pins axis_clock_converter_1/S_AXIS]
  connect_bd_intf_net -intf_net test_app_base_0_ack_out [get_bd_intf_pins ack_out] [get_bd_intf_pins test_app_base_0/ack_out]
  connect_bd_intf_net -intf_net test_app_base_0_counter_clear_out [get_bd_intf_pins axis_counter_0/s_axis_clear] [get_bd_intf_pins test_app_base_0/counter_clear_out]
  connect_bd_intf_net -intf_net test_app_base_0_data_out [get_bd_intf_pins data_out] [get_bd_intf_pins test_app_base_0/data_out]
  connect_bd_intf_net -intf_net test_app_base_0_pmbus_cmd_out [get_bd_intf_pins axis_clock_converter_0/S_AXIS] [get_bd_intf_pins test_app_base_0/pmbus_cmd_out]

  # Create port connections
  connect_bd_net -net aclk_1 [get_bd_pins ap_clk] [get_bd_pins axis_clock_converter_0/s_axis_aclk] [get_bd_pins axis_clock_converter_1/m_axis_aclk] [get_bd_pins axis_counter_0/clock] [get_bd_pins test_app_base_0/ap_clk]
  connect_bd_net -net aresetn_1 [get_bd_pins ap_rst_n] [get_bd_pins axis_clock_converter_0/s_axis_aresetn] [get_bd_pins axis_clock_converter_1/m_axis_aresetn] [get_bd_pins axis_counter_0/resetn] [get_bd_pins test_app_base_0/ap_rst_n] [get_bd_pins util_vector_logic_1/Op1]
  connect_bd_net -net cancel_test_1 [get_bd_pins cancel_test] [get_bd_pins test_app_base_0/cancel_test]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins ap_clk_pmbus] [get_bd_pins axis_clock_converter_0/m_axis_aclk] [get_bd_pins axis_clock_converter_1/s_axis_aclk] [get_bd_pins sync_reg_0/clk]
  connect_bd_net -net latency_checker_0_latency_cycle [get_bd_pins latency_result] [get_bd_pins test_app_base_0/latency_result]
  connect_bd_net -net proc_sys_reset_0_interconnect_aresetn [get_bd_pins ap_rst_n_pmbus] [get_bd_pins axis_clock_converter_0/m_axis_aresetn] [get_bd_pins axis_clock_converter_1/s_axis_aresetn]
  connect_bd_net -net sync_reg_0_data_out [get_bd_pins i2cClockSelect] [get_bd_pins sync_reg_0/data_out]
  connect_bd_net -net test_app_base_0_i2cClockSelect [get_bd_pins sync_reg_0/data_in] [get_bd_pins test_app_base_0/i2cClockSelect]
  connect_bd_net -net test_app_base_0_reset_modules_out_TVALID [get_bd_pins test_app_base_0/reset_modules_out_TVALID] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins util_vector_logic_0/Res] [get_bd_pins util_vector_logic_1/Op2]
  connect_bd_net -net util_vector_logic_1_Res [get_bd_pins reset_modules] [get_bd_pins util_vector_logic_1/Res]
  connect_bd_net -net xlconstant_val_1_dout [get_bd_pins test_app_base_0/ap_start] [get_bd_pins test_app_base_0/reset_modules_out_TREADY] [get_bd_pins xlconstant_val_1/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: pmbus
proc create_hier_cell_pmbus { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_pmbus() - Empty argument(s)!"}
     return
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

  # Create cell and set as current instance
  set hier_obj [create_bd_cell -type hier $nameHier]
  current_bd_instance $hier_obj

  # Create interface pins
  create_bd_intf_pin -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 m_axis_ack

  create_bd_intf_pin -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 s_axis_cmd


  # Create pins
  create_bd_pin -dir IO SCL
  create_bd_pin -dir IO SDA
  create_bd_pin -dir I -type clk ap_clk
  create_bd_pin -dir I -type rst ap_rst_n
  create_bd_pin -dir I cancel_pmbus
  create_bd_pin -dir I -from 1 -to 0 i2c_clock_select

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
  
  # Create instance: power_manager_0, and set properties
  set power_manager_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:power_manager:1.0 power_manager_0 ]

  # Create instance: system_ila_1, and set properties
  set system_ila_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 system_ila_1 ]
  set_property -dict [ list \
   CONFIG.C_MON_TYPE {MIX} \
   CONFIG.C_NUM_MONITOR_SLOTS {4} \
   CONFIG.C_NUM_OF_PROBES {5} \
   CONFIG.C_SLOT_0_APC_EN {0} \
   CONFIG.C_SLOT_0_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_0_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_1_APC_EN {0} \
   CONFIG.C_SLOT_1_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_1_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_2_APC_EN {0} \
   CONFIG.C_SLOT_2_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_2_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_3_APC_EN {0} \
   CONFIG.C_SLOT_3_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_3_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 ] $system_ila_1

  # Create instance: xlconstant_val_0, and set properties
  set xlconstant_val_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_val_0 ]
  set_property -dict [ list \
   CONFIG.CONST_VAL {0} \
 ] $xlconstant_val_0

  # Create instance: xlconstant_val_1, and set properties
  set xlconstant_val_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_val_1 ]

  # Create interface connections
  connect_bd_intf_net -intf_net Conn1 [get_bd_intf_pins s_axis_cmd] [get_bd_intf_pins power_manager_0/s_axis_cmd]
  connect_bd_intf_net -intf_net [get_bd_intf_nets Conn1] [get_bd_intf_pins s_axis_cmd] [get_bd_intf_pins system_ila_1/SLOT_0_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets Conn1]
  connect_bd_intf_net -intf_net Conn2 [get_bd_intf_pins m_axis_ack] [get_bd_intf_pins power_manager_0/m_axis_ack]
  connect_bd_intf_net -intf_net [get_bd_intf_nets Conn2] [get_bd_intf_pins m_axis_ack] [get_bd_intf_pins system_ila_1/SLOT_3_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets Conn2]
  connect_bd_intf_net -intf_net axis_pmbus_wrapper_0_m_axis [get_bd_intf_pins axis_pmbus_wrapper_0/m_axis] [get_bd_intf_pins power_manager_0/s_axis_pmbus_ack]
  connect_bd_intf_net -intf_net [get_bd_intf_nets axis_pmbus_wrapper_0_m_axis] [get_bd_intf_pins axis_pmbus_wrapper_0/m_axis] [get_bd_intf_pins system_ila_1/SLOT_2_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets axis_pmbus_wrapper_0_m_axis]
  connect_bd_intf_net -intf_net power_manager_0_m_axis_pmbus_cmd [get_bd_intf_pins axis_pmbus_wrapper_0/s_axis] [get_bd_intf_pins power_manager_0/m_axis_pmbus_cmd]
  connect_bd_intf_net -intf_net [get_bd_intf_nets power_manager_0_m_axis_pmbus_cmd] [get_bd_intf_pins power_manager_0/m_axis_pmbus_cmd] [get_bd_intf_pins system_ila_1/SLOT_1_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets power_manager_0_m_axis_pmbus_cmd]

  # Create port connections
  connect_bd_net -net Net [get_bd_pins SCL] [get_bd_pins axis_pmbus_wrapper_0/SCL]
  connect_bd_net -net Net1 [get_bd_pins SDA] [get_bd_pins axis_pmbus_wrapper_0/SDA]
  connect_bd_net -net SCLdbg [get_bd_pins axis_pmbus_wrapper_0/SCLdbg] [get_bd_pins system_ila_1/probe3]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets SCLdbg]
  connect_bd_net -net SDAdbg [get_bd_pins axis_pmbus_wrapper_0/SDAdbg] [get_bd_pins system_ila_1/probe4]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets SDAdbg]
  connect_bd_net -net axis_counter_0_counter [get_bd_pins axis_counter_0/counter] [get_bd_pins power_manager_0/counter]
  connect_bd_net -net cancel_pmbus_1 [get_bd_pins cancel_pmbus] [get_bd_pins power_manager_0/cancel]
  connect_bd_net -net clk_wiz_0_clk_out1 [get_bd_pins ap_clk] [get_bd_pins axis_counter_0/clock] [get_bd_pins axis_pmbus_wrapper_0/clock] [get_bd_pins power_manager_0/ap_clk] [get_bd_pins system_ila_1/clk]
  connect_bd_net -net error [get_bd_pins power_manager_0/error] [get_bd_pins system_ila_1/probe0]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets error]
  connect_bd_net -net error_1 [get_bd_pins axis_pmbus_wrapper_0/error] [get_bd_pins system_ila_1/probe2]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets error_1]
  connect_bd_net -net i2c_clock_select_1 [get_bd_pins i2c_clock_select] [get_bd_pins axis_pmbus_wrapper_0/i2c_clock_select]
  connect_bd_net -net monitoring [get_bd_pins power_manager_0/monitoring] [get_bd_pins system_ila_1/probe1]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets monitoring]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins ap_rst_n] [get_bd_pins axis_counter_0/resetn] [get_bd_pins axis_pmbus_wrapper_0/resetn] [get_bd_pins power_manager_0/ap_rst_n] [get_bd_pins system_ila_1/resetn]
  connect_bd_net -net xlconstant_val_0_dout [get_bd_pins axis_counter_0/s_axis_clear_TVALID] [get_bd_pins xlconstant_val_0/dout]
  connect_bd_net -net xlconstant_val_1_dout [get_bd_pins axis_counter_0/m_axis_cnt_TREADY] [get_bd_pins power_manager_0/ap_start] [get_bd_pins xlconstant_val_1/dout]

  # Restore current instance
  current_bd_instance $oldCurInst
}


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
  set ctrl_in [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 ctrl_in ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {125000000} \
   CONFIG.HAS_TKEEP {0} \
   CONFIG.HAS_TLAST {0} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {1} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $ctrl_in

  set data_in [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:axis_rtl:1.0 data_in ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {125000000} \
   CONFIG.HAS_TKEEP {0} \
   CONFIG.HAS_TLAST {0} \
   CONFIG.HAS_TREADY {1} \
   CONFIG.HAS_TSTRB {0} \
   CONFIG.LAYERED_METADATA {undef} \
   CONFIG.TDATA_NUM_BYTES {8} \
   CONFIG.TDEST_WIDTH {0} \
   CONFIG.TID_WIDTH {0} \
   CONFIG.TUSER_WIDTH {0} \
   ] $data_in

  set error_out [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 error_out ]

  set running_out [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:axis_rtl:1.0 running_out ]


  # Create ports
  set SCL [ create_bd_port -dir IO SCL ]
  set SDA [ create_bd_port -dir IO SDA ]
  set aclk [ create_bd_port -dir I -type clk -freq_hz 125000000 aclk ]
  set ap_clk_jtag [ create_bd_port -dir O -type clk ap_clk_jtag ]
  set_property -dict [ list \
   CONFIG.ASSOCIATED_BUSIF {running_out:error_out} \
 ] $ap_clk_jtag
  set aresetn [ create_bd_port -dir I -type rst aresetn ]
  set latency_cycle [ create_bd_port -dir I -from 31 -to 0 latency_cycle ]
  set led_ext_clk [ create_bd_port -dir O -from 0 -to 0 -type data led_ext_clk ]
  set led_int_clk [ create_bd_port -dir O -from 0 -to 0 led_int_clk ]
  set reset_latency [ create_bd_port -dir O -from 0 -to 0 reset_latency ]
  set sysclk [ create_bd_port -dir I -type clk -freq_hz 200000000 sysclk ]
  set_property -dict [ list \
   CONFIG.CLK_DOMAIN {design_1_sysclk1} \
 ] $sysclk
  set sysrstn [ create_bd_port -dir I -type rst sysrstn ]

  # Create instance: c_counter_binary_0, and set properties
  set c_counter_binary_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_counter_binary:12.0 c_counter_binary_0 ]
  set_property -dict [ list \
   CONFIG.Output_Width {26} \
 ] $c_counter_binary_0

  # Create instance: c_counter_binary_1, and set properties
  set c_counter_binary_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:c_counter_binary:12.0 c_counter_binary_1 ]
  set_property -dict [ list \
   CONFIG.Output_Width {26} \
 ] $c_counter_binary_1

  # Create instance: clk_wiz_0, and set properties
  set clk_wiz_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_0 ]
  set_property -dict [ list \
   CONFIG.PRIM_SOURCE {No_buffer} \
   CONFIG.RESET_PORT {resetn} \
   CONFIG.RESET_TYPE {ACTIVE_LOW} \
 ] $clk_wiz_0

  # Create instance: data_aligner_0, and set properties
  set data_aligner_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:data_aligner:1.0 data_aligner_0 ]

  # Create instance: data_checker_0, and set properties
  set data_checker_0 [ create_bd_cell -type ip -vlnv Fixstars:hls:data_checker:1.0 data_checker_0 ]

  # Create instance: pmbus
  create_hier_cell_pmbus [current_bd_instance .] pmbus

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]

  # Create instance: system_ila_0, and set properties
  set system_ila_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:system_ila:1.1 system_ila_0 ]
  set_property -dict [ list \
   CONFIG.C_MON_TYPE {MIX} \
   CONFIG.C_NUM_MONITOR_SLOTS {8} \
   CONFIG.C_NUM_OF_PROBES {2} \
   CONFIG.C_PROBE0_TYPE {0} \
   CONFIG.C_PROBE1_TYPE {0} \
   CONFIG.C_PROBE2_TYPE {0} \
   CONFIG.C_PROBE3_TYPE {0} \
   CONFIG.C_PROBE4_TYPE {0} \
   CONFIG.C_PROBE5_TYPE {0} \
   CONFIG.C_PROBE6_TYPE {0} \
   CONFIG.C_PROBE7_TYPE {0} \
   CONFIG.C_SLOT_0_APC_EN {0} \
   CONFIG.C_SLOT_0_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_0_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_0_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_1_APC_EN {0} \
   CONFIG.C_SLOT_1_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_1_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_1_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_2_APC_EN {0} \
   CONFIG.C_SLOT_2_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_2_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_2_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
   CONFIG.C_SLOT_3_APC_EN {0} \
   CONFIG.C_SLOT_3_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_3_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_3_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
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
   CONFIG.C_SLOT_7_APC_EN {0} \
   CONFIG.C_SLOT_7_AXI_DATA_SEL {1} \
   CONFIG.C_SLOT_7_AXI_TRIG_SEL {1} \
   CONFIG.C_SLOT_7_INTF_TYPE {xilinx.com:interface:axis_rtl:1.0} \
 ] $system_ila_0

  # Create instance: test_application
  create_hier_cell_test_application [current_bd_instance .] test_application

  # Create instance: test_manager
  create_hier_cell_test_manager [current_bd_instance .] test_manager

  # Create instance: xlconstant_val_1, and set properties
  set xlconstant_val_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_val_1 ]

  # Create instance: xlslice_0, and set properties
  set xlslice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_0 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {25} \
   CONFIG.DIN_TO {25} \
   CONFIG.DIN_WIDTH {26} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_0

  # Create instance: xlslice_1, and set properties
  set xlslice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_1 ]
  set_property -dict [ list \
   CONFIG.DIN_FROM {25} \
   CONFIG.DIN_TO {25} \
   CONFIG.DIN_WIDTH {26} \
   CONFIG.DOUT_WIDTH {1} \
 ] $xlslice_1

  # Create interface connections
connect_bd_intf_net -intf_net Conn [get_bd_intf_pins system_ila_0/SLOT_7_AXIS] [get_bd_intf_pins test_application/counter_val_in]
  connect_bd_intf_net -intf_net axis_clock_converter_0_M_AXIS [get_bd_intf_pins pmbus/s_axis_cmd] [get_bd_intf_pins test_application/pmbus_cmd_out]
  connect_bd_intf_net -intf_net ber_test_manager_0_cmd_out [get_bd_intf_pins test_application/cmd_in] [get_bd_intf_pins test_manager/cmd_out]
connect_bd_intf_net -intf_net [get_bd_intf_nets ber_test_manager_0_cmd_out] [get_bd_intf_pins system_ila_0/SLOT_4_AXIS] [get_bd_intf_pins test_application/cmd_in]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets ber_test_manager_0_cmd_out]
  connect_bd_intf_net -intf_net ctrl_in_0_1 [get_bd_intf_ports ctrl_in] [get_bd_intf_pins data_aligner_0/ctrl_in]
connect_bd_intf_net -intf_net [get_bd_intf_nets ctrl_in_0_1] [get_bd_intf_ports ctrl_in] [get_bd_intf_pins system_ila_0/SLOT_1_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets ctrl_in_0_1]
  connect_bd_intf_net -intf_net data_aligner_0_aligned_data_out [get_bd_intf_pins data_aligner_0/aligned_data_out] [get_bd_intf_pins data_checker_0/data_in]
connect_bd_intf_net -intf_net [get_bd_intf_nets data_aligner_0_aligned_data_out] [get_bd_intf_pins data_aligner_0/aligned_data_out] [get_bd_intf_pins system_ila_0/SLOT_2_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets data_aligner_0_aligned_data_out]
  connect_bd_intf_net -intf_net data_checker_0_cnt_out [get_bd_intf_pins data_checker_0/cnt_out] [get_bd_intf_pins test_application/cnt_in]
connect_bd_intf_net -intf_net [get_bd_intf_nets data_checker_0_cnt_out] [get_bd_intf_pins system_ila_0/SLOT_6_AXIS] [get_bd_intf_pins test_application/cnt_in]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets data_checker_0_cnt_out]
  connect_bd_intf_net -intf_net data_in_0_1 [get_bd_intf_ports data_in] [get_bd_intf_pins data_aligner_0/data_in]
connect_bd_intf_net -intf_net [get_bd_intf_nets data_in_0_1] [get_bd_intf_ports data_in] [get_bd_intf_pins system_ila_0/SLOT_0_AXIS]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets data_in_0_1]
  connect_bd_intf_net -intf_net pmbus_m_axis_ack [get_bd_intf_pins pmbus/m_axis_ack] [get_bd_intf_pins test_application/pmbus_ack_in]
  connect_bd_intf_net -intf_net test_app_base_0_ack_out [get_bd_intf_pins test_application/ack_out] [get_bd_intf_pins test_manager/ack_in]
connect_bd_intf_net -intf_net [get_bd_intf_nets test_app_base_0_ack_out] [get_bd_intf_pins system_ila_0/SLOT_5_AXIS] [get_bd_intf_pins test_manager/ack_in]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets test_app_base_0_ack_out]
  connect_bd_intf_net -intf_net test_app_base_0_data_out [get_bd_intf_pins data_checker_0/exp_in] [get_bd_intf_pins test_application/data_out]
connect_bd_intf_net -intf_net [get_bd_intf_nets test_app_base_0_data_out] [get_bd_intf_pins system_ila_0/SLOT_3_AXIS] [get_bd_intf_pins test_application/data_out]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_intf_nets test_app_base_0_data_out]
  connect_bd_intf_net -intf_net test_manager_error_out_0 [get_bd_intf_ports error_out] [get_bd_intf_pins test_manager/error_out]
  connect_bd_intf_net -intf_net test_manager_running_out_0 [get_bd_intf_ports running_out] [get_bd_intf_pins test_manager/running_out]

  # Create port connections
  connect_bd_net -net Net [get_bd_ports SCL] [get_bd_pins pmbus/SCL]
  connect_bd_net -net Net1 [get_bd_ports SDA] [get_bd_pins pmbus/SDA]
  connect_bd_net -net aclk_1 [get_bd_ports aclk] [get_bd_pins c_counter_binary_1/CLK] [get_bd_pins data_aligner_0/ap_clk] [get_bd_pins data_checker_0/ap_clk] [get_bd_pins system_ila_0/clk] [get_bd_pins test_application/ap_clk] [get_bd_pins test_manager/ap_clk]
  connect_bd_net -net aresetn_1 [get_bd_ports aresetn] [get_bd_pins system_ila_0/resetn] [get_bd_pins test_application/ap_rst_n] [get_bd_pins test_manager/ap_rst_n]
  connect_bd_net -net c_counter_binary_0_Q [get_bd_pins c_counter_binary_0/Q] [get_bd_pins xlslice_0/Din]
  connect_bd_net -net c_counter_binary_1_Q [get_bd_pins c_counter_binary_1/Q] [get_bd_pins xlslice_1/Din]
  connect_bd_net -net clk_wiz_0_clk_out2 [get_bd_ports ap_clk_jtag] [get_bd_pins c_counter_binary_0/CLK] [get_bd_pins clk_wiz_0/clk_out1] [get_bd_pins pmbus/ap_clk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] [get_bd_pins test_application/ap_clk_pmbus] [get_bd_pins test_manager/ap_clk_jtag]
  connect_bd_net -net clk_wiz_0_locked [get_bd_pins clk_wiz_0/locked] [get_bd_pins proc_sys_reset_0/dcm_locked]
  connect_bd_net -net latency_checker_0_latency_cycle [get_bd_ports latency_cycle] [get_bd_pins system_ila_0/probe0] [get_bd_pins test_application/latency_result]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets latency_checker_0_latency_cycle]
  connect_bd_net -net proc_sys_reset_0_interconnect_aresetn [get_bd_pins proc_sys_reset_0/interconnect_aresetn] [get_bd_pins test_application/ap_rst_n_pmbus]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins pmbus/ap_rst_n] [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins test_manager/ap_rst_n_jtag]
  connect_bd_net -net sync_reg_0_data_out [get_bd_pins pmbus/i2c_clock_select] [get_bd_pins test_application/i2cClockSelect]
  connect_bd_net -net sysclk_1 [get_bd_ports sysclk] [get_bd_pins clk_wiz_0/clk_in1]
  connect_bd_net -net sysrstn_1 [get_bd_ports sysrstn] [get_bd_pins clk_wiz_0/resetn] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net test_manager_cancel_pmbus [get_bd_pins pmbus/cancel_pmbus] [get_bd_pins test_manager/cancel_pmbus]
  connect_bd_net -net test_manager_cancel_test [get_bd_pins data_checker_0/cancel_test] [get_bd_pins test_application/cancel_test] [get_bd_pins test_manager/cancel_test]
  connect_bd_net -net util_vector_logic_1_Res [get_bd_ports reset_latency] [get_bd_pins data_aligner_0/ap_rst_n] [get_bd_pins data_checker_0/ap_rst_n] [get_bd_pins system_ila_0/probe1] [get_bd_pins test_application/reset_modules]
  set_property HDL_ATTRIBUTE.DEBUG {true} [get_bd_nets util_vector_logic_1_Res]
  connect_bd_net -net xlconstant_val_1_dout [get_bd_pins data_aligner_0/ap_start] [get_bd_pins data_checker_0/ap_start] [get_bd_pins xlconstant_val_1/dout]
  connect_bd_net -net xlslice_0_Dout [get_bd_ports led_int_clk] [get_bd_pins xlslice_0/Dout]
  connect_bd_net -net xlslice_1_Dout [get_bd_ports led_ext_clk] [get_bd_pins xlslice_1/Dout]

  # Create address segments
  assign_bd_address -offset 0x00010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces test_manager/jtag_axi_0/Data] [get_bd_addr_segs test_manager/axi_gpio_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x00020000 -range 0x00010000 -target_address_space [get_bd_addr_spaces test_manager/jtag_axi_0/Data] [get_bd_addr_segs test_manager/axi_gpio_1/S_AXI/Reg] -force
  assign_bd_address -offset 0x00030000 -range 0x00010000 -target_address_space [get_bd_addr_spaces test_manager/jtag_axi_0/Data] [get_bd_addr_segs test_manager/axi_gpio_2/S_AXI/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces test_manager/jtag_axi_0/Data] [get_bd_addr_segs test_manager/ber_test_manager_0/s_axi_regs/Reg] -force


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


