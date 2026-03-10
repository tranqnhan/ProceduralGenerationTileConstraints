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
    int Solve() const;
    int GetGlobalFrequency() const;

private:
    std::vector<AdjacentTile> tilePossibilities;
    int globalFrequency;
};


class Generator {
public:

    Generator();

    Image GenerateImage(const Ruleset& rule, int width, int height);

    static uint32_t randomState;
    // prng with xorshift from low to high inclusive
    static int RandomInteger(int lower, int upper) {
        randomState ^= randomState << 13;
        randomState ^= randomState >> 17;
        randomState ^= randomState << 5;

        return lower + randomState % (upper - lower + 1);
    }

    // TODO: for debug purposes
    Image debugImage;
    Texture2D debugTexture;
    Ruleset debugRules;
    int debugWidth; 
    int debugHeight;

    std::vector<bool> debugExplored;
    std::vector<int> debugCellsIndex;
    std::vector<Cell> debugCells;

    Heap<int> debugOpenSet = Heap<int>([this](const int& a, const int& b) -> bool {
        const Cell& cellA = this->debugCells[this->debugCellsIndex[a]];
        const Cell& cellB = this->debugCells[this->debugCellsIndex[b]];
        
        return cellA.GetGlobalFrequency() > cellB.GetGlobalFrequency();
    }); // stores index of next cells to solve

    void DebugInit(const Ruleset& rules, int width, int height);
    void DebugNext();
    void DebugPropagation(const Tile& tile, int coords);
    void DebugExpandAdjacent(int x, int y, TileDirection direction, const Tile& tile);


private:
    int GetEntropy(uint32_t constraints) const;
    uint32_t AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const;
    uint32_t ResolveContraints(uint32_t contraints); // Reduce contraints to 1 solution 

};
