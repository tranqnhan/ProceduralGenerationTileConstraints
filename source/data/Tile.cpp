#include "Tile.hpp"

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
