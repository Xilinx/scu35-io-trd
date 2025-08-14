/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */

function crc16CCIT_calculator(payload) {
    let crc = 0xFFFF;
    let crc8_0;
    let crc8_1;
    let crc_int;
    for (let i = 0; i < 16; i += 2){
        crc_int = (payload[i].charCodeAt(0) | (payload[i+1].charCodeAt(0) << 8));
        crc ^= crc_int;
        for (let j = 0; j < 16; j++){
            crc = (crc & 0x8000) ? ((crc << 1) ^ 0x1021) : (crc << 1);
        }
    }

    crc8_0 = (((crc >> 8) & 0xFF) % 94) + 32;
    crc8_1 = ((crc & 0xFF) % 94) +32;

    crc = (crc8_0 << 8) | crc8_1;
    return crc;
}

window.crc16CCIT_calculator = crc16CCIT_calculator;