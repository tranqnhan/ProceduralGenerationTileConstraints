#pragma once

#include <cstdint>
#include <vector>

#include "raylib.h"

#include "Heap.hpp"
#include "Ruleset.hpp"

class Cell {
public:
    Cell(const Ruleset& ruleset, const std::vector<AdjacentTile>& initialPossibilities);

    void Intersect(const Ruleset& ruleset, const std::vector<AdjacentTile>& possibilities);
    int Collapse();
    int GetEntropy() const;
    int GetValue() const;

private:
    std::vector<AdjacentTile> tilePossibilities;
    int globalFrequency;
    int tileId;
};


class Generator {
public:
    Generator();

    Image GenerateImage(const Ruleset& rule, int width, int height);

    // TODO: for debug purposes
    Image debugImage;
    Texture2D debugTexture;
    Ruleset debugRules;
    int debugWidth; 
    int debugHeight;

    std::vector<bool> debugExplored;
    std::vector<Cell> debugCells;

    Heap<int> debugOpenSet = Heap<int>([this](const int& entropyA, const int& entropyB) -> bool {        
       return entropyA > entropyB;
    }); // stores index of next cells to solve

    // std::vector<std::pair<int, int>> debugOpenSet;

    void DebugInit(const Ruleset& rules, int width, int height);
    void DebugNext();
    void DebugPropagate(const Tile& tile, int coords);
    void DebugExpandAdjacent(int x, int y, TileDirection direction, const Tile& tile);


private:
    int GetEntropy(uint32_t constraints) const;
    uint32_t AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const;
    uint32_t ResolveContraints(uint32_t contraints); // Reduce contraints to 1 solution 

};
