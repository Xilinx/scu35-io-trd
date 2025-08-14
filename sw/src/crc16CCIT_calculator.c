/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

//NAME:
//  crc16CCIT_calculator.c
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


// Calculate the checksum of a 16 bytes payload using the CRC-16-CCIT algorithm
#include "crc16CCIT_calculator.h"

u16 crc16CCIT_calculator(u8 *payload){
    u16 crc = 0xFFFF;
    u8 crc8[2];
    u16 crc_int;
    for (int i = 0; i < 16; i += 2){
    	crc_int = ((uint16_t) payload[i]) | ((uint16_t) payload[i+1] << 8);
        crc ^= crc_int;
        for (int j = 0; j < 16; j++) {
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
        }
    }

    // Ensure the CRCs are in the printable range
    crc8[0] = (((u8) ((crc >> 8) & 0xFF)) % 94) + 32;
    crc8[1] = (((u8) (crc & 0xFF)) % 94) + 32;

    crc = (crc8[0] << 8) | crc8[1];

    return crc;
}
