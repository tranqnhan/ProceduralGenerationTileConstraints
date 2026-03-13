#include <cstdint>
#include <bit>
#include <cstdio>
#include <queue>
#include <vector>

#include "Heap.hpp"
#include "XorshiftRandom.hpp"
#include "raylib.h"

#include "Ruleset.hpp"
#include "Generator.hpp"


Cell::Cell(const Ruleset& ruleset) {
    this->tilePossibilities.resize(int(ruleset.GetNumberOfTiles() / 64) + 1, ~uint64_t(0));
    uint64_t& lastTileSet = this->tilePossibilities.back();
    lastTileSet <<= (64 - (ruleset.GetNumberOfTiles() % 64));

    this->globalFrequency = 0;
    this->solvedTileId = -2;
    this->numberOfPossibleTiles = ruleset.GetNumberOfTiles();
}


bool Cell::Intersect(const std::vector<uint64_t>& otherPossibilities) {
    int changedIndex = -1;
    
    for (int i = 0; i < otherPossibilities.size(); ++i) {
        const uint64_t mask = this->tilePossibilities[i] & otherPossibilities[i];
        if (this->tilePossibilities[i] != mask) {
            this->tilePossibilities[i] = mask;
            changedIndex = i  + 1;
            break;
        }
    }

    if (changedIndex >= 0) {
        for (int i = changedIndex; i < otherPossibilities.size(); ++i) {
            this->tilePossibilities[i] &= otherPossibilities[i];
        }

        this->numberOfPossibleTiles = 0;
        for (const uint64_t& tileSet : this->tilePossibilities) {
            this->numberOfPossibleTiles += std::popcount(tileSet);
        }
    }

    return changedIndex >= 0;
}


int Cell::Collapse() {
    this->solvedTileId = -1;

    if (this->numberOfPossibleTiles == 0) return this->solvedTileId;

    std::vector<int> tilesCount(this->tilePossibilities.size());
    for (int i = 0; i < this->tilePossibilities.size(); ++i) {
        const int tileCount = std::popcount(this->tilePossibilities[i]);
        tilesCount[i] = tileCount;
    }

    int result = XorshiftRandom::RandomInteger(1, this->numberOfPossibleTiles);
    
    for (int i = 0; i < tilesCount.size(); ++i) {
        if (result > tilesCount[i]) {
            result -= tilesCount[i];
        } else {
            int tileCountInSet = 0;
            uint64_t tileSet = this->tilePossibilities[i];
            const uint64_t mask = uint64_t(1) << 63;
            while (result > 0) {
                if (mask & tileSet) {
                    result--;
                }
                tileSet <<= 1;
                tileCountInSet++;
            }
            tileCountInSet--;
            this->solvedTileId = i * 64 + tileCountInSet;
            break;
        }
    }


    return this->solvedTileId;
}


const std::vector<uint64_t>& Cell::GetTilePossibilities() const {
    return this->tilePossibilities;
}

    
int Cell::GetEntropy() const {
    return this->numberOfPossibleTiles;
}


int Cell::GetSolvedTile() const {
    return this->solvedTileId;
}


Generator::Generator() {
     //std::time(0);
}


void Generator::Init(const Ruleset& rules, int width, int height) {
    this->ruleset = rules;
    this->width = width;
    this->height = height;

    this->image = GenImageColor(width, height, BLACK);
    this->texture = LoadTextureFromImage(image);

    const Cell initialCell( this->ruleset);

    this->cells = std::vector<Cell>(width * height, initialCell);

    int initialCoords = XorshiftRandom::RandomInteger(0, width * height - 1);

    this->cellEntropyPriorityQueue.Push(0, initialCoords);
}


void Generator::Next() {
    if (this->cellEntropyPriorityQueue.GetSize() <= 0) {
        //std::printf("END!\n");
        return;
    }

    const int currentCoordinates = this->cellEntropyPriorityQueue.TopItemID();
    this->cellEntropyPriorityQueue.Pop();

    //std::printf("coords %i cell index %i\n", coords, cellIndex);
    const int solvedTileId = this->cells[currentCoordinates].Collapse();
    //std::printf("Solved Tile! %i coords %i\n", solvedTileId, currentCoordinates);

    if (solvedTileId == -1) {
        const int cellX = currentCoordinates % width;
        const int cellY = currentCoordinates / height;

        ImageDrawPixel(&image, cellX, cellY, RED);
        UpdateTexture(texture, image.data);
        //std::printf("No possibilities on cell %i %i\n", cellX, cellY);
        return;
    }

    const int cellX = currentCoordinates % width;
    const int cellY = currentCoordinates / width;

    const Tile& tile = this->ruleset.GetTile(solvedTileId);
    const uint32_t compressedColor = tile.GetColor();
    const Color color = Color{
        .r = (unsigned char)((compressedColor & 0xFF000000) >> 24), 
        .g = (unsigned char)((compressedColor & 0x00FF0000) >> 16),
        .b = (unsigned char)((compressedColor & 0x0000FF00) >> 8),
        .a = (unsigned char)((compressedColor & 0x000000FF))
    };


    ImageDrawPixel(&image, cellX, cellY, color);
    UpdateTexture(texture, image.data);

    // Propagation
    CompletePropagation(currentCoordinates);
}


