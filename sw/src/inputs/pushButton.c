/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  pushButton.c
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

#include "pushButton.h"
#include "xgpio.h"
#include "xparameters.h"
#include "stdio.h"
#include <unistd.h>
#include "../uart_config.h"
/************************** Constant Definitions *****************************/

// Constant map from xparameters.h file.
#define PUSHBGPIO_GPIO_BASEADDR    XPAR_GPIO_PUSHB_DIP_BASEADDR
#define PUSHBGPIO_CHANNEL	1

/************************** Variable Definitions *****************************/

XGpio 	PUSHBGpio;             /* The instance of the GPIO Device */
u8  	PUSHBState;

/************************** Function Prototypes ******************************/
void pushButton_sendState(void);
/****************************************************************************/
/**
*
* This function reads the status of the push buttons on board and
* update the console if there is a changes from the previous state.
*
* @param	None.
*
* @return	None
*
****************************************************************************/
void pushButton()
{
	u8 newState;
	u8 debounceState;
	int debounced = 0;

	// Debounce GPIO push button
	while (debounced != 1) {
		newState = XGpio_DiscreteRead(&PUSHBGpio, PUSHBGPIO_CHANNEL);
		usleep(50);
		debounceState = XGpio_DiscreteRead(&PUSHBGpio, PUSHBGPIO_CHANNEL);
		if (newState == debounceState) {
			debounced = 1;
		}
	}

	// If the push button state were modified, send update the console
	if (newState != PUSHBState) {
		PUSHBState = newState;
		pushButton_sendState();
	}
}

/****************************************************************************/
/**
*
* This function is use to initialize the push buttons on board
*
* @param	None
*
* @return	None
*
****************************************************************************/
int pushButton_init()
{
	if (XGpio_Initialize(&PUSHBGpio, PUSHBGPIO_GPIO_BASEADDR) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&PUSHBGpio, PUSHBGPIO_CHANNEL, 0xFFFFFFFF);
	PUSHBState = 0;

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is use to sent the push buttons state to the console
*
* @param	None
*
* @return	None
*
****************************************************************************/
void pushButton_sendState()
{

	u8 payload[16] = {'p','s','h','0'};
	const char hex_lookup[] = "0123456789abcdef";

	// Device Status
	payload[4] = hex_lookup[(PUSHBState >> 4)& 0xF];
	payload[5] = hex_lookup[PUSHBState & 0xF];

	uart_send(payload);
}
