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


class Ruleset {
public:
    Ruleset();
    Ruleset(int numTiles);

    void SetAdjacentTiles(int tileId, int direction, const std::vector<int>& adjacentTileIds, const std::vector<int>& adjacentTileFrequencies);

    const Tile& GetTile(int tileId) const;
    const std::vector<uint64_t>& GetAdjacentTiles(int tileId, int direction) const;
    const std::vector<uint32_t>& GetTileColor(int tileId) const;
    int GetNumberOfTiles() const;
    const std::vector<Tile>& GetTiles() const;
    void SetTileFrequency(int tileId, int globalFrequency);
    void SetTileColor(int tileId, std::vector<uint32_t>&& color);
    int GetTileFrequency(int tileId) const;
    int GetTile64Sets() const;

private:
    std::vector<Tile> tiles;
    int numTile64Sets;
};
