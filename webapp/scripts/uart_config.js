/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

let port;
let writerPromise;
const serialStatus = document.getElementById('serial-output');
let output_init;

function uart_config() {
    // First, check if the serial API is supported by the browser
    if ('serial' in navigator) {
        const connectBtn = document.getElementById('connect-btn');
        connectBtn.addEventListener('click', initializeSerialPortConnection);
    } else {
        serialStatus.textContent = 'Serial API not supported';
    }
}

async function initializeSerialPortConnection() {
    try {
        // Request for the permission to access the serial ports
        const baudRateInput = document.getElementById('baud-rate');
    
        // Get the baud rate from the user input
        const baudRate = parseInt(baudRateInput.value, 10);
    
        // Open the serial port with the desired baud rate
        port = await navigator.serial.requestPort();
        await port.open({
            baudRate: baudRate,
            dataBits: 8,
            stopBits: 1,
            parity: 'none',
            flowControl: 'none',
            bufferSize: 16
        });
        serialStatus.textContent = 'Serial Port Opened';
        // Sync with MicroBlaze
        await mb_sync();

        // Initialize the board output
        output_init();
        

        // Start reading data from the serial port
        readSerialPortData(); 
    
    } catch (error) {
        serialStatus.textContent = 'Failed to access the serial port';
        console.error('Error accessing serial port:', error);
    }
}

