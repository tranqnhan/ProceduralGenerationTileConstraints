#pragma once

#include <cstdint>
#include <bit>
#include <vector>

#include "raylib.h"

#include "Ruleset.hpp"


class Generator {
public:
    Generator();
    Image GenerateImage(const Ruleset& rule, int width, int height);

    // prng with xorshift from low to high inclusive
    int RandomInteger(int lower, int upper) {
        randomState ^= randomState << 13;
        randomState ^= randomState >> 17;
        randomState ^= randomState << 5;

        return lower + randomState % (upper - lower + 1);
    }


private:
    uint32_t randomState;

    int GetEntropy(uint32_t constraints) const;
    uint32_t AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const;
    uint32_t ResolveContraints(uint32_t contraints); // Reduce contraints to 1 solution

    bool Contains(const std::vector<int>& openSet, int value); // TODO: only temporary. remove this for heap
};

Generator::Generator() {
    randomState = 12345; //std::time(0);
}

int Generator::GetEntropy(uint32_t constraints) const {
    return std::popcount(constraints);
}

uint32_t Generator::AddConstraints(uint32_t oldContraints, uint32_t newConstraints) const {
    return oldContraints & newConstraints;
}

bool Generator::Contains(const std::vector<int>& openSet, int value) {
    for (int i : openSet) {
        if (i == value) {
            return true;
        }
    }
    return false;
}

uint32_t Generator::ResolveContraints(uint32_t contraints) {
    int totalPossibilities = std::popcount(contraints);

    if (totalPossibilities == 0) return 0;

    int pick = RandomInteger(1, totalPossibilities);

    uint32_t mask = 0b1;

    while (pick > 0) {     
        if (mask & contraints) {
            pick--;
        }
        mask <<= 1;
    }

    mask >>= 1;

    return mask & contraints;
}


Image Generator::GenerateImage(const Ruleset& rules, int width, int height) {
    std::vector<int> openSet; // stores index of next cells to solve
    
    std::vector<bool> explored(width * height, false);
    std::vector<uint32_t> constraints(width * height, ~((~0) << rules.GetNumberOfObjects()));
    
    int startingPoint = RandomInteger(0, width * height - 1);

    openSet.emplace_back(startingPoint);

    while (!openSet.empty()) {
        const int cellIndex = openSet.back();
        openSet.pop_back();

        constraints[cellIndex] = ResolveContraints(constraints[cellIndex]);
        const int type = std::countr_zero(constraints[cellIndex]);
        explored[cellIndex] = true;

        // Get neighbors
        const int cellX = cellIndex % width;
        const int cellY = cellIndex / height;

        const int boundMinY = (cellY - 1) > 0 ? (cellY - 1) : 0;
        const int boundMinX = (cellX - 1) > 0 ? (cellX - 1) : 0;
        const int boundMaxY = (cellY + 2) > height ? height : (cellY + 2);
        const int boundMaxX = (cellX + 2) > width ? width : (cellX + 2);
            
        for (int i = boundMinY; i < boundMaxY; ++i) {
            for (int j = boundMinX; j < boundMaxX; ++j) {
                if (i == 0 && j == 0) continue;
                const int nextIndex = i * width + j;
                if (explored[nextIndex]) continue;
                constraints[nextIndex] = AddConstraints(constraints[nextIndex], rules.GetConstraints(type));
                if (!Contains(openSet, nextIndex)) {
                    openSet.emplace_back(nextIndex);
                }
            }
        }

    }

    Image generateImage = GenImageColor(width, height, WHITE);
    // TODO: Generate image

    return generateImage;

}