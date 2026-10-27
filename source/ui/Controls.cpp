
#include "Controls.hpp"

Controls CONTROLS;

#ifdef PLATFORM_WASM

#include "emscripten.h"


extern "C" {

    EMSCRIPTEN_KEEPALIVE
    void SelectColor(int color) {
        if (CONTROLS.onColorSelected) {
            CONTROLS.onColorSelected(color);
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void ChangeScene() {
        if (CONTROLS.onChangeScene) {
            CONTROLS.onChangeScene();
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void ToggleFill(bool isFill) {
        if (CONTROLS.onToggleFill) {
            CONTROLS.onToggleFill(isFill);
        }
    }

}

#endif