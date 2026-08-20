# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

HW_PRJ_DIR = hw
VIV_DESIGN = scu35_io_design
VIV_PRJ_DIR = project
VIV_XSA = $(HW_PRJ_DIR)/$(VIV_PRJ_DIR)/$(VIV_DESIGN).xsa
VIV_PDI = $(HW_PRJ_DIR)/$(VIV_PRJ_DIR)/$(VIV_DESIGN)_wrapper.pdi

SW_PRJ_DIR = sw
VIT_WS = workspace
VIT_ELF_DIR = io_app/build
VIT_ELF = $(SW_PRJ_DIR)/$(VIT_WS)/$(VIT_ELF_DIR)/io_app.elf


.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make xsa'
	@echo '    Generate extensible xsa platform'
	@echo ''
	@echo '  make elf'
	@echo '    Generate Vitis software application.'
	@echo ''
	@echo '  make pdi'
	@echo '    Generate device image with embedded software'
	@echo ''
	@echo '  make clean'
	@echo '    Clean runs'
	@echo ''

.PHONY: all
all: pdi

.PHONY: pdi
pdi: $(VIV_PDI)
$(VIV_PDI): $(VIV_XSA) $(VIT_ELF)
	$(MAKE) -C $(HW_PRJ_DIR) pdi

.PHONY: elf
elf: $(VIT_ELF)
$(VIT_ELF): $(VIV_XSA)
	$(MAKE) -C $(SW_PRJ_DIR) elf

.PHONY: xsa
xsa: $(VIV_XSA)
$(VIV_XSA): 
	$(MAKE) -C $(HW_PRJ_DIR) xsa

.PHONY: clean
clean:
	$(MAKE) -C $(HW_PRJ_DIR) clean
	$(MAKE) -C $(SW_PRJ_DIR) clean
