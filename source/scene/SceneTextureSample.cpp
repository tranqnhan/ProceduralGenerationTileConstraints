#include <raylib.h>
#include <vector>

#include "Program.hpp"
#include "SceneTextureSample.hpp"
#include "SceneCoordinator.hpp"
#include "SceneSharedData.hpp"
#include "ColorPallete.hpp"
#include "Controls.hpp"

SceneTextureSample::SceneTextureSample() {
    this->sampleTexture = LoadTextureFromImage(SceneSharedData::sampleImage);

    for (int i = 0; i < NUM_COLOR; ++i) {
        this->colors[i] = GetColor(COLOR_PALLETE[i]);
    }

    this->currentColorId = 0;

    this->isPencil = false;
    this->isFill = false;

    this->toggleTool = false;

    this->uiCursor.Init();


    CONTROLS.onColorSelected = [this](int value) {
        if (value >= NUM_COLOR) value = NUM_COLOR - 1;
        if (value < 0) value = 0;
        
        this->currentColorId = value;
    };

    CONTROLS.onToggleFill = [this](bool isFill) {
        this->toggleTool = isFill;
    };

}


void SceneTextureSample::Fill() {
    int gridX = this->pointerX / SAMPLE_PIXEL_SCALE;
    int gridY = this->pointerY / SAMPLE_PIXEL_SCALE;

    Color *sampleColors = (Color *)SceneSharedData::sampleImage.data;
    const Color mainColor = sampleColors[gridY * SAMPLE_WIDTH + gridX];

    if (mainColor.r == this->colors[this->currentColorId].r &&
        mainColor.g == this->colors[this->currentColorId].g &&
        mainColor.b == this->colors[this->currentColorId].b
    ) {
        return;
    }

    std::vector<int> positions;

    positions.emplace_back(gridY * SAMPLE_WIDTH + gridX);

    constexpr int DIR_X[4] = { 0, 0, 1, -1};
    constexpr int DIR_Y[4] = { 1, -1, 0, 0};

    while(positions.size() > 0) {
        const int coords = positions.back();

        sampleColors[coords] = this->colors[this->currentColorId];

        positions.pop_back();

        gridX = coords % SAMPLE_WIDTH;
        gridY = coords / SAMPLE_WIDTH;

        for (int i = 0; i < 4; ++i) {
            const int nextGridX = gridX + DIR_X[i];
            const int nextGridY = gridY + DIR_Y[i];
            const int nextCoords = nextGridY * SAMPLE_WIDTH + nextGridX;
            if (nextGridX >= 0 && 
                nextGridX < SAMPLE_WIDTH && 
                nextGridY >= 0 && 
                nextGridY < SAMPLE_HEIGHT &&
                sampleColors[nextCoords].r == mainColor.r &&
                sampleColors[nextCoords].g == mainColor.g &&
                sampleColors[nextCoords].b == mainColor.b
            ) {
                positions.emplace_back(nextCoords);
            }
        }
    }

    UpdateTexture(this->sampleTexture, SceneSharedData::sampleImage.data);
}


void SceneTextureSample::OnEnter() {
    CONTROLS.onChangeScene = []() {
        SceneCoordinator::ChangeScene(SceneId::Generation);
    };

}


void SceneTextureSample::Input() {
    #ifndef PLATFORM_WASM
    if (IsKeyPressed(KEY_ENTER)) {
        SceneCoordinator::ChangeScene(SceneId::Generation);
        return;
    }
    #endif

    this->pointerX = GetMouseX();
    this->pointerY = GetMouseY();
    
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (toggleTool) { // Fill tool
            this->isFill = true;
        } else {
            this->isPencil = true;
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (toggleTool) {
            this->isFill = false;
        } else {
            this->isPencil = false;
        }
    }

    if (IsKeyPressed(KEY_F)) {
        this->isFill = true;
    }

    int key = GetKeyPressed();

    switch(key) {
        case KEY_ZERO: this->currentColorId = 0; break;
        case KEY_ONE: this->currentColorId = 1; break;
        case KEY_TWO: this->currentColorId = 2; break;
        case KEY_THREE: this->currentColorId = 3; break;
        case KEY_FOUR: this->currentColorId = 4; break;
        case KEY_FIVE: this->currentColorId = 5; break;
        case KEY_SIX: this->currentColorId = 6; break;
        case KEY_SEVEN: this->currentColorId = 7; break;
        case KEY_EIGHT: this->currentColorId = 8; break;
        case KEY_NINE: this->currentColorId = 9; break;
        default: ;
    }

    this->uiCursor.Input();
}


void SceneTextureSample::Update() {
    if (this->isPencil) {
        const int gridX = this->pointerX / SAMPLE_PIXEL_SCALE;
        const int gridY = this->pointerY / SAMPLE_PIXEL_SCALE;

        ImageDrawPixel(&SceneSharedData::sampleImage, gridX, gridY, this->colors[this->currentColorId]);
        UpdateTexture(this->sampleTexture, SceneSharedData::sampleImage.data);
    }

    if (this->isFill) {
        this->Fill();
        this->isFill = false;
    }

    this->uiCursor.Update();
}



void SceneTextureSample::Render() {
    DrawTextureEx(
        this->sampleTexture, 
        Vector2{.x = 0, .y = 0},
        0, 
        SAMPLE_PIXEL_SCALE, 
        WHITE);

    const Color color = this->colors[this->currentColorId];

    DrawRectangle(
        int(this->pointerX / SAMPLE_PIXEL_SCALE) * SAMPLE_PIXEL_SCALE, 
        int(this->pointerY / SAMPLE_PIXEL_SCALE) * SAMPLE_PIXEL_SCALE,
        SAMPLE_PIXEL_SCALE,
        SAMPLE_PIXEL_SCALE,
        Color {color.r, color.g, color.b, 170}
    );

    this->uiCursor.Render();
}


void SceneTextureSample::OnExit() {
    CONTROLS.onChangeScene = nullptr;
}