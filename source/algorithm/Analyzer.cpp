#include "raylib.h"
#include <array>
#include <cstdio>
#include <vector>

#include "Analyzer.hpp"

int Analyzer::GetModulusSpaceCoord(int coord, int maxCoord) const {
    if (coord < 0) {
        coord = maxCoord + coord + 1;
    } else if (coord > maxCoord) {
        coord = coord - maxCoord;
    }

    return coord;
}

void Analyzer::AddKernelComposite(int x, int y, int width, int height, int length, Color *colors, std::vector<Color>& composites) {
    // Kernels

    for (int k = 0; k < length; ++k) {
        for (int l = 0; l < length; ++l) {
            const int transformCoordY = GetModulusSpaceCoord(y + k, height - 1);
            const int transformCoordX = GetModulusSpaceCoord(x + l, width - 1);
            printf("x %i y %i tx %i ty %i\n", x + l, y + k, transformCoordX, transformCoordY);
            composites.emplace_back(colors[transformCoordY * height + transformCoordX]);
        }
    }
}


void Analyzer::AnalyzeImage(const std::string &imageFile, int length) {
    // Load image from filename
    Image sampleImage = LoadImage(imageFile.c_str());    
    ImageFormat(&sampleImage, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *colors = (Color *) sampleImage.data;

    const int height = sampleImage.height;
    const int width = sampleImage.width;

    std::vector<Color> composites;
    
    for (int i = -length + 1; i < height; ++i) {
        for (int j = -length + 1; j < width; ++j) {
            AddKernelComposite(j, i, width, height, length, colors, composites);
        }
    }

    UnloadImage(sampleImage);

    DebugGenerateTexture(composites, length);
}

void Analyzer::DebugGenerateTexture(const std::vector<Color> composites, int length) {
    //debugImage = GenImageColor(composites.size(), 1, BLACK);
    //Color *debugColors = (Color *) debugImage.data;
    //for (int i = 0; i < composites.size(); ++i) {
    //    debugColors[i] = composites[i];
    //}
    
    //Debugging
    const int debugImageWidth = composites.size() / length;
    const int debugImageHeight = length;
    
    debugImage = GenImageColor(composites.size() / length, length, BLACK);
    Color *debugColors = (Color *) debugImage.data;
    
    const int numComposites = composites.size() / (length * length);
    
    for (int i = 0; i < numComposites; ++i) {
       for (int k = 0; k < length; ++k) {
           for (int l = 0; l < length; ++l) {
               debugColors[(k * length + l) * (i + 1)] = composites[(i + 1) * (k + 1) * (l + 1) - 1];
           }            
       }
    }

    for (int i = 0; i < length; ++i) {
        for (int k = i * length; k < composites.size(); k += length * length) {
            for (int j = k; j < length; ++j) {
                
            }
        }
    }

    debugTexture = LoadTextureFromImage(debugImage);
    UnloadImage(debugImage);
}