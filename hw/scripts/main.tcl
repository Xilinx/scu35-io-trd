# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

set jobs 8

# parse arguments
for { set i 0 } { $i < $argc } { incr i } {
  # jobs
  if { [lindex $argv $i] == "-jobs" } {
    incr i
    set jobs [lindex $argv $i]
  }
}

set proj_name scu35_io_design
set proj_board [get_board_parts "*:scu35:*" -latest_file_version]
puts "Board Part: $proj_board"
create_project -name ${proj_name} -force -dir ./project -part [get_property PART_NAME [get_board_parts $proj_board]] 
set_property board_part $proj_board [current_project]


set proj_dir ./project
set bd_tcl_dir ./scripts
set device scu35p
set output {zip xsa bit}
set xdc_list {xdc/io_design.xdc}      
    
import_files -fileset constrs_1 $xdc_list

update_ip_catalog
    
# Create block diagram design and set as current design
set design_name $proj_name
create_bd_design $proj_name
current_bd_design $proj_name

# Set current bd instance as root of current design
set parentCell [get_bd_cells /]
set parentObj [get_bd_cells $parentCell]
current_bd_instance $parentObj
        
source $bd_tcl_dir/config_bd.tcl
save_bd_design
    

make_wrapper -files [get_files $proj_dir/${proj_name}.srcs/sources_1/bd/$proj_name/${proj_name}.bd] -top
import_files -force -norecurse $proj_dir/${proj_name}.srcs/sources_1/bd/$proj_name/hdl/${proj_name}_wrapper.v
update_compile_order
set_property top ${proj_name}_wrapper [current_fileset]
update_compile_order -fileset sources_1

save_bd_design
validate_bd_design

#Set Platform properties
set_property platform.board_id $proj_name [current_project]     
set_property platform.name $proj_name [current_project]
set_property platform.vendor "AMD" [current_project]
set_property platform.version "1.0" [current_project]


# Generate IPs and Implement design
generate_target all [get_files $proj_dir/${proj_name}.srcs/sources_1/bd/$proj_name/${proj_name}.bd]        

set_property synth_checkpoint_mode Hierarchical [get_files $proj_dir/${proj_name}.srcs/sources_1/bd/$proj_name/${proj_name}.bd]

launch_runs synth_1 -jobs ${jobs}
wait_on_run synth_1

#launch_runs impl_1 -to_step write_bitstream
            
#wait_on_run impl_1

# Generate XSA
#write_hw_platform -fixed -include_bit -hw -force -file $proj_dir/${proj_name}.xsa

write_hw_platform -fixed -hw -force -file $proj_dir/${proj_name}.xsa
validate_hw_platform -verbose $proj_dir/${proj_name}.xsa

exit