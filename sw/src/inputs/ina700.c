/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  ina700.c
//
//PURPOSE:
//  This C++ file is meant to be built with the Xilinx SDK and run as an
//  ELF file on a Microblaze-V processor.
//  For more information about programming the INA700, please refer to
//  the Texas Instrucments documentation.
//
//AUTHOR:
//  Thomas Delev
//
//DATE:
//  2025/03/24

/***************************** Include Files *********************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../uart_config.h"
#include "ina700.h"
#include "xparameters.h"
#include "xiic.h"
#include "xstatus.h"

/************************** Constant Definitions *****************************/
#define INA700_IIC_BASEADDR    XPAR_IIC_INA_BASEADDR

/************************** Variable Definitions *****************************/
XIic INA700_IicInstance;		/* The instance of the IIC device. */
INA700_Values ina700_vccint_v;
INA700_Values ina700_5v_v;
u8 INA700_VCCINT_DEVADDR = 0x45;
u8 INA700_VCC5V_DEVADDR = 0x44;

/************************** Function Prototypes ******************************/
void ina700_sendState(void);
s8 ina700_i2cTransfer( u8 i2cAddr, u8 *dataToWrite, u8 writeLength, u8 *dataToRead,  u8 readLength);
float INA700_getTemp(u8 devAddr);
float INA700_getVoltage(u8 devAddr);
float INA700_getCurrent(u8 devAddr);
s8 ina700_config(u8 devAddr);

