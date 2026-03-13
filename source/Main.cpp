#include <chrono>
#include <iostream>

#include "raylib.h"

#include "XorshiftRandom.hpp"
#include "Processor.hpp"
#include "Generator.hpp"

#define WINDOW_W 800
#define WINDOW_H 800
#define WINDOW_N "Procedural Generation Constraint Satisfaction"


uint32_t XorshiftRandom::randomState = 2222;


Generator generator;

// Main loop initialization
void Init() {
    InitWindow(WINDOW_W, WINDOW_H, WINDOW_N);
    SetTargetFPS(60);


    Processor sampleProcessor;

    auto start = std::chrono::steady_clock::now(); 

    Ruleset ruleset = sampleProcessor.AnalyzeImage("../assets/sample4.png", 3);

    auto stop = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Analyze image time: " << duration.count() << " microseconds" << std::endl;
    
    generator.Init(ruleset, 100, 100);
}


// Main loop update
void Update(float deltaTime) {
    const int numIterations = 10;
    for (int i = 0; i < numIterations; ++i) {
        generator.Next();
    }
}


// Main loop input
void Input() {
    if (IsKeyPressed(KEY_SPACE)) {
        generator.Next();
    }
}


// Main loop draw
void Render() {
    BeginDrawing();
    ClearBackground(BLACK);


    // Draw
    DrawTextureEx(generator.texture, Vector2{.x = 0, .y = 0}, 0, 8, WHITE);
    
    DrawFPS(0, 0);

    EndDrawing();
}


void OnClose() {
    //UnloadTexture(generator.debugTexture);
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