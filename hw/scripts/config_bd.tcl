# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

##################################################################
# DESIGN PROCs
##################################################################


# Hierarchical cell: fmc_multicam_control
proc create_hier_cell_microblaze_riscv_0_local_memory { parentCell nameHier } {

  variable script_folder

  if { $parentCell eq "" || $nameHier eq "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2092 -severity "ERROR" "create_hier_cell_microblaze_riscv_0_local_memory() - Empty argument(s)!"}
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
  create_bd_intf_pin -mode MirroredMaster -vlnv xilinx.com:interface:lmb_rtl:1.0 DLMB

  create_bd_intf_pin -mode MirroredMaster -vlnv xilinx.com:interface:lmb_rtl:1.0 ILMB


  # Create pins
  create_bd_pin -dir I -type clk LMB_Clk
  create_bd_pin -dir I -type rst SYS_Rst

  # Create instance: dlmb_v10, and set properties
  set dlmb_v10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_v10:3.0 dlmb_v10 ]

  # Create instance: ilmb_v10, and set properties
  set ilmb_v10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_v10:3.0 ilmb_v10 ]

  # Create instance: dlmb_bram_if_cntlr, and set properties
  set dlmb_bram_if_cntlr [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_bram_if_cntlr:4.0 dlmb_bram_if_cntlr ]
  set_property CONFIG.C_ECC {0} $dlmb_bram_if_cntlr


  # Create instance: ilmb_bram_if_cntlr, and set properties
  set ilmb_bram_if_cntlr [ create_bd_cell -type ip -vlnv xilinx.com:ip:lmb_bram_if_cntlr:4.0 ilmb_bram_if_cntlr ]
  set_property CONFIG.C_ECC {0} $ilmb_bram_if_cntlr


  # Create instance: lmb_bram, and set properties
  set lmb_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 lmb_bram ]
  set_property -dict [list \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.use_bram_block {BRAM_Controller} \
  ] $lmb_bram


  # Create interface connections
  connect_bd_intf_net -intf_net microblaze_riscv_0_dlmb [get_bd_intf_pins dlmb_v10/LMB_M] [get_bd_intf_pins DLMB]
  connect_bd_intf_net -intf_net microblaze_riscv_0_dlmb_bus [get_bd_intf_pins dlmb_v10/LMB_Sl_0] [get_bd_intf_pins dlmb_bram_if_cntlr/SLMB]
  connect_bd_intf_net -intf_net microblaze_riscv_0_dlmb_cntlr [get_bd_intf_pins dlmb_bram_if_cntlr/BRAM_PORT] [get_bd_intf_pins lmb_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net microblaze_riscv_0_ilmb [get_bd_intf_pins ilmb_v10/LMB_M] [get_bd_intf_pins ILMB]
  connect_bd_intf_net -intf_net microblaze_riscv_0_ilmb_bus [get_bd_intf_pins ilmb_v10/LMB_Sl_0] [get_bd_intf_pins ilmb_bram_if_cntlr/SLMB]
  connect_bd_intf_net -intf_net microblaze_riscv_0_ilmb_cntlr [get_bd_intf_pins ilmb_bram_if_cntlr/BRAM_PORT] [get_bd_intf_pins lmb_bram/BRAM_PORTB]

  # Create port connections
  connect_bd_net -net SYS_Rst_1  [get_bd_pins SYS_Rst] \
  [get_bd_pins dlmb_v10/SYS_Rst] \
  [get_bd_pins dlmb_bram_if_cntlr/LMB_Rst] \
  [get_bd_pins ilmb_v10/SYS_Rst] \
  [get_bd_pins ilmb_bram_if_cntlr/LMB_Rst]
  connect_bd_net -net microblaze_riscv_0_Clk  [get_bd_pins LMB_Clk] \
  [get_bd_pins dlmb_v10/LMB_Clk] \
  [get_bd_pins dlmb_bram_if_cntlr/LMB_Clk] \
  [get_bd_pins ilmb_v10/LMB_Clk] \
  [get_bd_pins ilmb_bram_if_cntlr/LMB_Clk]

  # Restore current instance
  current_bd_instance $oldCurInst
}

