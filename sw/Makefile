# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

RM = rm -rf

VITIS = $(XILINX_VITIS)/bin/vitis

VIT_WS = workspace
VIT_ELF_DIR = io_app/build
VIT_ELF = $(VIT_WS)/$(VIT_ELF_DIR)/io_app.elf

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make elf'
	@echo '    Generate Vitis software application.'
	@echo ''
	@echo '  make clean'
	@echo '    Clean runs'
	@echo ''

.PHONY: all
all: elf

.PHONY: elf
elf: $(VIT_ELF)
$(VIT_ELF): 
	$(VITIS) -s ./create_sw_design.py

.PHONY: clean
clean:
	$(RM) $(VIT_WS)