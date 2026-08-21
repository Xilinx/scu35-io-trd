/* Copyright (C) 2025, Advanced Micro Devices, Inc. All rights reserved.
SPDX-License-Identifier: MIT */
let currentMaskIdx = 12;

function pushButton(action) {
    const pushBAction = parseInt(action.slice(0, 2).join(''), 16).toString(2).padStart(8, '0').split('').map(Number);
    const squares = document.querySelectorAll('.square');

    // North is pressed:
    if (pushBAction[5] == 1){
        squares[currentMaskIdx].classList.remove('red-mask');
        currentMaskIdx = ((currentMaskIdx - 5) + squares.length) % squares.length;
        squares[currentMaskIdx].classList.add('red-mask');
    }
    // West is pressed:
    if (pushBAction[4] == 1){
        squares[currentMaskIdx].classList.remove('red-mask');
        currentMaskIdx = ((currentMaskIdx % Math.sqrt(squares.length)) == 0) ?
                            currentMaskIdx + (Math.sqrt(squares.length) - 1) :
                            currentMaskIdx - 1;
        squares[currentMaskIdx].classList.add('red-mask');
    }
    // East is pressed:
    if (pushBAction[3] == 1){
        squares[currentMaskIdx].classList.remove('red-mask');
        currentMaskIdx = ((currentMaskIdx % Math.sqrt(squares.length)) == Math.sqrt(squares.length)-1) ?
                            currentMaskIdx - (Math.sqrt(squares.length) - 1) :
                            currentMaskIdx + 1;
        squares[currentMaskIdx].classList.add('red-mask');
    }
    // South is pressed:
    if (pushBAction[6] == 1){
        squares[currentMaskIdx].classList.remove('red-mask');
        currentMaskIdx = (currentMaskIdx + 5) % squares.length;
        squares[currentMaskIdx].classList.add('red-mask');
    }
    // Center is pressed:
    if (pushBAction[7] === 1){
        if (squares[currentMaskIdx].classList.contains('logo-in-square')) {
            squares[currentMaskIdx].classList.remove('logo-in-square');
        } else {
            squares[currentMaskIdx].classList.add('logo-in-square');
        }
    }
}

document.addEventListener('DOMContentLoaded', function() {
    const pushBHTML = `
<div class="grid-pushB">
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square red-mask"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
    <div class="square"></div>
</div>
`
    document.getElementById('pushB-container').innerHTML = pushBHTML;
});

window.pushButton = pushButton;

