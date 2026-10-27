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

const SCENE_PAINT_SAMPLE_TEXTURE = 0;
const SCENE_GENERATE = 1;

let CURRENT_SCENE = SCENE_PAINT_SAMPLE_TEXTURE;

function switchScene() {
    if (CURRENT_SCENE == SCENE_GENERATE) {
        CURRENT_SCENE = SCENE_PAINT_SAMPLE_TEXTURE;
        document.getElementById("paintSampleTextureScene").style.display = "block";
        document.getElementById("generateScene").style.display = "none";
    
    } else if (CURRENT_SCENE == SCENE_PAINT_SAMPLE_TEXTURE) {
        CURRENT_SCENE = SCENE_GENERATE;
        document.getElementById("paintSampleTextureScene").style.display = "none";
        document.getElementById("generateScene").style.display = "block";
    }
}


function onChangeScene() {
    Module.ccall('ChangeScene', null, [], []);
    switchScene();
}

function toggleFill(is_fill) {
    if (is_fill) {
        document.getElementById("fillTool").style.background = "var(--color-red)";
        document.getElementById("pencilTool").style.background = "var(--color-white)";
    } else {
        document.getElementById("pencilTool").style.background = "var(--color-red)";
        document.getElementById("fillTool").style.background = "var(--color-white)";
    }
    Module.ccall('ToggleFill', null, ['boolean'], [is_fill]);
}

toggleFill(false);

window.onSwatchClick = onSwatchClick;
window.onChangeScene = onChangeScene;
window.toggleFill = toggleFill;