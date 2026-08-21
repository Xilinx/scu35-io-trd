/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
function accel_X(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('accel_X').querySelector('.accel-text').textContent = `X-Axis: ${floatVal.toFixed(3)} mg`;
}

function accel_Y(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('accel_Y').querySelector('.accel-text').textContent = `Y-Axis: ${floatVal.toFixed(3)} mg`;
}

function accel_Z(action) {
    const int32Val = parseInt(action.slice(0, 8).join(''), 16);
    const buff = new ArrayBuffer(4);
    const dataView = new DataView(buff);
    dataView.setInt32(0, int32Val);
    const floatVal = dataView.getFloat32(0);

    document.getElementById('accel_Z').querySelector('.accel-text').textContent = `Z-Axis: ${floatVal.toFixed(3)} mg`;
}


document.addEventListener('DOMContentLoaded', function() {
    const accelHTML = `
<div class="accel" id="accel_X">
    <span class="accel-text">X-Axis:</span>
</div>
<div class="accel" id="accel_Y">
    <span class="accel-text">Y-Axis:</span>
</div>
<div class="accel" id="accel_Z">
    <span class="accel-text">Z-Axis:</span>
</div>
`
    document.getElementById('accel-container').innerHTML = accelHTML;
});

window.accel_X = accel_X;
window.accel_Y = accel_Y;
window.accel_Z = accel_Z;