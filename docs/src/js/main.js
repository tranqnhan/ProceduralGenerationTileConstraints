//Import the modularized JS created by Emscripten
import createModule from '../../PGCS.js';

// Wait for the module to finish loading
const Module = await createModule({
    canvas: document.getElementById('canvas')  // pass your canvas
});

console.log("WASM loaded and ready");

// Do NOT call main(); raylib automatically starts the main loop

function onSwatchClick(index, colorHexValue) {
    console.log(`select color ${index} ${colorHexValue}`);
    document.getElementById("selectedColor").style.backgroundColor = colorHexValue;
    
    Module.ccall('SelectColor', // name of C function
        null, // return type
        ['number'], // argument types
        [index]); // arguments
}

window.onSwatchClick = onSwatchClick;