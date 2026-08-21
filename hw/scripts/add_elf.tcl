# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

set jobs 8

set proj_name scu35_io_design
set proj_board [get_board_parts "*:scu35:*" -latest_file_version]
puts "Board Part: $proj_board"

open_project ./project/${proj_name}.xpr
update_compile_order -fileset sources_1

add_files -norecurse ../sw/workspace/io_app/build/io_app.elf

set_property SCOPED_TO_REF scu35_io_design [get_files -all -of_objects [get_fileset sources_1] {../sw/workspace/io_app/build/io_app.elf}]
set_property SCOPED_TO_CELLS { microblaze_riscv_0 } [get_files -all -of_objects [get_fileset sources_1] {../sw/workspace/io_app/build/io_app.elf}]

launch_runs impl_1 -to_step write_bitstream
            
wait_on_run impl_1