/****************************************************************************/
/**
*
* This function is use to initialize the INA700 and inform the
* console of the initial state
*
* @param	None
*
* @return	None
*
****************************************************************************/
s8 ina700_init()
{
    XIic_Config *INA700ConfigPtr;

    INA700ConfigPtr = XIic_LookupConfig(INA700_IIC_BASEADDR);
    if (INA700ConfigPtr == NULL) {
		return XST_FAILURE;
	}

	if (XIic_CfgInitialize(&INA700_IicInstance, INA700ConfigPtr, INA700ConfigPtr->BaseAddress) != XST_SUCCESS) {
		return XST_FAILURE;
	}

    // Configure the INA700 VCCINT
    if (ina700_config(INA700_VCCINT_DEVADDR) != XST_SUCCESS) {
        return XST_FAILURE;
    }

    // Configure the INA700_5V
    if (ina700_config(INA700_VCC5V_DEVADDR) != XST_SUCCESS) {
        return XST_FAILURE;
    }

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function get the Temperature, Voltage and Current from the INA700.
*
* @param	None.
*
* @return	None
*
****************************************************************************/
void ina700()
{
    float retrivedValue;

    retrivedValue = INA700_getTemp(INA700_VCCINT_DEVADDR);
    ina700_vccint_v.temperature = *((u32 *) &retrivedValue);
    retrivedValue = INA700_getVoltage(INA700_VCCINT_DEVADDR);
    ina700_vccint_v.voltage = *((u32 *) &retrivedValue);
    retrivedValue = INA700_getCurrent(INA700_VCCINT_DEVADDR);
    ina700_vccint_v.current = *((u32 *) &retrivedValue);
    retrivedValue = INA700_getTemp(INA700_VCC5V_DEVADDR);
    ina700_5v_v.temperature = *((u32 *) &retrivedValue);
    retrivedValue = INA700_getVoltage(INA700_VCC5V_DEVADDR);
    ina700_5v_v.voltage = *((u32 *) &retrivedValue);
    retrivedValue = INA700_getCurrent(INA700_VCC5V_DEVADDR);
    ina700_5v_v.current = *((u32 *) &retrivedValue);
	
    ina700_sendState();
    return;
}

/*
 *  ======== ina700_i2cTransfer ========
 *  Transfer data to and from an I2C slave
 *
 *  @param busId         id of an I2C bus to access for the transfer
 *  @param sensorAddress I2C address of peripheral to access
 *  @param dataToWrite   non-NULL pointer to a buffer of at least writeLength
 *                       bytes; may be NULL if writeLength = 0.
 *  @param writeLength   number of bytes to write from the dataToWrite array
 *  @param dataToRead    non-NULL pointer to a buffer of at least readLength
 *                       bytes; may be NULL if readLength = 0.
 *  @param readLength    number of bytes to read into dataToRead array
 *
 *  @return              0 if successful, otherwise non-zero
 */
s8 ina700_i2cTransfer( u8 i2cAddr, u8 *dataToWrite, u8 writeLength, u8 *dataToRead,  u8 readLength)
{
    /*
     *  Add MCU specific I2C read/write code here.
     */

    if (XIic_WaitBusFree(INA700_IIC_BASEADDR) == XST_FAILURE){
        return XST_FAILURE;
    }

    if (XIic_Send(INA700_IIC_BASEADDR, i2cAddr, dataToWrite, writeLength, XIIC_STOP) != writeLength){
        return XST_FAILURE;
    }

    if (readLength != 0){
        if (XIic_WaitBusFree(INA700_IIC_BASEADDR) == XST_FAILURE){
            return XST_FAILURE;
        }

        if (XIic_Recv(INA700_IIC_BASEADDR, i2cAddr, dataToRead, readLength, XIIC_STOP) != readLength){
            return XST_FAILURE;
        }
    }
    
    return XST_SUCCESS;
}

float INA700_getTemp(u8 devAddr){
    u8 txBuf[1] = {0};
    u8 rxBuf[2] = {0};

    float retrivedValue;

    txBuf[0] = 0x06;
    if (ina700_i2cTransfer(devAddr, txBuf, 1, rxBuf, 2)  != XST_SUCCESS){
        return 0;
    }
    
    // Merge retrieved value and remove reserved bits
    retrivedValue = (rxBuf[0] << 8 | rxBuf[1]) >> 4;

    //Convert for 2's compliment and signed value
    if(retrivedValue > 0x7FF)
    {
        return ((float)retrivedValue - 0x1000)*125/1000; 
    }
    else
    {
        return ((float)retrivedValue)*125/1000; 
    }
}

float INA700_getVoltage(u8 devAddr){
    u8 txBuf[1] = {0};
    u8 rxBuf[2] = {0};

    float retrivedValue;

    txBuf[0] = 0x05;
    if (ina700_i2cTransfer(devAddr, txBuf, 1, rxBuf, 2)  != XST_SUCCESS){
        return 0;
    }
    
    // Merge retrieved value and remove reserved bits
    retrivedValue = rxBuf[0] << 8 | rxBuf[1];

    //Convert for 2's compliment and signed value
    if(retrivedValue > 0x7FFF)
    {
        return ((float)retrivedValue - 0x10000)*3.125/1000; 
    }
    else
    {
        return ((float)retrivedValue)*3.125/1000; 
    }
}

float INA700_getCurrent(u8 devAddr){
    u8 txBuf[1] = {0};
    u8 rxBuf[2] = {0};

    float retrivedValue;

    txBuf[0] = 0x07;
    if (ina700_i2cTransfer(devAddr, txBuf, 1, rxBuf, 2)  != XST_SUCCESS){
        return 0;
    }
    
    // Merge retrieved value and remove reserved bits
    retrivedValue = rxBuf[0] << 8 | rxBuf[1];

    //Convert for 2's compliment and signed value
    if(retrivedValue > 0x7FFF)
    {
        return ((float)retrivedValue - 0x10000)*480/1000000; 
    }
    else
    {
        return ((float)retrivedValue)*480/1000000; 
    }
}

s8 ina700_config(u8 devAddr){
    u8 txBuf[3] = {0};
    
    // Sensor Config Register
    txBuf[0] = 0x00;
    txBuf[1] = 0x00;
    txBuf[2] = 0x00;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // ADC Config Register
    txBuf[0] = 0x01;
    txBuf[1] = 0xFB;
    txBuf[2] = 0x68;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Diagnostic and Alert Register
    txBuf[0] = 0x0B;
    txBuf[1] = 0x00;
    txBuf[2] = 0x00;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Current over limit Register
    txBuf[0] = 0x0C;
    txBuf[1] = 0x7F;
    txBuf[2] = 0xFF;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Current under limit Register
    txBuf[0] = 0x0D;
    txBuf[1] = 0x80;
    txBuf[2] = 0x00;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Bus Overvoltage Register
    txBuf[0] = 0x0E;
    txBuf[1] = 0x7F;
    txBuf[2] = 0xFF;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Bus Undervoltage Register
    txBuf[0] = 0x0F;
    txBuf[1] = 0x00;
    txBuf[2] = 0x00;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Temperature over limit Register
    txBuf[0] = 0x10;
    txBuf[1] = 0x7F;
    txBuf[2] = 0xF0;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;
    // Power over limit Register
    txBuf[0] = 0x11;
    txBuf[1] = 0xFF;
    txBuf[2] = 0xFF;
    if (ina700_i2cTransfer(devAddr, txBuf, 3, NULL, 0) != XST_SUCCESS) return XST_FAILURE;


    return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is use to sent the INA700 state to the console
*
* @param	None
*
* @return	None
*
****************************************************************************/
void ina700_sendState()
{    
	u8 payload_vccint_t[16] = {'i','n','a','0'};
	u8 payload_vccint_v[16] = {'i','n','a','1'};
	u8 payload_vccint_c[16] = {'i','n','a','2'};
	u8 payload_5v_t[16] = {'i','n','a','3'};
	u8 payload_5v_v[16] = {'i','n','a','4'};
	u8 payload_5v_c[16] = {'i','n','a','5'};
	const char hex_lookup[] = "0123456789abcdef";

	
	payload_vccint_t[4] = hex_lookup[(ina700_vccint_v.temperature >> 28)& 0xF];
    payload_vccint_t[5] = hex_lookup[(ina700_vccint_v.temperature >> 24)& 0xF];
    payload_vccint_t[6] = hex_lookup[(ina700_vccint_v.temperature >> 20)& 0xF];
    payload_vccint_t[7] = hex_lookup[(ina700_vccint_v.temperature >> 16)& 0xF];
    payload_vccint_t[8] = hex_lookup[(ina700_vccint_v.temperature >> 12)& 0xF];
    payload_vccint_t[9] = hex_lookup[(ina700_vccint_v.temperature >> 8)& 0xF];
    payload_vccint_t[10] = hex_lookup[(ina700_vccint_v.temperature >> 4)& 0xF];
    payload_vccint_t[11] = hex_lookup[ina700_vccint_v.temperature & 0xF];
    uart_send(payload_vccint_t);

    payload_vccint_v[4] = hex_lookup[(ina700_vccint_v.voltage >> 28)& 0xF];
    payload_vccint_v[5] = hex_lookup[(ina700_vccint_v.voltage >> 24)& 0xF];
    payload_vccint_v[6] = hex_lookup[(ina700_vccint_v.voltage >> 20)& 0xF];
    payload_vccint_v[7] = hex_lookup[(ina700_vccint_v.voltage >> 16)& 0xF];
    payload_vccint_v[8] = hex_lookup[(ina700_vccint_v.voltage >> 12)& 0xF];
    payload_vccint_v[9] = hex_lookup[(ina700_vccint_v.voltage >> 8)& 0xF];
    payload_vccint_v[10] = hex_lookup[(ina700_vccint_v.voltage >> 4)& 0xF];
    payload_vccint_v[11] = hex_lookup[ina700_vccint_v.voltage & 0xF];
    uart_send(payload_vccint_v);

    payload_vccint_c[4] = hex_lookup[(ina700_vccint_v.current >> 28)& 0xF];
    payload_vccint_c[5] = hex_lookup[(ina700_vccint_v.current >> 24)& 0xF];
    payload_vccint_c[6] = hex_lookup[(ina700_vccint_v.current >> 20)& 0xF];
    payload_vccint_c[7] = hex_lookup[(ina700_vccint_v.current >> 16)& 0xF];
    payload_vccint_c[8] = hex_lookup[(ina700_vccint_v.current >> 12)& 0xF];
    payload_vccint_c[9] = hex_lookup[(ina700_vccint_v.current >> 8)& 0xF];
    payload_vccint_c[10] = hex_lookup[(ina700_vccint_v.current >> 4)& 0xF];
    payload_vccint_c[11] = hex_lookup[ina700_vccint_v.current & 0xF];
    uart_send(payload_vccint_c);

	payload_5v_t[4] = hex_lookup[(ina700_5v_v.temperature >> 28)& 0xF];
    payload_5v_t[5] = hex_lookup[(ina700_5v_v.temperature >> 24)& 0xF];
    payload_5v_t[6] = hex_lookup[(ina700_5v_v.temperature >> 20)& 0xF];
    payload_5v_t[7] = hex_lookup[(ina700_5v_v.temperature >> 16)& 0xF];
    payload_5v_t[8] = hex_lookup[(ina700_5v_v.temperature >> 12)& 0xF];
    payload_5v_t[9] = hex_lookup[(ina700_5v_v.temperature >> 8)& 0xF];
    payload_5v_t[10] = hex_lookup[(ina700_5v_v.temperature >> 4)& 0xF];
    payload_5v_t[11] = hex_lookup[ina700_5v_v.temperature & 0xF];
    uart_send(payload_5v_t);

    payload_5v_v[4] = hex_lookup[(ina700_5v_v.voltage >> 28)& 0xF];
    payload_5v_v[5] = hex_lookup[(ina700_5v_v.voltage >> 24)& 0xF];
    payload_5v_v[6] = hex_lookup[(ina700_5v_v.voltage >> 20)& 0xF];
    payload_5v_v[7] = hex_lookup[(ina700_5v_v.voltage >> 16)& 0xF];
    payload_5v_v[8] = hex_lookup[(ina700_5v_v.voltage >> 12)& 0xF];
    payload_5v_v[9] = hex_lookup[(ina700_5v_v.voltage >> 8)& 0xF];
    payload_5v_v[10] = hex_lookup[(ina700_5v_v.voltage >> 4)& 0xF];
    payload_5v_v[11] = hex_lookup[ina700_5v_v.voltage & 0xF];
    uart_send(payload_5v_v);

    payload_5v_c[4] = hex_lookup[(ina700_5v_v.current >> 28)& 0xF];
    payload_5v_c[5] = hex_lookup[(ina700_5v_v.current >> 24)& 0xF];
    payload_5v_c[6] = hex_lookup[(ina700_5v_v.current >> 20)& 0xF];
    payload_5v_c[7] = hex_lookup[(ina700_5v_v.current >> 16)& 0xF];
    payload_5v_c[8] = hex_lookup[(ina700_5v_v.current >> 12)& 0xF];
    payload_5v_c[9] = hex_lookup[(ina700_5v_v.current >> 8)& 0xF];
    payload_5v_c[10] = hex_lookup[(ina700_5v_v.current >> 4)& 0xF];
    payload_5v_c[11] = hex_lookup[ina700_5v_v.current & 0xF];
    uart_send(payload_5v_c);
}
