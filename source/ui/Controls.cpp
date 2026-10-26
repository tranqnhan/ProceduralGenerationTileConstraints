#include "raylib.h"
#include "emscripten.h"

static int backgroundColor = 0;

extern "C" {

    EMSCRIPTEN_KEEPALIVE
    void setBackgroundColor(int color)
    {
        backgroundColor = color;
    }

    EMSCRIPTEN_KEEPALIVE
    void resetGame()
    {
        // Reset your game state here.
    }

}
