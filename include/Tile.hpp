#pragma once

#include <cstdint>
#include <vector>
#include <array>


enum TileDirection {
    NORTH = 0,
    WEST = 1,
    EAST = 2,
    SOUTH = 3,
    NUM_DIRECTIONS = 4
};


class Tile {
public:
    Tile();
    Tile(std::vector<uint32_t>&& color, int globalFrequency);
    
    void SetAdjacentTiles(std::vector<uint64_t>&& adjacentTilesInDirection, int direction);
    const std::vector<uint64_t>& GetAdjacentTiles(int direction) const;

    const std::vector<uint32_t>& GetColor() const;
    void SetColor(std::vector<uint32_t>&& color);

    int GetGlobalFrequency() const;
    void SetGlobalFrequency(int globalFrequency);

private:
    std::vector<uint32_t> color;
    std::array<std::vector<uint64_t>, TileDirection::NUM_DIRECTIONS> adjacentTiles;
    int globalFrequency;
};
