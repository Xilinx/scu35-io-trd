/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
function DIPSwitches(action) {
    const binAction = parseInt(action.slice(0, 4).join(''), 16).toString(2).padStart(16, '0');
    const flipAction = binAction.split('').reverse().join('');

    for (let i = 0; i < 8; i++) {
        const dip = document.getElementById(`dip${i}`);

        if (flipAction[i] === '1') {
            dip.classList.add('on');
        } else {
            dip.classList.remove('on');
        }
    }
}

document.addEventListener('DOMContentLoaded', function() {
    const dipHTML = `
<div class="dip" id="dip0">
    <span class="dip-text">Dip Switch 0</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip1">
    <span class="dip-text">Dip Switch 1</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip2">
    <span class="dip-text">Dip Switch 2</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip3">
    <span class="dip-text">Dip Switch 3</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip4">
    <span class="dip-text">Dip Switch 4</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip5">
    <span class="dip-text">Dip Switch 5</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip6">
    <span class="dip-text">Dip Switch 6</span>
    <div class="dip-circle"></div>
</div>
<div class="dip" id="dip7">
    <span class="dip-text">Dip Switch 7</span>
    <div class="dip-circle"></div>
</div>
`
    document.getElementById('DIPs-container').innerHTML = dipHTML;
});

window.DIPSwitches = DIPSwitches;

