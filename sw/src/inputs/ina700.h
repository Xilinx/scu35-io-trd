/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

#ifndef INA700_H
#define INA700_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <xil_types.h>
#include "xstatus.h"

typedef struct INA700_Values {
    u32 temperature;
    u32 voltage;
    u32 current;
} INA700_Values;

s8 ina700_init(void);
void ina700(void);

#endif

