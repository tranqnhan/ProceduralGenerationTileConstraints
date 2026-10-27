#pragma once

#include <raylib.h>

#include "ColorPallete.hpp"
#include "Scene.hpp"
#include "Cursor.hpp"




class SceneTextureSample : public Scene {
public:
    SceneTextureSample();
    
    void OnEnter();
    void Input();
    void Update();
    void Render();
    void OnExit();
    
private:
    Texture2D sampleTexture;
    Color colors[NUM_COLOR];

    int currentColorId;
    int pointerX;
    int pointerY;
    bool isPencil;
    bool isFill;

    bool toggleTool;

    Cursor uiCursor;

    void Fill();
};