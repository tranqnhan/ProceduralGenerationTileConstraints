#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include "ankerl/unordered_dense.h"

#include "Ruleset.hpp"

class Kernel {
public:
    std::vector<uint32_t> leafs;
    std::array<std::vector<uint32_t>, TileDirection::NUM_DIRECTIONS> overlaps;

    int globalFrequency;
    std::array<ankerl::unordered_dense::map<int, int>, TileDirection::NUM_DIRECTIONS> adjacentKernelFrequencies;

    Kernel(std::vector<uint32_t>&& leafs);

    void AddAdjacency(int adjacentKernelId, int direction);
    bool CompareAdjacentOverlap(const Kernel& otherKernel, int direction, int opposeDirection) const;
    const std::vector<uint32_t>& GetAdjacentOverlap(int direction) const;
};

