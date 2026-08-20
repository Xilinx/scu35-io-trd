/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  main.c
//
//PURPOSE:
//  This C file is meant to be built with the Xilinx SDK and run as an
//  ELF file on a Microblaze-V processor.
//
//AUTHOR:
//  Thomas Delev
//
//DATE:
//  2025/06/24

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xstatus.h"
#include <sys/unistd.h>
#include <xil_printf.h>
#include <stdio.h>
#include <unistd.h>
#include "output_manager.h"
#include "input_manager.h"
#include "uart_config.h"

int main()
{

	//Variable definitions
	int Status=0;
	xil_printf("Started");

	Status = uart_interrupt_config();
	if (Status != XST_SUCCESS) {
		goto errorCatched;
	}
	Status = output_init();
	if (Status != XST_SUCCESS) {
		goto errorCatched;
	}
	Status = console_sync();
	if (Status != XST_SUCCESS) {
		goto errorCatched;
	}
	usleep(100000);
	Status = input_init();
	if (Status != XST_SUCCESS) {
		goto errorCatched;
	}
	usleep(1000000);

	while(1){
		input_updater();
	}

	errorCatched:
		usleep(100000);
		xil_printf("XXXXXXXXXXXXXXXXXXXXXXX");

    //End of program
	return Status;
}
