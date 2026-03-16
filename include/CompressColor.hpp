#pragma once

#include <cstdint>
#include <raylib.h>

class CompressColor {
public:
    static uint32_t Compress(Color color);
    static Color Decompress(uint32_t compressedColor);
};