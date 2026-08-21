/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  pmodGPIO.c
//
//PURPOSE:
//  This C++ file is meant to be built with the Xilinx SDK and run as an
//  ELF file on a Microblaze-V processor.
//
//AUTHOR:
//  Thomas Delev
//
//DATE:
//  2025/06/25

/***************************** Include Files *********************************/

#include "pmodGPIO.h"
#include "xgpio.h"
#include "xparameters.h"
#include "stdio.h"
#include <unistd.h>
#include "../uart_config.h"
/************************** Constant Definitions *****************************/

// Constant map from xparameters.h file.
#define PMOD_GPIO_BASEADDR    XPAR_GPIO_PMOD_I_BASEADDR
#define PMOD_GPIO_CHANNEL	  1

/************************** Variable Definitions *****************************/

XGpio 	PmodGPIO;             /* The instance of the GPIO Device */
u32  	PmodGPIOState;

/************************** Function Prototypes ******************************/
void pmodGPIO_sendState(void);
/****************************************************************************/
/**
*
* This function reads the status of the pmod inputs and
* update the console if there is a changes from the previous state.
*
* @param	None.
*
* @return	None
*
****************************************************************************/
void pmodGPIO()
{
	u32 newState;

	// Read the state of the pmod input
	newState = XGpio_DiscreteRead(&PmodGPIO, PMOD_GPIO_CHANNEL);
	
	// If the pmod input is different from the previous one, send the update
	if (newState != PmodGPIOState) {
		PmodGPIOState = newState;
		pmodGPIO_sendState();
	}
}

/****************************************************************************/
/**
*
* This function is use to initialize the pmod as GPIO inputs
*
* @param	None
*
* @return	None
*
****************************************************************************/
int pmodGPIO_init()
{
	if (XGpio_Initialize(&PmodGPIO, PMOD_GPIO_BASEADDR) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&PmodGPIO, PMOD_GPIO_CHANNEL, 0xFFFFFFFF);
	
	PmodGPIOState = XGpio_DiscreteRead(&PmodGPIO, PMOD_GPIO_CHANNEL);
	pmodGPIO_sendState();

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is use to sent the pmods state to the console
*
* @param	None
*
* @return	None
*
****************************************************************************/
void pmodGPIO_sendState()
{

	u8 payload[16] = {'p','m','d','0'};
	const char hex_lookup[] = "0123456789abcdef";

	// Device Status
	payload[4] = hex_lookup[(PmodGPIOState >> 28)& 0xF];
	payload[5] = hex_lookup[(PmodGPIOState >> 24)& 0xF];
	payload[6] = hex_lookup[(PmodGPIOState >> 20)& 0xF];
	payload[7] = hex_lookup[(PmodGPIOState >> 16)& 0xF];
	payload[8] = hex_lookup[(PmodGPIOState >> 12)& 0xF];
	payload[9] = hex_lookup[(PmodGPIOState >> 8)& 0xF];
	payload[10] = hex_lookup[(PmodGPIOState >> 4)& 0xF];
	payload[11] = hex_lookup[PmodGPIOState & 0xF];

	uart_send(payload);
}