async function readSerialPortData() {
    const data_received = new Array(23);
    let bytes_received = 0;

    while (port.readable) {
        let headerAt = 0;
        const reader = port.readable.getReader();
        try {
        let dataRead = '';
        const { value, done } = await reader.read();
        if (value) {
            dataRead = new TextDecoder().decode(value);
            for (let i = 0; i < dataRead.length; i++) {
                data_received[bytes_received] = dataRead[i];
                bytes_received++;
                // Verify the first 4 bytes to ensure we are reading a message intended for the console
	            // We are looking for the "microblaze to console" header (m2c{)
	            // If we did not receive the header, we reset the counter
                if (bytes_received == 1 && data_received[0] != 'm') {
                    bytes_received = 0;
                }
                else if (bytes_received == 2 && data_received[1] != '2'){
                    bytes_received = 0;
                }
                else if (bytes_received == 3 && data_received[2] != 'c') {
                    bytes_received = 0;
                }
                else if (bytes_received == 4 && data_received[3] != '{') {
                    bytes_received = 0;
                }

                // Verify if the message has been closed correctly, if not, check if a header can be found somewhere else in the payload
	            if (bytes_received == 21 && data_received[20] != '}') {
                    for (let j = 4; j < bytes_received; j++){
                        // Check for a header
                        if (data_received[j] == 'c' && ((j == 20) || 			// Last char is a c
                                (data_received[j+1] == '2' && ((j == 19) ||		// Last 2 char are c2
                                (data_received[j+2] == 'm' && ((j == 18) ||		// Last 3 char are c2m
                                data_received[j+3] == '{')))))) {
                            headerAt = j;
                            break;
                        }
                    }
                    // Move the data because a header was found
                    if (headerAt != 0) {
                        for (let j = 0; j < bytes_received; j++){
                            data_received[j] = data_received[headerAt + j];
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
                        // Send the payload to the input_manager for processing
                        input_updater(data_received.slice(4,20));
                    }
                    bytes_received = 0;
                }
            }            
        }
        if (done) {
            break;
        }
        } catch (error) {
        console.error('Error reading data:', error);
        } finally {
        reader.releaseLock();
        }
    }
}

function checksum_valid(message) {
    crc = crc16CCIT_calculator(message.slice(4,20));
    return crc === (message[21].charCodeAt(0) << 8 | message[22].charCodeAt(0));
}

async function sendToSerialPort(payload){
	let crc16 = crc16CCIT_calculator(payload);
	const message = new Uint8Array(23);
    
    message[0] = 'c'.charCodeAt(0);
    message[1] = '2'.charCodeAt(0);
    message[2] = 'm'.charCodeAt(0);
    message[3] = '{'.charCodeAt(0);

	// Add the payload to the message, after the Header (c2m{)
	for (let i = 0; i < 16; i++){
		message[i+4] = payload[i].charCodeAt(0);
	}
	// Add the tail
	message[20] = '}'.charCodeAt(0);

	// Add the crc
	message[21] = (crc16 >> 8) & 0xFF;
	message[22] = crc16 & 0xFF;

    const textDecoder = new TextDecoder('utf-8');
    console.log(textDecoder.decode(message));

    // Send the message
    try {
        writerPromise = writerPromise
            ? writerPromise.then(() => writeToPort(message))
            : writeToPort(message);
        await writerPromise;
    } catch (error) {
        console.error('Error writing data to serial port:', error);
    }
}

async function writeToPort(message){
    const writer = port.writable.getWriter();
    await writer.write(message);
    writer.releaseLock();
}

async function mb_sync() {
    const data_received = new Array(23);
    const sync_payload =  Array(16).fill('-');
        sync_payload[6] = 's';
        sync_payload[7] = 'y';
        sync_payload[8] = 'n';
        sync_payload[9] = 'c';
    const sack_payload =  Array(16).fill('-');
        sack_payload[6] = 's';
        sack_payload[7] = 'a';
        sack_payload[8] = 'c';
        sack_payload[9] = 'k';

    let bytes_received = 0;
    let mbSync = 0;

    sendToSerialPort(sync_payload);

    
    while (port.readable && mbSync != 2) {
        let headerAt = 0;
        const reader = port.readable.getReader();
        try {
        let dataRead = '';
        const { value, done } = await reader.read();
        if (value) {
            dataRead = new TextDecoder().decode(value);
            for (let i = 0; i < dataRead.length; i++) {
                data_received[bytes_received] = dataRead[i];
                bytes_received++;
                // Verify the first 4 bytes to ensure we are reading a message intended for the console
	            // We are looking for the "microblaze to console" header (m2c{)
	            // If we did not receive the header, we reset the counter
                if (bytes_received == 1 && data_received[0] != 'm') {
                    bytes_received = 0;
                }
                else if (bytes_received == 2 && data_received[1] != '2'){
                    bytes_received = 0;
                }
                else if (bytes_received == 3 && data_received[2] != 'c') {
                    bytes_received = 0;
                }
                else if (bytes_received == 4 && data_received[3] != '{') {
                    bytes_received = 0;
                }

                // Verify if the message has been closed correctly, if not, check if a header can be found somewhere else in the payload
	            if (bytes_received == 21 && data_received[20] != '}') {
                    for (let j = 4; j < bytes_received; j++){
                        // Check for a header
                        if (data_received[j] == 'c' && ((j == 20) || 			// Last char is a c
                                (data_received[j+1] == '2' && ((j == 19) ||		// Last 2 char are c2
                                (data_received[j+2] == 'm' && ((j == 18) ||		// Last 3 char are c2m
                                data_received[j+3] == '{')))))) {
                            headerAt = j;
                            break;
                        }
                    }
                    // Move the data because a header was found
                    if (headerAt != 0) {
                        for (let j = 0; j < bytes_received; j++){
                            data_received[j] = data_received[headerAt + j];
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
                        // Verify that we received the sync message
                        if (mbSync == 0 && (data_received.slice(4,20).join('') == '------sync------')) {
                            mbSync = 1;
                            sendToSerialPort(sack_payload);
                        }
                        if (mbSync == 1 && (data_received.slice(4,20).join('') == '------sack------')) {
                            mbSync = 2;
                        }
                    }
                    console.log(data_received);
                    bytes_received = 0;
                }
            }            
        }
        if (done) {
            break;
        }
        } catch (error) {
        console.error('Error reading data:', error);
        } finally {
        reader.releaseLock();
        }
    }
}

window.addEventListener('load', function() {
    output_init = window.output_updater;
});

window.uart_config = uart_config;
window.sendToSerialPort = sendToSerialPort;