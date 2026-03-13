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
    void AddAdjacency(int adjacentKernelId, int direction);
    bool CompareAdjacentOverlap(const Kernel& otherKernel, int direction, int opposeDirection) const;
    const std::vector<uint32_t>& GetAdjacentOverlap(int direction) const;
};


// Datastructure to map the kernels to an integer (the order seen)
class Composite {
public:
    Composite(int size);
    
    int AppendKernel(std::vector<uint32_t>&& leafs);
    int GetGlobalFrequency(int kernelId) const;
    const std::vector<Kernel>& GetKernels() const;
    Kernel& GetKernel(int kernelId);
    const int GetNumberOfKernels() const;

private:
    std::vector<Kernel> kernels;
    int size;

    // Returns index if kernel exists, -1 if not
    int IsKernelExists(const std::vector<uint32_t>& kernel); 
};