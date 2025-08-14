/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <xil_types.h>
#include "xstatus.h"

typedef struct accelerometer_Values {
    u32 acc_x;
    u32 acc_y;
    u32 acc_z;
} accelerometer_Values;

s8 accelerometer_init(void);
void accelerometer(void);

#endif

