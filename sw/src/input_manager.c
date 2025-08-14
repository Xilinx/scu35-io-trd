/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  intput_manager.c
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

#include "input_manager.h"
#include "xstatus.h"
#include <unistd.h>
#include "inputs/pushButton.h"
#include "inputs/dipSwitches.h"
#include "inputs/ina700.h"
#include "inputs/accelerometer.h"
#include "inputs/pmodGPIO.h"
#include "xparameters.h"  
#include "xtmrctr.h"  
/************************** Variable Definitions *****************************/
#define TIMER_BASEADDR XPAR_AXI_TIMER_0_BASEADDR
#define DELAY_1SECOND XPAR_CPU_CORE_CLOCK_FREQ_HZ
XTmrCtr TIMER_INSTANCE;

/****************************************************************************/
/**
*
* This function is called to trigger a read of all input and update the console.
*
* @param	None
*
* @return	None
*
****************************************************************************/
void input_updater(){
	static u32 lastINA_timer = 0;

	pushButton();
	dipSwitches();
	pmodGPIO();
	if ((XTmrCtr_GetValue(&TIMER_INSTANCE, 0) - lastINA_timer) >= DELAY_1SECOND){
		ina700();
		// usleep(1000);
		accelerometer();
		// usleep(1000);
		lastINA_timer = XTmrCtr_GetValue(&TIMER_INSTANCE, 0);
	}
}

/****************************************************************************/
/**
*
* This function is called to initialize all inputs and provide the console
* with their initial status.
*
* @param	None
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
****************************************************************************/
int input_init()
{
	if (pushButton_init() != XST_SUCCESS
        || dipSwitches_init() != XST_SUCCESS
		|| ina700_init() != XST_SUCCESS
		|| accelerometer_init() != XST_SUCCESS
		|| pmodGPIO_init() != XST_SUCCESS
	) {
		return XST_FAILURE;
	}
	
	if (XTmrCtr_Initialize(&TIMER_INSTANCE, TIMER_BASEADDR) == XST_DEVICE_NOT_FOUND) {
		return XST_FAILURE;
	}
	XTmrCtr_SetOptions(&TIMER_INSTANCE, 0, XTC_AUTO_RELOAD_OPTION);
	XTmrCtr_SetResetValue(&TIMER_INSTANCE, 0, 0);
	XTmrCtr_Start(&TIMER_INSTANCE, 0);    

	return XST_SUCCESS;
}