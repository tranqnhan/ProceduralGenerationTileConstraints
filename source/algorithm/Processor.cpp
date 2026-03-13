#include <vector>

#include "Ruleset.hpp"
#include "raylib.h"

#include "Composite.hpp"

#include "Processor.hpp"

int Processor::GetModulusSpaceCoord(int coord, int maxCoord) const {
    if (coord < 0) {
        coord = maxCoord + coord + 1;
    } else if (coord > maxCoord) {
        coord = coord - maxCoord - 1;
    }

    return coord;
}

int Processor::ProcessKernel(int x, int y, int width, int height, int length, Color *colors, Composite& compositeTree) {
    // Kernels
    std::vector<uint32_t> kernel;

    for (int k = 0; k < length; ++k) {
        for (int l = 0; l < length; ++l) {
            const int transformCoordY = GetModulusSpaceCoord(y + k, height - 1);
            const int transformCoordX = GetModulusSpaceCoord(x + l, width - 1);
            //printf("x %i y %i tx %i ty %i\n", x + l, y + k, transformCoordX, transformCoordY);
            const Color color = colors[transformCoordY * width + transformCoordX];
            const uint32_t compressedColor = (uint32_t(color.r) << 24) | (uint32_t(color.g) << 16) | (uint32_t(color.b) << 8) | uint32_t(color.a);
            kernel.emplace_back(compressedColor);

            //std::printf("%i %i color r %i g %i b %i a %i\n", transformCoordX, transformCoordY, color.r, color.g, color.b, color.a);

        }
    }

    return compositeTree.AppendKernel(std::move(kernel));
}


Ruleset Processor::AnalyzeImage(const std::string &imageFile, int length) {
    // Load image from filename
    Image sampleImage = LoadImage(imageFile.c_str());    
    ImageFormat(&sampleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *colors = (Color *) sampleImage.data;

    const int height = sampleImage.height;
    const int width = sampleImage.width;

    const int compositeHeight = height;
    const int compositeWidth = width;

    Composite composite(length);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            ProcessKernel(j, i, width, height, length, colors, composite);
        }
    }
    UnloadImage(sampleImage);

    // Get adjacent kernels
    for (int i = 0; i < composite.GetNumberOfKernels(); ++i) {
        Kernel& primaryKernel = composite.GetKernel(i);
        for (int j = i; j < composite.GetNumberOfKernels(); ++j) {
            Kernel& secondaryKernel = composite.GetKernel(j);
            if (primaryKernel.CompareAdjacentOverlap(secondaryKernel, SOUTH, NORTH)) {
                primaryKernel.AddAdjacency(j, NORTH);
                secondaryKernel.AddAdjacency(i, SOUTH);
            }
            
            if (primaryKernel.CompareAdjacentOverlap(secondaryKernel, NORTH, SOUTH)) {
                primaryKernel.AddAdjacency(j, SOUTH);
                secondaryKernel.AddAdjacency(i, NORTH);
            }
            
            if (primaryKernel.CompareAdjacentOverlap(secondaryKernel, EAST, WEST)) {
                primaryKernel.AddAdjacency(j, WEST);
                secondaryKernel.AddAdjacency(i, EAST);
            }

            if (primaryKernel.CompareAdjacentOverlap(secondaryKernel, WEST, EAST)) {
                primaryKernel.AddAdjacency(j, EAST);
                secondaryKernel.AddAdjacency(i, WEST);
            }
        }
    }

    // for (int i = 0; i < composite.GetNumKernels(); ++i) {
    //     std::printf("tile %i \n", i);
    //     const Kernel& primaryKernel = composite.GetKernel(i);
    //     primaryKernel.Print();
    // }

    // Translate to Ruleset
    Ruleset ruleset(composite.GetNumberOfKernels());
    const std::vector<Kernel>& kernels = composite.GetKernels();

    for (int i = 0; i < composite.GetNumberOfKernels(); ++i) {
        const Kernel& kernel = kernels[i];
        ruleset.SetTileFrequency(i, kernel.GetGlobalFrequency());
        ruleset.SetTileColor(i, kernel.leafs[0]);
        for (int d = 0; d < TileDirection::NUM_DIRECTIONS; ++d) {
            const int adjacentSize = kernel.adjacentKernelFrequencies[d].size();
            std::vector<int> adjacentTileIds(adjacentSize);
            std::vector<int> adjacentTileFrequencies(adjacentSize);
            int v = 0;
            for (const auto& iteAdjacentTile : kernel.adjacentKernelFrequencies[d]) {
                adjacentTileIds[v] = iteAdjacentTile.first;
                adjacentTileFrequencies[v] = iteAdjacentTile.second;
                v++;
            }
            ruleset.SetAdjacentTiles(i, d, adjacentTileIds, adjacentTileFrequencies);
        }
    }

    std::printf("num tiles %i\n", ruleset.GetNumberOfTiles());
    
    //DebugGenerateTexture(composite, width, height, length);

    return ruleset;
}


void Processor::DebugGenerateTexture(const Composite& compositeTree, int width, int height, int length) {
    //Debugging
    std::printf("length %i\n", length);

    const int debugImageWidth = width * length;
    const int debugImageHeight = height * length;
    
    debugImage = GenImageColor(debugImageWidth, debugImageHeight, WHITE);
    Color *debugColors = (Color *) debugImage.data;
    
    const std::vector<Kernel>& kernels = compositeTree.GetKernels();

    int atX = 0;
    int atY = 0;

    for (int k = 0; k < kernels.size(); ++k) {
        for (int i = 0; i < length; ++i) {
            for (int j = 0; j < length; ++j) {
                const uint32_t colorId = kernels[k].leafs[i * length + j];
                const Color color = Color{
                    .r = (unsigned char)((colorId & 0xFF0000) >> 16), 
                    .g = (unsigned char)((colorId & 0xFF00) >> 8),
                    .b = (unsigned char)((colorId & 0xFF)),
                    .a = 255
                };

                const int x = j + atX;
                const int y = i + atY;
                debugColors[y * debugImageWidth + x] = color;
            }
        }

        atX += length;
        if (atX >= debugImageWidth) {
            atX = 0;
            atY += length;
        }
    }

    debugTexture = LoadTextureFromImage(debugImage);
    UnloadImage(debugImage);
}