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
            const Color color = colors[transformCoordY * height + transformCoordX];
            kernel.emplace_back(((color.r) << 16) | (color.g << 8) | (color.b));
        }
    }

    return compositeTree.AppendKernel(kernel);
}


Ruleset Processor::AnalyzeImage(const std::string &imageFile, int length) {
    // Load image from filename
    Image sampleImage = LoadImage(imageFile.c_str());    
    ImageFormat(&sampleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *colors = (Color *) sampleImage.data;

    const int height = sampleImage.height;
    const int width = sampleImage.width;

    const int compositeHeight = height + length - 1;
    const int compositeWidth = width + length - 1;

    int compositeMap[compositeHeight * compositeWidth];

    Composite composite;
    
    // Get kernels
    for (int i = -length + 1; i < height; ++i) {
        for (int j = -length + 1; j < width; ++j) {
            const int kernelId = ProcessKernel(j, i, width, height, length, colors, composite);
            const int y = i + length - 1;
            const int x = j + length - 1; 
            compositeMap[y * compositeWidth + x] = kernelId;
        }
    }

    UnloadImage(sampleImage);

    // Get adjacent kernels
    for (int i = 0; i < compositeWidth * compositeHeight; ++i) {
        const int x = i % compositeWidth;
        const int y = int(i / compositeWidth);
        const int kernelId = compositeMap[i];
        
        Kernel& kernel = composite.GetKernel(kernelId);
        
        // TODO: Maybe unrolling these out to eliminate an additional check
        // North
        if (y - 1 >= 0) {
            const int northKernelId = compositeMap[(y - 1) * compositeWidth + x];
            kernel.IncreaseAdjacentFrequency(northKernelId, TileDirection::NORTH);
        }

        // South
        if (y + 1 < compositeHeight) {
            const int southKernelId = compositeMap[(y + 1) * compositeWidth + x];
            kernel.IncreaseAdjacentFrequency(southKernelId, TileDirection::SOUTH);
        }

        // West
        if (x - 1 >= 0) {
            const int westKernelId = compositeMap[y * compositeWidth + (x - 1)];
            kernel.IncreaseAdjacentFrequency(westKernelId, TileDirection::WEST);
        }

        // East
        if (x + 1 < compositeWidth) {
            const int eastKernelId = compositeMap[y * compositeWidth + (x + 1)];
            kernel.IncreaseAdjacentFrequency(eastKernelId, TileDirection::EAST);
        }
    }

    // Translate to Ruleset
    Ruleset ruleset(composite.GetNumKernels());
    const std::vector<Kernel>& kernels = composite.GetKernels();
    TileDirection directions[4] = {NORTH, SOUTH, WEST, EAST};
    for (int i = 0; i < composite.GetNumKernels(); ++i) {
        const Kernel& kernel = kernels[i];
        ruleset.SetTileFrequency(i, kernel.GetGlobalFrequency());
        ruleset.SetTileColor(i, kernel.leafs[0].color);
        for (TileDirection d : directions) {
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
    
    DebugGenerateTexture(composite, width, height, length);

    return ruleset;
}


void Processor::DebugGenerateTexture(const Composite& compositeTree, int width, int height, int length) {
    //Debugging
    const int debugImageWidth = (width + length - 1) * length;
    const int debugImageHeight = (height + length - 1) * length;
    
    debugImage = GenImageColor(debugImageWidth, debugImageHeight, WHITE);
    Color *debugColors = (Color *) debugImage.data;
    
    const std::vector<Kernel>& kernels = compositeTree.GetKernels();

    int atX = 0;
    int atY = 0;

    for (int k = 0; k < kernels.size(); ++k) {
        for (int i = 0; i < length; ++i) {
            for (int j = 0; j < length; ++j) {
                const uint32_t colorId = kernels[k].leafs[i * length + j].color;
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