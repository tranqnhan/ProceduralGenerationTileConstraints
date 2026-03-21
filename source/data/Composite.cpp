#include <cstdint>

#include "Composite.hpp"

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
