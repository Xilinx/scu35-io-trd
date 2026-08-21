/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  uart_config.c
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

#include "uart_config.h"
#include "xil_exception.h"
#include "xuartlite.h"
#include <unistd.h>
#include "xintc.h"
#include "crc16CCIT_calculator.h"
#include "output_manager.h"
#include "xparameters.h"
/************************** Constant Definitions *****************************/

// Constant map from xparameters.h file.
#define UARTLITE_BASEADDR       XPAR_AXI_UARTLITE_0_BASEADDR
#define INTC_BASEADDR           XPAR_XINTC_0_BASEADDR
#define UARTLITE_INT_IRQ_ID     XPAR_AXI_UARTLITE_0_INTERRUPTS

// Constant for the receive and sent buffer size
#define	UART_BUFFER_SIZE	23

/************************** Function Prototypes ******************************/

int SetupInterruptSystem(XUartLite *UartLitePtr);
void SendHandler(void *CallBackRef, unsigned int EventData);
void RecvHandler(void *CallBackRef, unsigned int EventData);
int checksum_valid(u8 *message);
/************************** Variable Definitions *****************************/

XUartLite UartLite;             /* The instance of the UartLite Device */
XIntc InterruptController;      /* The instance of the Interrupt Controller */
int consoleSync = 0;

