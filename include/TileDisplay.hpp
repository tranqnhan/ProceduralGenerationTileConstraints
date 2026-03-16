#pragma once

#include <raylib.h>

#include "Generator.hpp"

class TileDisplay {
public:
    TileDisplay();

    void Init();
    void Input();
    void Update(const Generator& generator);
    void Render();

    ~TileDisplay();

private:
    Texture2D tileTexture;
    Image tileImage;

    Color pointerColor;

    int pointerX;
    int pointerY;

    bool uiSelectCell;
    int uiSelectSameCellCount;
    int uiSelectCellCoords;
    int uiSelectTileId;
};