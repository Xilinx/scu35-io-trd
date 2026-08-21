# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

# Add package: Vitis Python CLI
import vitis

# Create a Vitis client object
client = vitis.create_client()

# Set Vitis Workspace
client.set_workspace(path="./workspace")

# Create Platform
plat_name = "io_plat"
xsa_file = "../hw/project/scu35_io_design.xsa" 
platform_obj=client.create_platform_component(name=plat_name, hw_design=xsa_file, cpu="microblaze_riscv_0", os="standalone")
platform_obj.build()
platform_xpfm=client.find_platform_in_repos(plat_name)

# Create Application
app_name = "io_app"
app_comp = client.create_app_component(name = app_name, platform = platform_xpfm)
sources_dir = "./src/"
app_comp.import_files(from_loc = sources_dir, dest_dir_in_cmp="src")
app_comp.build()