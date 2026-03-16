#include <cstdint>

#include "Composite.hpp"
#include "Ruleset.hpp"

Kernel::Kernel(std::vector<uint32_t>&& leafs){
    this->leafs = std::move(leafs);
    this->globalFrequency = 1;
}


void Kernel::AddAdjacency(int adjacentKernelId, int direction) {
    const auto& iteAdjacentKernel = this->adjacentKernelFrequencies[direction].find(adjacentKernelId);
    if (iteAdjacentKernel == this->adjacentKernelFrequencies[direction].end()) {
        this->adjacentKernelFrequencies[direction].emplace(adjacentKernelId, 1);
    } else {
        iteAdjacentKernel->second++;
    }
}


bool Kernel::CompareAdjacentOverlap(const Kernel& otherKernel, int direction, int opposeDirection) const {
    const std::vector<uint32_t>& otherLeafs = otherKernel.GetAdjacentOverlap(opposeDirection);
    const std::vector<uint32_t>& myLeafs = this->overlaps[direction];

    for (int i = 0; i < myLeafs.size(); ++i) {
        if (otherLeafs[i] != myLeafs[i]) {
            return false;
        }
    }
    return true;
}


const std::vector<uint32_t>&  Kernel::GetAdjacentOverlap(int direction) const {
    return this->overlaps[direction];
}


Composite::Composite(int kernelLength) {
    this->kernelLength = kernelLength;
}


Kernel& Composite::GetKernel(int kernelId) {
    return this->kernels[kernelId];
}

int Composite::AppendKernel(std::vector<uint32_t>&& leafs) {
    Kernel kernel(std::move(leafs));

    const int overlapSize = this->kernelLength * (this->kernelLength - 1);
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
    for (int i = this->kernelLength; i < kernel.leafs.size(); ++i) {
        kernel.overlaps[TileDirection::SOUTH][j++] = kernel.leafs[i];
    }

    // East
    j = 0;
    kernel.overlaps[TileDirection::EAST].resize(overlapSize);
    for (int i = 0; i < this->kernelLength; ++i) {
        for (int k = 1; k < this->kernelLength; ++k) {
            kernel.overlaps[TileDirection::EAST][j++] = kernel.leafs[i * this->kernelLength + k];
        }
    }

    // West
    j = 0;
    kernel.overlaps[TileDirection::WEST].resize(overlapSize);
    for (int i = 0; i < this->kernelLength; ++i) {
        for (int k = 0; k < this->kernelLength - 1; ++k) {
            kernel.overlaps[TileDirection::WEST][j++] = kernel.leafs[i * this->kernelLength + k];
        }
    }

    kernels.emplace_back(std::move(kernel));

    return kernels.size() - 1;
}


int Composite::NextKernel(std::vector<uint32_t>&& leafs) {
    int j;
    for (j = 0; j < kernels.size(); ++j) {
        int i;
        for (i = 0; i < leafs.size(); ++i) {
            if (leafs[i] != kernels[j].leafs[i]) break;
        } 

        if (i == leafs.size()) {
            break;
        }
    }

    int resultKernelId;

    if (j == kernels.size()) {
        resultKernelId = this->AppendKernel(std::move(leafs));
    } else {
        resultKernelId = j;
        this->kernels[resultKernelId].globalFrequency++;
    }

    return resultKernelId;
}


std::vector<Kernel>& Composite::GetKernels() {
    return this->kernels;
}


const int Composite::GetNumberOfKernels() const {
    return this->kernels.size();
}
