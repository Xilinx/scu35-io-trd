/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  dipSwitches.c
//
//PURPOSE:
//  This C++ file is meant to be built with the Xilinx SDK and run as an
//  ELF file on a Microblaze-V processor.
//
//AUTHOR:
//  Thomas Delev
//
//DATE:
//  2025/06/24

/***************************** Include Files *********************************/

#include "dipSwitches.h"
#include "xgpio.h"
#include "xparameters.h"
#include "stdio.h"
#include <unistd.h>
#include "../uart_config.h"
/************************** Constant Definitions *****************************/

// Constant map from xparameters.h file.
#define DIPSGPIO_GPIO_BASEADDR  XPAR_GPIO_PUSHB_DIP_BASEADDR
#define DIPSGPIO_CHANNEL	2

/************************** Variable Definitions *****************************/

XGpio DIPSGpio;             /* The instance of the GPIO Device */
u32   DIPSState;

/************************** Function Prototypes ******************************/
void dipSwitches_sendState(void);
/****************************************************************************/
/**
*
* This function reads the status of the dip switches on board and
* update the console if there is a changes from the previous state.
*
* @param	None.
*
* @return	None
*
****************************************************************************/
void dipSwitches()
{
	u32 newState;
	u32 debounceState;
	int debounced = 0;

	// Debounce GPIO dip switches
	while (debounced != 1) {
		newState = XGpio_DiscreteRead(&DIPSGpio, DIPSGPIO_CHANNEL);
		usleep(50);
		debounceState = XGpio_DiscreteRead(&DIPSGpio, DIPSGPIO_CHANNEL);
		if (newState == debounceState) {
			debounced = 1;
		}
	}

	// If the dip switches were modifier, send update the console
	if (newState != DIPSState) {
		DIPSState = newState;
		dipSwitches_sendState();
	}
}

/****************************************************************************/
/**
*
* This function is use to initialize the dip switches on board and inform the
* console of the initial state
*
* @param	None
*
* @return	None
*
****************************************************************************/
int dipSwitches_init()
{
	if (XGpio_Initialize(&DIPSGpio, DIPSGPIO_GPIO_BASEADDR) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&DIPSGpio, DIPSGPIO_CHANNEL, 0xFFFFFFFF);
	DIPSState = XGpio_DiscreteRead(&DIPSGpio, DIPSGPIO_CHANNEL);
	dipSwitches_sendState();

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is use to sent the dip switches state to the console
*
* @param	None
*
* @return	None
*
****************************************************************************/
void dipSwitches_sendState()
{

	u8 payload[16] = {'d','i','p','0'};
	const char hex_lookup[] = "0123456789abcdef";

	// Device Status
	payload[4] = hex_lookup[(DIPSState >> 4)& 0xF];
	payload[5] = hex_lookup[DIPSState & 0xF];

	uart_send(payload);
}
