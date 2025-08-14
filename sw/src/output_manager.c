/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  output_manager.c
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

#include "output_manager.h"
#include "xstatus.h"
#include "outputs/LEDs.h"

/***************************** device format *********************************/

#define DEVICE_ID(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))

/****************************************************************************/
/**
*
* This function is called to update the output.
* The payload is decoded to identify the appropriate output device using the
* first4 ASCII characters. Then it trigger the appropriate action
*
* @param	payload is 16 ASCII payload received.
*
* @return	None
*
****************************************************************************/
void output_updater(u8 *payload){
	u8* action;
	u32 device;

	// // Extract the device and the action
	action = &payload[4];
	device = *((u32*)payload);

	switch(device) {
		// LEDs on board
		case DEVICE_ID('l','e','d','0'):
			leds(action);
			break;
	// 	// The device does not exist
		default:
			break;
	}
}

/****************************************************************************/
/**
*
* This function is called to initialize all outputs.
*
* @param	None
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
****************************************************************************/
int output_init(void){
	if (leds_init() != XST_SUCCESS
	) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}