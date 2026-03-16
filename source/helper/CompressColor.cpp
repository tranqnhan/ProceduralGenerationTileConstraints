#include "CompressColor.hpp"


uint32_t CompressColor::Compress(Color color) {
    return (uint32_t(color.r) << 24) | (uint32_t(color.g) << 16) | (uint32_t(color.b) << 8) | uint32_t(color.a);
}


Color CompressColor::Decompress(uint32_t compressedColor) {
    const Color color = Color{
        .r = (unsigned char)((compressedColor & 0xFF000000) >> 24), 
        .g = (unsigned char)((compressedColor & 0x00FF0000) >> 16),
        .b = (unsigned char)((compressedColor & 0x0000FF00) >> 8),
        .a = (unsigned char)((compressedColor & 0x000000FF))
    };
    return color;
}