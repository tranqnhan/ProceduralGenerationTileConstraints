#pragma once

#include <raylib.h>

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
    Color colors[10];

    int currentColorId;
    int pointerX;
    int pointerY;
    bool isDraw;
    bool isFill;

    Cursor uiCursor;

    void Fill();
};