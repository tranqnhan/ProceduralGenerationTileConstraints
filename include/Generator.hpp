#pragma once

#include <cstdint>
#include <vector>

#include <raylib.h>

#include "Heap.hpp"
#include "Ruleset.hpp"


enum SpecialCellType {
    NoSolution = -1,
    Unexplored = -2
};


class Cell {
public:
    Cell(const Ruleset& ruleset);

    bool Intersect(const std::vector<uint64_t>& otherPossibilities);
    int Collapse(const Ruleset& ruleset);
    int GetEntropy() const;
    int GetResultTile() const;

    const std::vector<uint64_t>& GetTilePossibilities() const;

private:
    std::vector<uint64_t> tilePossibilities;
    int globalFrequency;
    int resultTileId;
    int numberOfPossibleTiles;
};


class Generator {
public:
    Generator();

    void Init(const Ruleset& rules, int chunkWidth, int chunkHeight, int numChunkWidth, int numChunkHeight);
    void Next();

    int GetCellTileId(int coordinates) const;
    const Cell& GetCell(int coordinates) const;
    const Ruleset& GetRuleset() const;
    int GetWidth() const;
    int GetHeight() const;

    void Render();

    ~Generator();
    
private:
    Image image;
    Texture2D texture;
    Ruleset ruleset;
    int width; 
    int height;

    int chunkWidth;
    int chunkHeight;
    int numChunkWidth;
    int numChunkHeight;

    bool failed;

    std::vector<Cell> cells;

    Heap<int> cellEntropyPriorityQueue = Heap<int>([this](const int& entropyA, const int& entropyB) -> bool {        
       return entropyA <= entropyB;
    }); // stores index of next cells to solve

    void CompletePropagation(int beginCoordinates);
    void Propagate(int coordinates, std::vector<int>& queueCoordinates, std::vector<bool>& isInQueue);
    void ExpandAdjacent(int adjacentCoordinates, TileDirection direction, const Cell& cell, std::vector<int>& queueCoordinate, std::vector<bool>& isInQueue);
};