# Hierarchical cell: pipe_3# Procedure to create entire design; Provide argument to make
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
  set sys_diff_clock [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 sys_diff_clock ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {100000000} \
   ] $sys_diff_clock

  set push_buttons [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 push_buttons ]

  set DIP [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 DIP ]

  set iic_acl_main [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 iic_acl_main ]

  set scu35_uartb [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:uart_rtl:1.0 scu35_uartb ]

  set iic_eeprom_main [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 iic_eeprom_main ]

  set iic_ina_main [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:iic_rtl:1.0 iic_ina_main ]

  set LEDs [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 LEDs ]

  set PMOD_i [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 PMOD_i ]


  # Create ports
  set reset [ create_bd_port -dir I -type rst reset ]
  set_property -dict [ list \
   CONFIG.POLARITY {ACTIVE_LOW} \
 ] $reset

  # Create instance: microblaze_riscv_0, and set properties
  set microblaze_riscv_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:microblaze_riscv:1.0 microblaze_riscv_0 ]
  set_property -dict [list \
    CONFIG.C_DEBUG_ENABLED {1} \
    CONFIG.C_D_AXI {1} \
    CONFIG.C_D_LMB {1} \
    CONFIG.C_I_LMB {1} \
    CONFIG.G_TEMPLATE_LIST {1} \
  ] $microblaze_riscv_0


  # Create instance: microblaze_riscv_0_local_memory
  create_hier_cell_microblaze_riscv_0_local_memory [current_bd_instance .] microblaze_riscv_0_local_memory

  # Create instance: microblaze_riscv_0_axi_periph, and set properties
  set microblaze_riscv_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 microblaze_riscv_0_axi_periph ]
  set_property -dict [list \
    CONFIG.NUM_MI {9} \
    CONFIG.NUM_SI {1} \
  ] $microblaze_riscv_0_axi_periph


  # Create instance: microblaze_riscv_0_axi_intc, and set properties
  set microblaze_riscv_0_axi_intc [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_intc:4.1 microblaze_riscv_0_axi_intc ]
  set_property CONFIG.C_HAS_FAST {1} $microblaze_riscv_0_axi_intc


  # Create instance: microblaze_riscv_0_xlconcat, and set properties
  set microblaze_riscv_0_concat [ create_bd_cell -type inline_hdl -vlnv xilinx.com:inline_hdl:ilconcat:1.0 microblaze_riscv_0_concat ]

  # Create instance: mdm_1, and set properties
  set mdm_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:mdm_riscv:1.0 mdm_1 ]

  # Create instance: clk_wiz_1, and set properties
  set clk_wiz_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:clk_wiz:6.0 clk_wiz_1 ]
  set_property -dict [list \
    CONFIG.CLKOUT1_JITTER {102.086} \
    CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200.000} \
    CONFIG.CLK_IN1_BOARD_INTERFACE {sys_diff_clock} \
    CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} \
    CONFIG.PRIM_SOURCE {Differential_clock_capable_pin} \
    CONFIG.RESET_BOARD_INTERFACE {reset} \
    CONFIG.RESET_PORT {resetn} \
    CONFIG.RESET_TYPE {ACTIVE_LOW} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $clk_wiz_1


  # Create instance: rst_clk_wiz_1_200M, and set properties
  set rst_clk_wiz_1_200M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_clk_wiz_1_200M ]
  set_property -dict [list \
    CONFIG.RESET_BOARD_INTERFACE {reset} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $rst_clk_wiz_1_200M


  # Create instance: gpio_pushB_DIP, and set properties
  set gpio_pushB_DIP [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 gpio_pushB_DIP ]
  set_property -dict [list \
    CONFIG.C_GPIO2_WIDTH {8} \
    CONFIG.C_IS_DUAL {1} \
    CONFIG.GPIO_BOARD_INTERFACE {push_buttons} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $gpio_pushB_DIP


  # Create instance: iic_acl, and set properties
  set iic_acl [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_iic:2.1 iic_acl ]
  set_property -dict [list \
    CONFIG.IIC_BOARD_INTERFACE {iic_acl_main} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $iic_acl


  # Create instance: axi_uartlite_0, and set properties
  set axi_uartlite_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_uartlite:2.0 axi_uartlite_0 ]
  set_property -dict [list \
    CONFIG.C_BAUDRATE {230400} \
    CONFIG.UARTLITE_BOARD_INTERFACE {scu35_uartb} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $axi_uartlite_0


  # Create instance: axi_timer_0, and set properties
  set axi_timer_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_0 ]

  # Create instance: iic_eeprom, and set properties
  set iic_eeprom [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_iic:2.1 iic_eeprom ]
  set_property -dict [list \
    CONFIG.IIC_BOARD_INTERFACE {iic_eeprom_main} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $iic_eeprom


  # Create instance: iic_ina, and set properties
  set iic_ina [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_iic:2.1 iic_ina ]
  set_property -dict [list \
    CONFIG.IIC_BOARD_INTERFACE {iic_ina_main} \
    CONFIG.USE_BOARD_FLOW {true} \
  ] $iic_ina


  # Create instance: gpio_LEDs, and set properties
  set gpio_LEDs [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 gpio_LEDs ]
  set_property CONFIG.C_GPIO_WIDTH {15} $gpio_LEDs


  # Create instance: gpio_PMOD_i, and set properties
  set gpio_PMOD_i [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 gpio_PMOD_i ]
  set_property CONFIG.C_GPIO_WIDTH {32} $gpio_PMOD_i


  # Create interface connections
  connect_bd_intf_net -intf_net axi_gpio_0_GPIO [get_bd_intf_ports push_buttons] [get_bd_intf_pins gpio_pushB_DIP/GPIO]
  connect_bd_intf_net -intf_net axi_gpio_0_GPIO2 [get_bd_intf_ports DIP] [get_bd_intf_pins gpio_pushB_DIP/GPIO2]
  connect_bd_intf_net -intf_net axi_iic_0_IIC [get_bd_intf_ports iic_acl_main] [get_bd_intf_pins iic_acl/IIC]
  connect_bd_intf_net -intf_net axi_iic_0_IIC1 [get_bd_intf_ports iic_eeprom_main] [get_bd_intf_pins iic_eeprom/IIC]
  connect_bd_intf_net -intf_net axi_iic_0_IIC2 [get_bd_intf_ports iic_ina_main] [get_bd_intf_pins iic_ina/IIC]
  connect_bd_intf_net -intf_net axi_uartlite_0_UART [get_bd_intf_ports scu35_uartb] [get_bd_intf_pins axi_uartlite_0/UART]
  connect_bd_intf_net -intf_net gpio_LEDs_GPIO [get_bd_intf_ports LEDs] [get_bd_intf_pins gpio_LEDs/GPIO]
  connect_bd_intf_net -intf_net gpio_PMOD_i_GPIO [get_bd_intf_ports PMOD_i] [get_bd_intf_pins gpio_PMOD_i/GPIO]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_dp [get_bd_intf_pins microblaze_riscv_0_axi_periph/S00_AXI] [get_bd_intf_pins microblaze_riscv_0/M_AXI_DP]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M01_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M01_AXI] [get_bd_intf_pins axi_timer_0/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M02_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M02_AXI] [get_bd_intf_pins axi_uartlite_0/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M03_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M03_AXI] [get_bd_intf_pins gpio_LEDs/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M04_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M04_AXI] [get_bd_intf_pins gpio_pushB_DIP/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M05_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M05_AXI] [get_bd_intf_pins iic_acl/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M06_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M06_AXI] [get_bd_intf_pins iic_eeprom/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M07_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M07_AXI] [get_bd_intf_pins iic_ina/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_axi_periph_M08_AXI [get_bd_intf_pins microblaze_riscv_0_axi_periph/M08_AXI] [get_bd_intf_pins gpio_PMOD_i/S_AXI]
  connect_bd_intf_net -intf_net microblaze_riscv_0_debug [get_bd_intf_pins mdm_1/MBDEBUG_0] [get_bd_intf_pins microblaze_riscv_0/DEBUG]
  connect_bd_intf_net -intf_net microblaze_riscv_0_dlmb_1 [get_bd_intf_pins microblaze_riscv_0/DLMB] [get_bd_intf_pins microblaze_riscv_0_local_memory/DLMB]
  connect_bd_intf_net -intf_net microblaze_riscv_0_ilmb_1 [get_bd_intf_pins microblaze_riscv_0/ILMB] [get_bd_intf_pins microblaze_riscv_0_local_memory/ILMB]
  connect_bd_intf_net -intf_net microblaze_riscv_0_intc_axi [get_bd_intf_pins microblaze_riscv_0_axi_periph/M00_AXI] [get_bd_intf_pins microblaze_riscv_0_axi_intc/s_axi]
  connect_bd_intf_net -intf_net microblaze_riscv_0_interrupt [get_bd_intf_pins microblaze_riscv_0_axi_intc/interrupt] [get_bd_intf_pins microblaze_riscv_0/INTERRUPT]
  connect_bd_intf_net -intf_net sys_diff_clock_1 [get_bd_intf_ports sys_diff_clock] [get_bd_intf_pins clk_wiz_1/CLK_IN1_D]

  # Create port connections
  connect_bd_net -net axi_timer_0_interrupt  [get_bd_pins axi_timer_0/interrupt] \
  [get_bd_pins microblaze_riscv_0_concat/In1]
  connect_bd_net -net axi_uartlite_0_interrupt  [get_bd_pins axi_uartlite_0/interrupt] \
  [get_bd_pins microblaze_riscv_0_concat/In0]
  connect_bd_net -net clk_wiz_1_locked  [get_bd_pins clk_wiz_1/locked] \
  [get_bd_pins rst_clk_wiz_1_200M/dcm_locked]
  connect_bd_net -net mdm_1_debug_sys_rst  [get_bd_pins mdm_1/Debug_SYS_Rst] \
  [get_bd_pins rst_clk_wiz_1_200M/mb_debug_sys_rst]
  connect_bd_net -net microblaze_riscv_0_Clk  [get_bd_pins clk_wiz_1/clk_out1] \
  [get_bd_pins microblaze_riscv_0/Clk] \
  [get_bd_pins microblaze_riscv_0_axi_periph/aclk] \
  [get_bd_pins microblaze_riscv_0_axi_intc/s_axi_aclk] \
  [get_bd_pins microblaze_riscv_0_axi_intc/processor_clk] \
  [get_bd_pins microblaze_riscv_0_local_memory/LMB_Clk] \
  [get_bd_pins rst_clk_wiz_1_200M/slowest_sync_clk] \
  [get_bd_pins axi_timer_0/s_axi_aclk] \
  [get_bd_pins axi_uartlite_0/s_axi_aclk] \
  [get_bd_pins gpio_pushB_DIP/s_axi_aclk] \
  [get_bd_pins iic_acl/s_axi_aclk] \
  [get_bd_pins iic_eeprom/s_axi_aclk] \
  [get_bd_pins iic_ina/s_axi_aclk] \
  [get_bd_pins gpio_LEDs/s_axi_aclk] \
  [get_bd_pins gpio_PMOD_i/s_axi_aclk]
  connect_bd_net -net microblaze_riscv_0_intr  [get_bd_pins microblaze_riscv_0_concat/dout] \
  [get_bd_pins microblaze_riscv_0_axi_intc/intr]
  connect_bd_net -net reset_1  [get_bd_ports reset] \
  [get_bd_pins clk_wiz_1/resetn] \
  [get_bd_pins rst_clk_wiz_1_200M/ext_reset_in]
  connect_bd_net -net rst_clk_wiz_1_100M_bus_struct_reset  [get_bd_pins rst_clk_wiz_1_200M/bus_struct_reset] \
  [get_bd_pins microblaze_riscv_0_local_memory/SYS_Rst]
  connect_bd_net -net rst_clk_wiz_1_100M_mb_reset  [get_bd_pins rst_clk_wiz_1_200M/mb_reset] \
  [get_bd_pins microblaze_riscv_0/Reset] \
  [get_bd_pins microblaze_riscv_0_axi_intc/processor_rst]
  connect_bd_net -net rst_clk_wiz_1_100M_peripheral_aresetn  [get_bd_pins rst_clk_wiz_1_200M/peripheral_aresetn] \
  [get_bd_pins microblaze_riscv_0_axi_periph/aresetn] \
  [get_bd_pins microblaze_riscv_0_axi_intc/s_axi_aresetn] \
  [get_bd_pins axi_timer_0/s_axi_aresetn] \
  [get_bd_pins axi_uartlite_0/s_axi_aresetn] \
  [get_bd_pins gpio_pushB_DIP/s_axi_aresetn] \
  [get_bd_pins iic_acl/s_axi_aresetn] \
  [get_bd_pins iic_eeprom/s_axi_aresetn] \
  [get_bd_pins iic_ina/s_axi_aresetn] \
  [get_bd_pins gpio_LEDs/s_axi_aresetn] \
  [get_bd_pins gpio_PMOD_i/s_axi_aresetn]

  # Create address segments
  assign_bd_address -offset 0x40020000 -range 0x00010000 -with_name SEG_axi_gpio_0_Reg -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs gpio_PMOD_i/S_AXI/Reg] -force
  assign_bd_address -offset 0x41C00000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs axi_timer_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x40600000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs axi_uartlite_0/S_AXI/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00020000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs microblaze_riscv_0_local_memory/dlmb_bram_if_cntlr/SLMB/Mem] -force
  assign_bd_address -offset 0x40000000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs gpio_LEDs/S_AXI/Reg] -force
  assign_bd_address -offset 0x40010000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs gpio_pushB_DIP/S_AXI/Reg] -force
  assign_bd_address -offset 0x40800000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs iic_acl/S_AXI/Reg] -force
  assign_bd_address -offset 0x40810000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs iic_eeprom/S_AXI/Reg] -force
  assign_bd_address -offset 0x40820000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs iic_ina/S_AXI/Reg] -force
  assign_bd_address -offset 0x41200000 -range 0x00010000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Data] [get_bd_addr_segs microblaze_riscv_0_axi_intc/S_AXI/Reg] -force
  assign_bd_address -offset 0x00000000 -range 0x00020000 -target_address_space [get_bd_addr_spaces microblaze_riscv_0/Instruction] [get_bd_addr_segs microblaze_riscv_0_local_memory/ilmb_bram_if_cntlr/SLMB/Mem] -force


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

