/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

#ifndef OUTPUT_MANAGER_H /* prevent circular inclusions */
#define OUTPUT_MANAGER_H /* by using protection macros */

#include "xil_types.h"
void output_updater(u8*);
int output_init(void);

#endif
