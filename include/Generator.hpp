#pragma once

#include <cstdint>
#include <vector>

#include "raylib.h"

#include "Heap.hpp"
#include "Ruleset.hpp"

class Cell {
public:
    Cell(const Ruleset& ruleset);

    bool Intersect(const std::vector<uint64_t>& otherPossibilities);
    int Collapse(const Ruleset& ruleset);
    int GetEntropy() const;
    int GetSolvedTile() const;

    const std::vector<uint64_t>& GetTilePossibilities() const;
    std::vector<int> GetTileIds() const;

private:
    std::vector<uint64_t> tilePossibilities;
    int globalFrequency;
    int resultTileId;
    int numberOfPossibleTiles;
};


class Generator {
public:
    Generator();

    Image image;
    Texture2D texture;
    Ruleset ruleset;
    int width; 
    int height;

    std::vector<Cell> cells;

    Heap<int> cellEntropyPriorityQueue = Heap<int>([this](const int& entropyA, const int& entropyB) -> bool {        
       return entropyA <= entropyB;
    }); // stores index of next cells to solve

    void Init(const Ruleset& rules, int width, int height);
    void Next();

    
private:
    void CompletePropagation(int beginCoordinates);
    void Propagate(int coordinates, std::vector<int>& queueCoordinates, std::vector<bool>& isInQueue);
    void ExpandAdjacent(int adjacentCoordinates, TileDirection direction, const Cell& cell, std::vector<int>& queueCoordinate, std::vector<bool>& isInQueue);

};