void Generator::CompletePropagation(int beginCoordinates) {
    std::vector<int> queueCoordinates;
    std::vector<bool> isInQueue(this->width * this->height);

    queueCoordinates.emplace_back(beginCoordinates);
    
    while (!queueCoordinates.empty()) {
        const int currentCoordinates = queueCoordinates.back();
        queueCoordinates.pop_back();

        isInQueue[currentCoordinates] = false;

        const Cell& currentCell = this->cells[currentCoordinates];

        Propagate(currentCoordinates, queueCoordinates, isInQueue);
    }
}

void Generator::Propagate(int coordinates, 
    std::vector<int>& queueCoordinates,
    std::vector<bool>& isInQueue
) {
    const Cell& cell = this->cells[coordinates];

    const int x = coordinates % this->width;
    const int y = coordinates / this->width;

    if (x + 1 < this->width) {
        const int adjacentCoordinates = coordinates + 1;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::EAST, cell, queueCoordinates, isInQueue);
    }

    if (x - 1 >= 0) {
        const int adjacentCoordinates = coordinates - 1;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::WEST, cell, queueCoordinates, isInQueue);
    }

    if (y + 1 < this->height) {
        const int adjacentCoordinates = coordinates + this->width;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::SOUTH, cell, queueCoordinates, isInQueue);
    }

    if (y - 1 >= 0) {
        const int adjacentCoordinates = coordinates - this->width;
        this->ExpandAdjacent(adjacentCoordinates, TileDirection::NORTH, cell, queueCoordinates, isInQueue);
    }
}


void Generator::ExpandAdjacent(int adjacentCoordinates, 
    TileDirection direction, 
    const Cell& cell, 
    std::vector<int>& queueCoordinates,
    std::vector<bool>& isInQueue
) {

    Cell& adjacentCell = this->cells[adjacentCoordinates];

    if (adjacentCell.GetSolvedTile() >= -1) return;

    const int cellSolvedTiled = cell.GetSolvedTile();

    bool changes;

    if (cellSolvedTiled < 0) {
        std::vector<uint64_t> adjacentTilesUnion(int(this->ruleset.GetNumberOfTiles() / 64) + 1, 0);

        constexpr uint64_t mask = uint64_t(1) << 63;
        const std::vector<uint64_t>& tilePossibilities = cell.GetTilePossibilities();

        for (int i = 0; i < tilePossibilities.size(); i++) {
            if (tilePossibilities[i] == 0) continue;
            uint64_t tileSet = tilePossibilities[i];
            //std::printf("tileSt %lb\n", tileSet);
            for (int j = 0; j < 64; ++j) {
                if (mask & tileSet) {
                    const int tileId = i * 64 + j;
                                        
                    const Tile& tile = this->ruleset.GetTile(tileId);
                    const std::vector<uint64_t>& adjacentTiles = tile.GetAdjacentTiles(direction);

                    for (int k = 0; k < adjacentTiles.size(); ++k) {
                        adjacentTilesUnion[k] |= adjacentTiles[k];
                    }
                }

                tileSet <<= 1;
            }

        }

        changes = adjacentCell.Intersect(adjacentTilesUnion);
        
    } else {

        const Tile& tile = ruleset.GetTile(cellSolvedTiled);
        changes = adjacentCell.Intersect(tile.GetAdjacentTiles(direction));
    
    }

    if (changes) {
        if (!isInQueue[adjacentCoordinates]) {
            queueCoordinates.emplace_back(adjacentCoordinates);
            isInQueue[adjacentCoordinates] = true;
        }
    }

    const int entropy = adjacentCell.GetEntropy();
    this->cellEntropyPriorityQueue.Push(entropy, adjacentCoordinates);

}