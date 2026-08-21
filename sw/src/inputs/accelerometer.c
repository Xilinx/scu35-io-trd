/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  accelerometer.c
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
#include <stdint.h>
#include "../uart_config.h"
#include "accelerometer.h"
#include "xparameters.h"
#include "xiic.h"
#include "xstatus.h"
#include <xil_printf.h>

/************************** Constant Definitions *****************************/
#define ACCELEROMETER_ADDR    XPAR_IIC_ACL_BASEADDR

/************************** Variable Definitions *****************************/
XIic accelerometer_IicInstance;		/* The instance of the IIC device. */
accelerometer_Values accelerometer_v;
float scaleFactor;

/************************** Function Prototypes ******************************/
void accelerometer_sendState(void);
s8 accelerometer_i2cTransfer(u8, u8*, u8, u8*, u8);

/****************************************************************************/
/**
*
* This function is use to initialize the accelerometer IIC and inform the
* console of the initial state
*
* @param	None
*
* @return	None
*
****************************************************************************/
s8 accelerometer_init()
{
    XIic_Config *ACCELEROMETERConfigPtr;
    u8 dataToWrite[2];
    u8 dataToRead[1];

    ACCELEROMETERConfigPtr = XIic_LookupConfig(ACCELEROMETER_ADDR);
    if (ACCELEROMETERConfigPtr == NULL) {
		return XST_FAILURE;
	}

	if (XIic_CfgInitialize(&accelerometer_IicInstance, ACCELEROMETERConfigPtr, ACCELEROMETERConfigPtr->BaseAddress) != XST_SUCCESS) {
		return XST_FAILURE;
	}
    // Enable the accelerometer and the 3 axis
    dataToWrite[0] = 0x20;
    dataToWrite[1] = 0x4F;
    accelerometer_i2cTransfer(0x18, dataToWrite, 2, NULL, 0);
    // Get Scale Factor
    dataToWrite[0] = 0x23;
    if (accelerometer_i2cTransfer(0x18, dataToWrite, 1, dataToRead, 1) != XST_SUCCESS){
        return XST_FAILURE;
    }
    switch ((dataToRead[0] & 0x30) >> 4) {
        case 0:
            scaleFactor = 2.0;
            break;
        case 1:
            scaleFactor = 4.0;
            break;
        case 2:
            scaleFactor = 8.0;
            break;
        case 3:
            scaleFactor = 16.0;
            break;
        default:
            return XST_FAILURE;
    }
	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function reads the status of the accelerometer and
* update the console.
*
* @param	None.
*
* @return	None
*
****************************************************************************/
void accelerometer()
{
    u8 dataToWrite[1];
    u8 dataToRead[1];
    float floatVal;

    // Get X acceleration
    dataToWrite[0] = 0x29;
	if (accelerometer_i2cTransfer(0x18, dataToWrite, 1, dataToRead, 1) != XST_SUCCESS){
        return;
    }
    floatVal = (scaleFactor*15.6/512) * ((int8_t) dataToRead[0]);
    accelerometer_v.acc_x = *((u32 *) &floatVal);

    // Get Y acceleration
    dataToWrite[0] = 0x2B;
	if (accelerometer_i2cTransfer(0x18, dataToWrite, 1, dataToRead, 1) != XST_SUCCESS){
        return;
    }
    floatVal = (scaleFactor*15.6/512) * ((int8_t) dataToRead[0]);
    accelerometer_v.acc_y = *((u32 *) &floatVal);

    // Get Z acceleration
    dataToWrite[0] = 0x2D;
	if (accelerometer_i2cTransfer(0x18, dataToWrite, 1, dataToRead, 1) != XST_SUCCESS){
        return;
    }
    floatVal = (scaleFactor*15.6/512) * ((int8_t) dataToRead[0]);
    accelerometer_v.acc_z = *((u32 *) &floatVal);

    accelerometer_sendState();
    return;
}

void accelerometer_sendState(){

    u8 payload_acc_x[16] = {'a','c','c','0'};
	u8 payload_acc_y[16] = {'a','c','c','1'};
	u8 payload_acc_z[16] = {'a','c','c','2'};
	const char hex_lookup[] = "0123456789abcdef";

	
	payload_acc_x[4] = hex_lookup[(accelerometer_v.acc_x >> 28)& 0xF];
    payload_acc_x[5] = hex_lookup[(accelerometer_v.acc_x >> 24)& 0xF];
    payload_acc_x[6] = hex_lookup[(accelerometer_v.acc_x >> 20)& 0xF];
    payload_acc_x[7] = hex_lookup[(accelerometer_v.acc_x >> 16)& 0xF];
    payload_acc_x[8] = hex_lookup[(accelerometer_v.acc_x >> 12)& 0xF];
    payload_acc_x[9] = hex_lookup[(accelerometer_v.acc_x >> 8)& 0xF];
    payload_acc_x[10] = hex_lookup[(accelerometer_v.acc_x >> 4)& 0xF];
    payload_acc_x[11] = hex_lookup[accelerometer_v.acc_x & 0xF];
    uart_send(payload_acc_x);

    payload_acc_y[4] = hex_lookup[(accelerometer_v.acc_y >> 28)& 0xF];
    payload_acc_y[5] = hex_lookup[(accelerometer_v.acc_y >> 24)& 0xF];
    payload_acc_y[6] = hex_lookup[(accelerometer_v.acc_y >> 20)& 0xF];
    payload_acc_y[7] = hex_lookup[(accelerometer_v.acc_y >> 16)& 0xF];
    payload_acc_y[8] = hex_lookup[(accelerometer_v.acc_y >> 12)& 0xF];
    payload_acc_y[9] = hex_lookup[(accelerometer_v.acc_y >> 8)& 0xF];
    payload_acc_y[10] = hex_lookup[(accelerometer_v.acc_y >> 4)& 0xF];
    payload_acc_y[11] = hex_lookup[accelerometer_v.acc_y & 0xF];
    uart_send(payload_acc_y);

    payload_acc_z[4] = hex_lookup[(accelerometer_v.acc_z >> 28)& 0xF];
    payload_acc_z[5] = hex_lookup[(accelerometer_v.acc_z >> 24)& 0xF];
    payload_acc_z[6] = hex_lookup[(accelerometer_v.acc_z >> 20)& 0xF];
    payload_acc_z[7] = hex_lookup[(accelerometer_v.acc_z >> 16)& 0xF];
    payload_acc_z[8] = hex_lookup[(accelerometer_v.acc_z >> 12)& 0xF];
    payload_acc_z[9] = hex_lookup[(accelerometer_v.acc_z >> 8)& 0xF];
    payload_acc_z[10] = hex_lookup[(accelerometer_v.acc_z >> 4)& 0xF];
    payload_acc_z[11] = hex_lookup[accelerometer_v.acc_z & 0xF];
    uart_send(payload_acc_z);
}

s8 accelerometer_i2cTransfer(u8 i2cAddr, u8 *dataToWrite, u8 writeLength, u8 *dataToRead, u8 readLength) {
    if (XIic_WaitBusFree(ACCELEROMETER_ADDR) == XST_FAILURE){
        return XST_FAILURE;
    }

    if (XIic_Send(ACCELEROMETER_ADDR, i2cAddr, dataToWrite, writeLength, XIIC_STOP) != writeLength){
        return XST_FAILURE;
    }

    if (readLength != 0){
        if (XIic_WaitBusFree(ACCELEROMETER_ADDR) == XST_FAILURE){
            return XST_FAILURE;
        }

        if (XIic_Recv(ACCELEROMETER_ADDR, i2cAddr, dataToRead, readLength, XIIC_STOP) != readLength){
            return XST_FAILURE;
        }
    }
    
    return XST_SUCCESS;
}