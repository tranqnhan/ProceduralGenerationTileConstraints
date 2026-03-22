#pragma once

#include <raylib.h>

class Cursor {
public:
    Cursor();

    void Init();
    void Input();
    void Update();
    void Render();

    ~Cursor();
    
private:
    int positionX;
    int positionY;

    Texture2D cursorTexture;
};