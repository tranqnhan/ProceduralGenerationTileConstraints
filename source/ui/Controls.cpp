
#include "Controls.hpp"

Controls CONTROLS;

#ifdef PLATFORM_WASM

#include "emscripten.h"


extern "C" {

    EMSCRIPTEN_KEEPALIVE
    void SelectColor(int color)
    {
        if (CONTROLS.onColorSelected) {
            CONTROLS.onColorSelected(color);
        }
    }

}

#endif