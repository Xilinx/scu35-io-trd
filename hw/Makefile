# Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
# SPDX-License-Identifier: MIT

RM = rm -rf
VIVADO = $(XILINX_VIVADO)/bin/vivado
JOBS ?= 8

VIV_DESIGN = scu35_io_design

VIV_PRJ_DIR = project
VIV_SCRIPTS_DIR = scripts

VIV_XSA = $(VIV_PRJ_DIR)/$(VIV_DESIGN).xsa
VIV_PDI = $(VIV_PRJ_DIR)/$(VIV_DESIGN)_wrapper.pdi
VIV_SRC_XSA = $(VIV_SCRIPTS_DIR)/main.tcl
VIV_SRC_PDI = $(VIV_SCRIPTS_DIR)/add_elf.tcl

.PHONY: help
help:
	@echo 'Usage:'
	@echo ''
	@echo '  make xsa'
	@echo '    Generate extensible xsa platform'
	@echo ''
	@echo '  make pdi'
	@echo '    Generate device image with embedded software'
	@echo '    The Vitis software application must be generated before this command.'
	@echo ''

.PHONY: all
all: 
	@echo 'MAKE ALL is not a valid entry'
	@echo ''
	@echo 'Usage:'
	@echo '  make xsa'
	@echo '    Generate extensible xsa platform'
	@echo ''
	@echo '  make pdi'
	@echo '    Generate device image with embedded software'
	@echo '    The Vitis software application must be generated before this command.'
	@echo ''
	

.PHONY: xsa
xsa: $(VIV_XSA)
$(VIV_XSA):
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC_XSA) -tclargs -jobs $(JOBS)

.PHONY: pdi
pdi: $(VIV_PDI)
$(VIV_PDI): $(VIV_XSA) 
	$(VIVADO) -mode batch -notrace -source $(VIV_SRC_PDI) -tclargs -jobs $(JOBS)
	cp -f $(VIV_PRJ_DIR)/$(VIV_DESIGN).runs/impl_1/$(VIV_DESIGN)_wrapper.pdi $(VIV_PRJ_DIR)/$(VIV_DESIGN)_wrapper.pdi
	@echo ''
	@echo '	   The device image with the embedded software application'
	@echo '    was successfully generated. It is located here:'
	@echo '         hw/$(VIV_PRJ_DIR)/$(VIV_DESIGN)_wrapper.pdi'
	@echo ''

.PHONY: clean
clean:
	$(RM) $(VIV_PRJ_DIR) vivado* .Xil *dynamic* *.log *.xpe
