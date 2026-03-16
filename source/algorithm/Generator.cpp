#include <cstdint>
#include <vector>

#include <raylib.h>

#include "BitMath.hpp"
#include "Heap.hpp"
#include "Program.hpp"
#include "XorshiftRandom.hpp"
#include "CompressColor.hpp"
#include "Ruleset.hpp"
#include "Generator.hpp"


Cell::Cell(const Ruleset& ruleset) {
    this->tilePossibilities.resize(ruleset.GetTile64Sets(), ~uint64_t(0));

    uint64_t& tileSet = this->tilePossibilities.back();
    tileSet <<= 64 - (ruleset.GetNumberOfTiles() % 64);

    this->globalFrequency = 0;
    this->resultTileId = SpecialCellType::Unexplored;
    this->numberOfPossibleTiles = ruleset.GetNumberOfTiles();
}


bool Cell::Intersect(const std::vector<uint64_t>& otherPossibilities) {
    int changedIndex = -1;
    
    for (int i = 0; i < otherPossibilities.size(); ++i) {
        const uint64_t mask = this->tilePossibilities[i] & otherPossibilities[i];
        if (this->tilePossibilities[i] != mask) {
            this->tilePossibilities[i] = mask;
            changedIndex = i + 1;
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


int Cell::Collapse(const Ruleset& ruleset) {
    this->resultTileId = SpecialCellType::NoSolution;

    if (this->numberOfPossibleTiles == 0) return this->resultTileId;

    const std::vector<int> tileIds = BitMath::GetSetPositions(this->tilePossibilities);

    std::vector<int> globalFrequencies(tileIds.size());

    int sumGlobalFrequencies = 0;
    for (int i = 0; i < tileIds.size(); ++i) {
        sumGlobalFrequencies += ruleset.GetTileFrequency(tileIds[i]);
        globalFrequencies[i] = sumGlobalFrequencies;
    }

    int result = XorshiftRandom::RandomInteger(0, sumGlobalFrequencies);
    
    for (int i = 0; i < globalFrequencies.size(); ++i) {
        if (result <= globalFrequencies[i]) {
            this->resultTileId = tileIds[i];
            break;
        }
    }

    return this->resultTileId;
}


const std::vector<uint64_t>& Cell::GetTilePossibilities() const {
    return this->tilePossibilities;
}

    
int Cell::GetEntropy() const {
    return this->numberOfPossibleTiles;
}


int Cell::GetResultTile() const {
    return this->resultTileId;
}


Generator::Generator() {
    this->failed = false;
}


void Generator::Init(const Ruleset& rules, int chunkWidth, int chunkHeight, int numChunkWidth, int numChunkHeight) {
    this->ruleset = rules;
    this->width = chunkWidth * numChunkWidth;
    this->height = chunkHeight * numChunkHeight;

    this->image = GenImageColor(this->width, this->height, BLACK);
    this->texture = LoadTextureFromImage(this->image);

    const Cell initialCell( this->ruleset);

    this->cells = std::vector<Cell>(this->width * this->height, initialCell);

    const int initialCoords = XorshiftRandom::RandomInteger(0, this->width * this->height - 1);

    this->cellEntropyPriorityQueue.Push(0, initialCoords);
}


void Generator::Next() {
    if (this->cellEntropyPriorityQueue.GetSize() <= 0 || this->failed) {
        return;
    }

    const int currentCoordinates = this->cellEntropyPriorityQueue.TopItemID();
    this->cellEntropyPriorityQueue.Pop();

    const int resultTileId = this->cells[currentCoordinates].Collapse(this->ruleset);

    if (resultTileId == SpecialCellType::NoSolution) {
        const int cellX = currentCoordinates % width;
        const int cellY = currentCoordinates / height;

        ImageDrawPixel(&image, cellX, cellY, RED);
        UpdateTexture(texture, image.data);
        this->failed = true;

        return;
    }

    const int cellX = currentCoordinates % width;
    const int cellY = currentCoordinates / width;

    const Tile& tile = this->ruleset.GetTile(resultTileId);
    const uint32_t compressedColor = tile.GetColor()[0];

    ImageDrawPixel(&image, cellX, cellY, CompressColor::Decompress(compressedColor));
    UpdateTexture(texture, image.data);

    // Propagation
    CompletePropagation(currentCoordinates);
}


void Generator::CompletePropagation(int beginCoordinates) {
    std::vector<int> queueCoordinates;
    std::vector<bool> isInQueue(this->width * this->height);

    queueCoordinates.emplace_back(beginCoordinates);
    
    while (!queueCoordinates.empty() && !this->failed) {
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

    if (adjacentCell.GetResultTile() >= SpecialCellType::NoSolution) return;

    const int cellSolvedTiled = cell.GetResultTile();

    bool changes;

    if (cellSolvedTiled < 0) {
        std::vector<uint64_t> adjacentTilesUnion(this->ruleset.GetTile64Sets(), 0);

        const std::vector<int> tileIds = BitMath::GetSetPositions(cell.GetTilePossibilities());
        
        for (const int tileId : tileIds) {
            const Tile& tile = this->ruleset.GetTile(tileId);
            const std::vector<uint64_t>& adjacentTiles = tile.GetAdjacentTiles(direction);

            for (int k = 0; k < adjacentTiles.size(); ++k) {
                adjacentTilesUnion[k] |= adjacentTiles[k];
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
    
    if (entropy == 0) {
        this->failed = true;
        return;
    }
    
    this->cellEntropyPriorityQueue.Push(entropy, adjacentCoordinates);


}


int Generator::GetCellTileId(int coordinates) const {
    if (coordinates >= this->cells.size()) return SpecialCellType::Unexplored;
    return this->cells[coordinates].GetResultTile();
}


const Cell& Generator::GetCell(int coordinates) const {
    return this->cells[coordinates];
}


void Generator::Render() {
    DrawTextureEx(this->texture, Vector2{.x = 0, .y = 0}, 0, PIXEL_SCALE, WHITE);
}


const Ruleset& Generator::GetRuleset() const {
    return this->ruleset;
}


int Generator::GetWidth() const {
    return this->width;
}


int Generator::GetHeight() const {
    return this->height;
}


Generator::~Generator() {
    UnloadImage(this->image);
    UnloadTexture(this->texture);
}