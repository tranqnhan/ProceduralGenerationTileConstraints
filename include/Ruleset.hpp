#pragma once

#include <cstdint>
#include <vector>
#include <array>


enum TileDirection {
    NORTH,
    SOUTH,
    WEST,
    EAST,
    NUM_DIRECTIONS
};


class AdjacentTile {
public:
    AdjacentTile();
    AdjacentTile(int id, int localFrequency);

    int GetTileId() const;
    int GetLocalFrequency() const;
private:
    int id;
    int localFrequency;
};


class Tile {
public:
    Tile();
    Tile(uint32_t color, int globalFrequency);
    
    void SetAdjacentTiles(std::vector<AdjacentTile>&& adjacentTiles, TileDirection direction);
    const std::vector<AdjacentTile>& GetAdjacentTiles(TileDirection direction) const;

    uint32_t GetColor() const;
    void SetColor(uint32_t color);

    int GetGlobalFrequency() const;
    void SetGlobalFrequency(int globalFrequency);

private:
    uint32_t color;
    std::array<std::vector<AdjacentTile>, TileDirection::NUM_DIRECTIONS> adjacentTiles;
    int globalFrequency;
};


class Ruleset {
public:
    Ruleset();
    Ruleset(int numTiles);

    void SetAdjacentTiles(int tileId, TileDirection direction, const std::vector<int>& adjacentTileIds, const std::vector<int>& adjacentTileFrequencies);

    const Tile& GetTile(int tileId) const;
    const std::vector<AdjacentTile>& GetAdjacentTiles(int tileId, TileDirection direction) const;
    const uint32_t GetTileColor(int tileId) const;
    int GetNumTiles() const;
    const std::vector<Tile>& GetTiles() const;
    void SetTileFrequency(int tileId, int globalFrequency);
    void SetTileColor(int tileId, uint32_t color);

private:
    std::vector<Tile> tiles;
};
