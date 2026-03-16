#include <cstdint>

#include "Ruleset.hpp"


Tile::Tile() {
    this->globalFrequency = 0;
}


Tile::Tile(std::vector<uint32_t>&& color, int globalFrequency) {
    this->color = std::move(color);
    this->globalFrequency = globalFrequency;
}


void Tile::SetColor(std::vector<uint32_t>&& color) {
    this->color = std::move(color);
}


const std::vector<uint32_t>& Tile::GetColor() const {
    return this->color;
}


int Tile::GetGlobalFrequency() const {
    return this->globalFrequency;
}

void Tile::SetGlobalFrequency(int globalFrequency) {
    this->globalFrequency = globalFrequency;
}


void Tile::SetAdjacentTiles(std::vector<uint64_t>&& adjacentTilesInDirection, int direction) {
    this->adjacentTiles[direction] = std::move(adjacentTilesInDirection);
}


const std::vector<uint64_t>& Tile::GetAdjacentTiles(int direction) const {
    return this->adjacentTiles[direction];
}

//TODO: remove temporary constructor
Ruleset::Ruleset() {
    
}


Ruleset::Ruleset(int numTiles) {
    this->tiles.resize(numTiles);

    this->numTile64Sets = int(numTiles / 64) + (numTiles % 64 > 0 ? 1 : 0);
}


int Ruleset::GetNumberOfTiles() const {
    return this->tiles.size();
}


const std::vector<Tile>& Ruleset::GetTiles() const {
    return this->tiles;
}


void Ruleset::SetTileColor(int tileId, std::vector<uint32_t>&& color) {
    this->tiles[tileId].SetColor(std::move(color));
}


void Ruleset::SetTileFrequency(int tileId, int globalFrequency) {
    this->tiles[tileId].SetGlobalFrequency(globalFrequency);
}


int Ruleset::GetTileFrequency(int tileId) const {
    return this->tiles[tileId].GetGlobalFrequency();
}


void Ruleset::SetAdjacentTiles(int tileId, int direction, const std::vector<int>& adjacentTileIds, const std::vector<int>& adjacentTileFrequencies) {
    std::vector<uint64_t> adjacentTiles(this->numTile64Sets);

    for (const int tileId : adjacentTileIds) {
        const int valueIndex = int(tileId / 64);
        const int bitIndex = tileId % 64;
        adjacentTiles[valueIndex] |= (uint64_t(1) << (63 - bitIndex));
    } 
    
    this->tiles[tileId].SetAdjacentTiles(std::move(adjacentTiles), direction);
}

const Tile& Ruleset::GetTile(int tileId) const {
    return this->tiles[tileId];
}


const std::vector<uint64_t>& Ruleset::GetAdjacentTiles(int tileId, int direction) const {
    return this->tiles[tileId].GetAdjacentTiles(direction);
}


const std::vector<uint32_t>& Ruleset::GetTileColor(int tileId) const {
    return this->tiles[tileId].GetColor();
}


int Ruleset::GetTile64Sets() const {
    return this->numTile64Sets;
}
