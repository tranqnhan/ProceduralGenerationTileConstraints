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

    void IncreaseGlobalFrequency();
    int GetGlobalFrequency() const;
    void AddAdjacency(int adjacentKernelId, TileDirection direction);
    bool CompareAdjacentOverlap(const Kernel& otherKernel, TileDirection otherDirection, TileDirection myDirection) const;
    const std::vector<uint32_t>& GetAdjacentOverlap(TileDirection direction) const;

    void Print() const {
        std::printf("NORTH\n");
        for (const auto& p : this->adjacentKernelFrequencies[NORTH]) {
            std::printf("%i ", p.first);
        }
        std::printf("\n");
        std::printf("SOUTH\n");
        for (const auto& p : this->adjacentKernelFrequencies[SOUTH]) {
            std::printf("%i ", p.first);
        }
        std::printf("\n");
        std::printf("WEST\n");
        for (const auto& p : this->adjacentKernelFrequencies[WEST]) {
            std::printf("%i ", p.first);
        }
        std::printf("\n");
        std::printf("EAST\n");
        for (const auto& p : this->adjacentKernelFrequencies[EAST]) {
            std::printf("%i ", p.first);
        }
        std::printf("\n");
    }
};


// Datastructure to map the kernels to an integer (the order seen)
class Composite {
public:
    Composite(int size);
    
    int AppendKernel(std::vector<uint32_t>&& leafs);
    int GetGlobalFrequency(int kernelId) const;
    const std::vector<Kernel>& GetKernels() const;
    Kernel& GetKernel(int kernelId);
    const int GetNumKernels() const;

private:
    std::vector<Kernel> kernels;
    int size;

    // Returns index if kernel exists, -1 if not
    int IsKernelExists(const std::vector<uint32_t>& kernel); 
};