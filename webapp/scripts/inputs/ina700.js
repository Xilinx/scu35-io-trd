/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
function ina700_INT_T(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('ina700_INT_T').querySelector('.ina700-text').textContent = `Die Temperature: ${floatVal.toFixed(3)} C`;
}

function ina700_INT_V(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('ina700_INT_V').querySelector('.ina700-text').textContent = `Bus Voltage: ${floatVal.toFixed(3)} V`;
}

function ina700_INT_A(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('ina700_INT_A').querySelector('.ina700-text').textContent = `Bus Current: ${floatVal.toFixed(3)} A`;
}

function ina700_5V_T(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('ina700_5V_T').querySelector('.ina700-text').textContent = `Die Temperature: ${floatVal.toFixed(3)} C`;
}

function ina700_5V_V(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('ina700_5V_V').querySelector('.ina700-text').textContent = `Bus Voltage: ${floatVal.toFixed(3)} V`;
}

function ina700_5V_A(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('ina700_5V_A').querySelector('.ina700-text').textContent = `Bus Current: ${floatVal.toFixed(3)} A`;
}


document.addEventListener('DOMContentLoaded', function() {
    const ina700HTML = `
<div class="ina700Group">
    <span class="ina700Group-text">VCC INT</span>
    <div class="ina700" id="ina700_INT_T">
        <span class="ina700-text">Die Temperature:</span>
    </div>
    <div class="ina700" id="ina700_INT_V">
        <span class="ina700-text">Bus Voltage:</span>
    </div>
    <div class="ina700" id="ina700_INT_A">
        <span class="ina700-text">Bus Current:</span>
    </div>
</div>
<div class="ina700Group">
    <span class="ina700Group-text">VCC 5V</span>
    <div class="ina700" id="ina700_5V_T">
    <span class="ina700-text">Die Temperature:</span>
    </div>
    <div class="ina700" id="ina700_5V_V">
        <span class="ina700-text">Bus Voltage:</span>
    </div>
    <div class="ina700" id="ina700_5V_A">
        <span class="ina700-text">Bus Current:</span>
    </div>
</div>
`
    document.getElementById('ina700-container').innerHTML = ina700HTML;
});

window.ina700_INT_T = ina700_INT_T;
window.ina700_INT_V = ina700_INT_V;
window.ina700_INT_A = ina700_INT_A;
window.ina700_5V_T = ina700_5V_T;
window.ina700_5V_V = ina700_5V_V;
window.ina700_5V_A = ina700_5V_A;
