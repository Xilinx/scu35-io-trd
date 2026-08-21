/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
function input_updater(payload) {
    const device = payload.slice(0,4).join('');
    const action = payload.slice(4,16);

    switch (device) {
        case 'psh0':
            pushButton(action);
            break;
        case 'dip0':
            DIPSwitches(action);
            break;
        case 'ina0':
            ina700_INT_T(action);
            break;
        case 'ina1':
            ina700_INT_V(action);
            break;
        case 'ina2':
            ina700_INT_A(action);
            break;
        case 'ina3':
            ina700_5V_T(action);
            break;
        case 'ina4':
            ina700_5V_V(action);
            break;
        case 'ina5':
            ina700_5V_A(action);
            break;
        case 'acc0':
            accel_X(action);
            break;
        case 'acc1':
            accel_Y(action);
            break;
        case 'acc2':
            accel_Z(action);
            break;
        case 'pmd0':
            pmodGPIO(action);
            break;
        default:
            break;
    }
    console.log(payload);
}

window.input_updater = input_updater;