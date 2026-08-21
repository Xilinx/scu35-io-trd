/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  LEDs.c
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

#include "LEDs.h"
#include "xgpio.h"
#include "xparameters.h"
#include "stdlib.h"

/************************** Constant Definitions *****************************/

// Constant map from xparameters.h file.
#define LEDSGPIO_GPIO_BASEADDR    XPAR_GPIO_LEDS_BASEADDR
#define LEDSGPIO_CHANNEL	1

/************************** Variable Definitions *****************************/

XGpio LEDsGpio;             /* The instance of the GPIO Device */


/****************************************************************************/
/**
*
* This function modify the status of the 8 leds on board
*
* The first 2 ASCII char of the action param converted to its 8-bits value set
* the LED state. A 0 turn off the led, a 1 turn it on.
*
*
* @param	action is 12 last ASCII from the payload received.
*
* @return	None
*
****************************************************************************/
void leds(u8 *action){
	u32 mask = 0;
	// Isolate each character
	char val_led_3_0_c[] = {action[3], 0};
	char val_led_7_4_c[] = {action[2], 0};
	char val_led_11_8_c[] = {action[1], 0};
	char val_led_14_12_c[] = {action[0],0};

	// Convert to their binary value
	u32 val_led_3_0_u = strtol(val_led_3_0_c, NULL, 16);
	u32 val_led_7_4_u = strtol(val_led_7_4_c, NULL, 16);
	u32 val_led_11_8_u = strtol(val_led_11_8_c, NULL, 16);
	u32 val_led_14_12_u = strtol(val_led_14_12_c, NULL, 16);

	// Convert the u8 action[0] value to the mask
	mask = val_led_3_0_u + (val_led_7_4_u << 4) + (val_led_11_8_u << 8) +(val_led_14_12_u << 12);

	// Turn off the appropriate LEDs
	// By inverting the mask, we clear the LEDs with value 0
	XGpio_DiscreteClear(&LEDsGpio, LEDSGPIO_CHANNEL, ~mask);
	// Turn on the appropriate LEDs
	XGpio_DiscreteSet(&LEDsGpio, LEDSGPIO_CHANNEL, mask);

}

/****************************************************************************/
/**
*
* This function is use to initialize the leds on board
*
* @param	None.
*
* @return	None
*
****************************************************************************/
int leds_init(){
	if (XGpio_Initialize(&LEDsGpio, LEDSGPIO_GPIO_BASEADDR) != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&LEDsGpio, LEDSGPIO_CHANNEL, 0);

	return XST_SUCCESS;
}
