#include "raylib.h"

#include "Analyzer.hpp"

#define WINDOW_W 700
#define WINDOW_H 700
#define WINDOW_N "Procedural Generation Constraint Satisfaction"

Analyzer sampleAnalyzer;

// Main loop initialization
void Init() {
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    SetTargetFPS(60);


    Ruleset ruleset = sampleAnalyzer.AnalyzeImage("../assets/sample1.png");

    for (int i = 0; i < ruleset.GetNumberOfObjects(); ++i) {
        const uint32_t value = ruleset.GetConstraints(i);
    }
}


// Main loop update
void Update(float deltaTime) {

}


// Main loop input
void Input() {

}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLACK);

    DrawFPS(0, 0);

    // Draw

    EndDrawing();
}


void OnClose() {

}

// Main loop
int main(void) {
    Init();

    while (!WindowShouldClose()) {
        Input();
        Update(GetFrameTime());
        Render();
    }

    OnClose();

    CloseWindow();
    return 0;
}