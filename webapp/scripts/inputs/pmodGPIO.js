/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
function pmodGPIO(action) {
    const binAction = parseInt(action.slice(0, 8).join(''), 16).toString(2);
    const flipAction = binAction.split('').reverse().join('');

    for (let i = 0; i < 32; i++) {
        const pmod = document.getElementById(`pmod${i}`);

        if (flipAction[i] === '1') {
            pmod.classList.add('on');
        } else {
            pmod.classList.remove('on');
        }
    }
}

document.addEventListener('DOMContentLoaded', function() {
    const pmodHTML = `
<div class="pmod-group">
    <span class="pmodH">PMOD 1</span>
    <div class="pmod" id="pmod0">
        <span class="pmod-text">Pin 1</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod1">
        <span class="pmod-text">Pin 2</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod2">
        <span class="pmod-text">Pin 3</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod3">
        <span class="pmod-text">Pin 4</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod4">
        <span class="pmod-text">Pin 5</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod5">
        <span class="pmod-text">Pin 6</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod6">
        <span class="pmod-text">Pin 7</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod7">
        <span class="pmod-text">Pin 8</span>
        <div class="pmod-circle"></div>
    </div>
</div>
<div class="pmod-group">
    <span class="pmodH">PMOD 2</span>
    <div class="pmod" id="pmod8">
        <span class="pmod-text">Pin 1</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod9">
        <span class="pmod-text">Pin 2</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod10">
        <span class="pmod-text">Pin 3</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod11">
        <span class="pmod-text">Pin 4</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod12">
        <span class="pmod-text">Pin 5</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod13">
        <span class="pmod-text">Pin 6</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod14">
        <span class="pmod-text">Pin 7</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod15">
        <span class="pmod-text">Pin 8</span>
        <div class="pmod-circle"></div>
    </div>
</div>
<div class="pmod-group">
    <span class="pmodH">PMOD 3</span>
    <div class="pmod" id="pmod16">
        <span class="pmod-text">Pin 1</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod17">
        <span class="pmod-text">Pin 2</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod18">
        <span class="pmod-text">Pin 3</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod19">
        <span class="pmod-text">Pin 4</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod20">
        <span class="pmod-text">Pin 5</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod21">
        <span class="pmod-text">Pin 6</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod22">
        <span class="pmod-text">Pin 7</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod23">
        <span class="pmod-text">Pin 8</span>
        <div class="pmod-circle"></div>
    </div>
</div>
<div class="pmod-group">
    <span class="pmodH">PMOD 4</span>
    <div class="pmod" id="pmod24">
        <span class="pmod-text">Pin 1</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod25">
        <span class="pmod-text">Pin 2</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod26">
        <span class="pmod-text">Pin 3</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod27">
        <span class="pmod-text">Pin 4</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod28">
        <span class="pmod-text">Pin 5</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod29">
        <span class="pmod-text">Pin 6</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod30">
        <span class="pmod-text">Pin 7</span>
        <div class="pmod-circle"></div>
    </div>
    <div class="pmod" id="pmod31">
        <span class="pmod-text">Pin 8</span>
        <div class="pmod-circle"></div>
    </div>
</div>
`
    document.getElementById('pmod-container').innerHTML = pmodHTML;
});

window.pmodGPIO = pmodGPIO;

