#include "Composite.hpp"
#include "Ruleset.hpp"
#include <cstdint>
#include <sys/types.h>

Kernel::Kernel(std::vector<uint32_t>&& leafs){
    this->leafs = std::move(leafs);
    this->globalFrequency = 1;
}


void Kernel::IncreaseGlobalFrequency(){
    this->globalFrequency++;
}


int Kernel::GetGlobalFrequency() const {
    return this->globalFrequency;
}


void Kernel::AddAdjacency(int adjacentKernelId, TileDirection direction) {
    const auto& iteAdjacentKernel = this->adjacentKernelFrequencies[direction].find(adjacentKernelId);
    if (iteAdjacentKernel == this->adjacentKernelFrequencies[direction].end()) {
        this->adjacentKernelFrequencies[direction].emplace(adjacentKernelId, 1);
    } else {
        iteAdjacentKernel->second++;
    }
}


bool Kernel::CompareAdjacentOverlap(const Kernel& otherKernel, TileDirection otherDirection, TileDirection myDirection) const {
    const std::vector<uint32_t>& otherLeafs = otherKernel.GetAdjacentOverlap(otherDirection);
    const std::vector<uint32_t>& myLeafs = this->overlaps[myDirection];

    for (int i = 0; i < myLeafs.size(); ++i) {
        if (otherLeafs[i] != myLeafs[i]) {
            return false;
        }
    }
    return true;
}


const std::vector<uint32_t>&  Kernel::GetAdjacentOverlap(TileDirection direction) const {
    return this->overlaps[direction];
}


Composite::Composite(int size) {
    this->size = size;
}


Kernel& Composite::GetKernel(int kernelId) {
    return this->kernels[kernelId];
}

int Composite::AppendKernel(std::vector<uint32_t>&& leafs) {
    const int kernelIndex = IsKernelExists(leafs);
    
    if (kernelIndex >= 0) {
        this->kernels[kernelIndex].IncreaseGlobalFrequency();
        return kernelIndex;
    }

    Kernel kernel(std::move(leafs));

    const int overlapSize = this->size * (this->size - 1);
    int j;

    // North
    j = 0;
    kernel.overlaps[TileDirection::NORTH].resize(overlapSize);
    for (int i = 0; i < overlapSize; ++i) {
        kernel.overlaps[TileDirection::NORTH][j++] = kernel.leafs[i];
    }

    // South
    j = 0;
    kernel.overlaps[TileDirection::SOUTH].resize(overlapSize);
    for (int i = this->size; i < kernel.leafs.size(); ++i) {
        kernel.overlaps[TileDirection::SOUTH][j++] = kernel.leafs[i];
    }

    // East
    j = 0;
    kernel.overlaps[TileDirection::EAST].resize(overlapSize);
    for (int i = 0; i < this->size; ++i) {
        for (int k = 1; k < this->size; k++) {
            kernel.overlaps[TileDirection::EAST][j++] = kernel.leafs[i * this->size + k];
        }
    }

    // West
    j = 0;
    kernel.overlaps[TileDirection::WEST].resize(overlapSize);
    for (int i = 0; i < this->size; ++i) {
        for (int k = 0; k < this->size - 1; k++) {
            kernel.overlaps[TileDirection::WEST][j++] = kernel.leafs[i * this->size + k];
        }
    }

    kernels.emplace_back(std::move(kernel));

    return kernels.size() - 1;
}


int Composite::GetGlobalFrequency(int kernelIndex) const {
    return kernels[kernelIndex].GetGlobalFrequency();
}


int Composite::IsKernelExists(const std::vector<uint32_t>& kernel) {
    for (int i = 0; i < kernels.size(); ++i) {
        int j;
        for (j = 0; j < kernels[i].leafs.size(); ++j) {
            if (kernels[i].leafs[j] != kernel[j]) {
                break;
            }
        }
        if (j == kernels[i].leafs.size()) {
            return i;
        }
    }

    return -1;
}


const std::vector<Kernel>& Composite::GetKernels() const {
    return this->kernels;
}


const int Composite::GetNumKernels() const {
    return this->kernels.size();
}