/****************************************************************************/
/**
*
* This function configure the UARTLITE and the
*
* @param
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
****************************************************************************/
int uart_interrupt_config(){
	int Status;

	// Initialize the UartLite
	Status = XUartLite_Initialize(&UartLite, UARTLITE_BASEADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Perform self-test
	Status = XUartLite_SelfTest(&UartLite);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Connect the UartLite to the interrupt subsystem
	Status = SetupInterruptSystem(&UartLite);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Setup the UartLite handlers
	XUartLite_SetSendHandler(&UartLite, SendHandler, &UartLite);
	XUartLite_SetRecvHandler(&UartLite, RecvHandler, &UartLite);

	// Enable the UartLite interrupt
	XUartLite_EnableInterrupt(&UartLite);

	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function setups the interrupt system such that interrupts can occur
* for the UartLite device.
*
* @param    UartLitePtr contains a pointer to the instance of the UartLite
*           component which is going to be connected to the interrupt
*           controller.
*
* @return   XST_SUCCESS if successful, otherwise XST_FAILURE.
*
****************************************************************************/
int SetupInterruptSystem(XUartLite *UartLitePtr)
{
	int Status;

	// Initialize the interrupt controller
	Status = XIntc_Initialize(&InterruptController, INTC_BASEADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Connect the device handler to be called when an interrupt occurs
	Status = XIntc_Connect(&InterruptController, UARTLITE_INT_IRQ_ID,
			   (XInterruptHandler)XUartLite_InterruptHandler,
			   (void *)UartLitePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Start the interrupt controller
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// Enable the UartLite interrupt
	XIntc_Enable(&InterruptController, UARTLITE_INT_IRQ_ID);

	// Initialize the exception
	Xil_ExceptionInit();

	// Register the interrupt controller handler to the exception
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)XIntc_InterruptHandler,
			 &InterruptController);

	// Enable exceptions
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function is the handler which performs processing to send data to the
* UartLite. It is called from an interrupt context such that the amount of
* processing performed should be minimized. It is called when the transmit
* FIFO of the UartLite is empty and more data can be sent through the UartLite.
*
* This handler provides an example of how to handle data for the UartLite,
* but is application specific.
*
* @param	CallBackRef contains a callback reference from the driver.
*		In this case it is the instance pointer for the UartLite driver.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
****************************************************************************/
void SendHandler(void *CallBackRef, unsigned int EventData)
{}

/****************************************************************************/
/**
*
* This function is the handler which performs processing to receive data from
* the UartLite. It is called from an interrupt context such that the amount of
* processing performed should be minimized.  It is called data is present in
* the receive FIFO of the UartLite such that the data can be retrieved from
* the UartLite. The size of the data present in the FIFO is not known when
* this function is called.
*
* @param	CallBackRef contains a callback reference from the driver, in
*		this case it is the instance pointer for the UartLite driver.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
****************************************************************************/
void RecvHandler(void *CallBackRef, unsigned int EventData)
{
	static u8 data_received[23];
	static int bytes_received = 0;
	int headerAt = 0;

	// Read the received byte
	XUartLite_Recv(&UartLite, &data_received[bytes_received], 1);
	bytes_received++;


	// Verify the first 4 bytes to ensure we are reading a message intended for the MB-V
	// We are looking for the "console to microblaze" header (c2m{)
	// If we did not receive the header, we reset the counter
	if (bytes_received == 1 && data_received[0] != 'c') {
		bytes_received = 0;
	}
	else if (bytes_received == 2 && data_received[1] != '2'){
		bytes_received = 0;
	}
	else if (bytes_received == 3 && data_received[2] != 'm') {
		bytes_received = 0;
	}
	else if (bytes_received == 4 && data_received[3] != '{') {
		bytes_received = 0;
	}

	// Verify if the message has been closed correctly, if not, check if a header can be found somewhere else in the payload
	if (bytes_received == 21 && data_received[20] != '}') {
		for (int i = 4; i < bytes_received; i++){
			// Check for a header
			if (data_received[i] == 'c' && ((i == 20) || 			// Last char is a c
					(data_received[i+1] == '2' && ((i == 19) ||		// Last 2 char are c2
					(data_received[i+2] == 'm' && ((i == 18) ||		// Last 3 char are c2m
					data_received[i+3] == '{')))))) {
				headerAt = i;
				break;
			}
		}
		// Move the data because a header was found
		if (headerAt != 0) {
			for (int i = 0; i < bytes_received; i++){
				data_received[i] = data_received[headerAt + i];
			}
			bytes_received -= headerAt;
		}
		// No header found, reset
		else {
			bytes_received = 0;
		}
	}

	// The complete message has been received
	if (bytes_received == 23) {
		// Verify the Checksum
		if (checksum_valid(data_received)) {
			// Verify if the MB is sync with the console
			if (consoleSync == 0){
				// Verify the payload to make sure we received the sync sequence
				if (data_received[4] == '-' &&
						data_received[5] == '-' &&
						data_received[6] == '-' &&
						data_received[7] == '-' &&
						data_received[8] == '-' &&
						data_received[9] == '-' &&
						data_received[10] == 's' &&
						data_received[11] == 'y' &&
						data_received[12] == 'n' &&
						data_received[13] == 'c' &&
						data_received[14] == '-' &&
						data_received[15] == '-' &&
						data_received[16] == '-' &&
						data_received[17] == '-' &&
						data_received[18] == '-' &&
						data_received[19] == '-') {
					consoleSync = 1;
				}
			}
			else if (consoleSync == 1) {
				// Verify the payload to make sure we received the sync ack sequence
				if (data_received[4] == '-' &&
						data_received[5] == '-' &&
						data_received[6] == '-' &&
						data_received[7] == '-' &&
						data_received[8] == '-' &&
						data_received[9] == '-' &&
						data_received[10] == 's' &&
						data_received[11] == 'a' &&
						data_received[12] == 'c' &&
						data_received[13] == 'k' &&
						data_received[14] == '-' &&
						data_received[15] == '-' &&
						data_received[16] == '-' &&
						data_received[17] == '-' &&
						data_received[18] == '-' &&
						data_received[19] == '-') {
					consoleSync = 2;
				}
			}
			else if (consoleSync == 2){
				// update the board outputs
				output_updater(&data_received[4]);
			}
		}
		bytes_received = 0;
	}
}

/****************************************************************************/
/**
*
* This function is called to verify if the checksum received is valid.
*
* @param	message is 23 ASCII message received.
*
* @return	True or False.
*
****************************************************************************/
int checksum_valid(u8 *message){
    // Calculate the 16-bit CRC using CRC-16-CCITT
    u16 crc;
    crc = crc16CCIT_calculator(&message[4]);

    // Verify that the computed CRC correspond to the received CRC
    return crc == ((u16) message[21] << 8 | message[22]);
}

/****************************************************************************/
/**
*
* This function is called to synchronize the console and the MB, to make sure
* both are ready.
*
* @param	None.
*
* @return	XST_SUCCESS if successful
*
****************************************************************************/
int console_sync(){
	u8 sync_payload[16] = {'-','-','-','-','-','-','s','y','n','c','-','-','-','-','-','-'};
	u8 sack_payload[16] = {'-','-','-','-','-','-','s','a','c','k','-','-','-','-','-','-'};
	// wait for the sync signal from the console
	while (consoleSync == 0) {}

	// As long as the console has not acknowledge the sync continue sending the sync
	while (consoleSync == 1) {
		uart_send(sync_payload);
		usleep(10000);
	}
	uart_send(sack_payload);
	return XST_SUCCESS;
}

/****************************************************************************/
/**
*
* This function is called to send a payload through the UartLite.
* It adds the header, the tail and calculate the CRC
*
* @param	payload is 16 ASCII payload to send.
*
* @return	None
*
****************************************************************************/
void uart_send(u8 *payload){
	u16 crc16 = crc16CCIT_calculator(payload);
	u8 message[23] = {'m','2','c','{'};
	int bytes_sent = 0;

	// While the uartlite is sending, wait to ensure not to modify what is being sent
	while (XUartLite_IsSending(&UartLite)) {}

	// Add the payload to the message, after the Header (m2c{)
	for (int i = 0; i < 16; i++){
		message[i+4] = payload[i];
	}
	// Add the tail
	message[20] = '}';

	// Add the crc
	message[21] = (u8) ((crc16 >> 8) & 0xFF);
	message[22] = (u8) (crc16 & 0xFF);

	while (bytes_sent < 23) {
		bytes_sent += XUartLite_Send(&UartLite, &message[bytes_sent], 23-bytes_sent);
//		bytes_sent = bytes_sent;
	}

}
