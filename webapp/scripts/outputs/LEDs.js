/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
let switchesValue = '0';

function LEDs_init() {
    for (let i = 14; i >= 0; i--) {
        document.getElementById(`led${i}`).addEventListener('change', LEDs);

        if(document.getElementById(`led${i}`).checked){
            switchesValue += '1';
        } else {
            switchesValue += '0';
        }
    }
    LEDs_sendState();
}

function LEDs() {
    let newState = '0';
    for (let i = 14; i >= 0; i--) {
        if(document.getElementById(`led${i}`).checked) {
            newState += '1';
        } else {
            newState += '0';
        }
    }
    if (newState != switchesValue) {
        switchesValue = newState;
        LEDs_sendState();
    }
}

function LEDs_sendState() {
    let payload = Array(16).fill(';');
    payload[0] = 'l';
    payload[1] = 'e';
    payload[2] = 'd';
    payload[3] = '0';
    payload[4] = parseInt(switchesValue.slice(0,4), 2).toString(16).charAt(0);
    payload[5] = parseInt(switchesValue.slice(4,8), 2).toString(16).charAt(0);
    payload[6] = parseInt(switchesValue.slice(8,12), 2).toString(16).charAt(0);
    payload[7] = parseInt(switchesValue.slice(12,16), 2).toString(16).charAt(0);

    sendToSerialPort(payload);

}

window.LEDs_init = LEDs_init;

document.addEventListener('DOMContentLoaded', function() {
    const switchHTML = `
<div class="led-group">
    <span class="ledGroup-text">LED 0</span>
    <div class="switch">
        <span class="switch-text">R</span>
        <label for="led0">
            <input type="checkbox" id="led0" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">G</span>
        <label for="led1">
            <input type="checkbox" id="led1" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">B</span>
        <label for="led2">
            <input type="checkbox" id="led2" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
</div>
<div class="led-group">
    <span class="ledGroup-text">LED 1</span>
    <div class="switch">
        <span class="switch-text">R</span>
        <label for="led3">
            <input type="checkbox" id="led3" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">G</span>
        <label for="led4">
            <input type="checkbox" id="led4" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">B</span>
        <label for="led5">
            <input type="checkbox" id="led5" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
</div>
<div class="led-group">
    <span class="ledGroup-text">LED 2</span>
    <div class="switch">
        <span class="switch-text">R</span>
        <label for="led6">
            <input type="checkbox" id="led6" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">G</span>
        <label for="led7">
            <input type="checkbox" id="led7" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">B</span>
        <label for="led8">
            <input type="checkbox" id="led8" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
</div>
<div class="led-group">
    <span class="ledGroup-text">LED 3</span>
    <div class="switch">
        <span class="switch-text">R</span>
        <label for="led9">
            <input type="checkbox" id="led9" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">G</span>
        <label for="led10">
            <input type="checkbox" id="led10" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">B</span>
        <label for="led11">
            <input type="checkbox" id="led11" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
</div>
<div class="led-group">
    <span class="ledGroup-text">LED 4</span>
    <div class="switch">
        <span class="switch-text">R</span>
        <label for="led12">
            <input type="checkbox" id="led12" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">G</span>
        <label for="led13">
            <input type="checkbox" id="led13" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
    <div class="switch">
        <span class="switch-text">B</span>
        <label for="led14">
            <input type="checkbox" id="led14" class="switch-input" />
            <span class="switch-slider"></span>
        </label>
    </div>
</div>
`
    document.getElementById('LEDs-container').innerHTML = switchHTML;
});