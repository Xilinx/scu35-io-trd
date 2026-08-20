/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

#ifndef LEDS_H /* prevent circular inclusions */
#define LEDS_H /* by using protection macros */

#include "xil_types.h"
int leds_init(void);
void leds(u8*);

#endif
