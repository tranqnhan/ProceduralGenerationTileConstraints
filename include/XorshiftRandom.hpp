#pragma once

#include <cstdint>

class XorshiftRandom {
public:
    static uint32_t randomState;
    // prng with xorshift from low to high inclusive
    static int RandomInteger(int lower, int upper) {
        randomState ^= randomState << 13;
        randomState ^= randomState >> 17;
        randomState ^= randomState << 5;

        return lower + randomState % (upper - lower + 1);
    }
};