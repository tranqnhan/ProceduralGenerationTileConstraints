#pragma once

#include <cstdint>
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
