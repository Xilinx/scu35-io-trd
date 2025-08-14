/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

#ifndef UART_CONFIG_H /* prevent circular inclusions */
#define UART_CONFIG_H /* by using protection macros */

#include "xil_types.h"
int uart_interrupt_config(void);
void uart_send(u8*);
int console_sync(void);

#endif
