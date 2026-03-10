#include "Composite.hpp"
#include "Ruleset.hpp"

Kernel::Kernel(std::vector<Item>&& leafs){
    this->leafs = std::move(leafs);
    this->globalFrequency = 1;
}


void Kernel::IncreaseGlobalFrequency(){
    this->globalFrequency++;
}


int Kernel::GetGlobalFrequency() const {
    return this->globalFrequency;
}


void Kernel::IncreaseAdjacentFrequency(int adjacentKernelId, TileDirection direction) {
    auto iteAdjacentKernel = this->adjacentKernelFrequencies[direction].find(adjacentKernelId);
    if (iteAdjacentKernel == this->adjacentKernelFrequencies[direction].end()) {
        this->adjacentKernelFrequencies[direction].emplace(adjacentKernelId, 1);
    } else {
        iteAdjacentKernel->second++;
    }
}


Kernel& Composite::GetKernel(int kernelId) {
    return this->kernels[kernelId];
}

int Composite::AppendKernel(const std::vector<uint32_t>& kernel) {
    const int kernelIndex = IsKernelExists(kernel);
    
    if (kernelIndex >= 0) {
        this->kernels[kernelIndex].IncreaseGlobalFrequency();
        return kernelIndex;
    }

    std::vector<Item> leafs;
    leafs.resize(kernel.size());
    for (int i = 0; i < kernel.size(); ++i) {
        leafs[i].color = kernel[i];
    }

    kernels.emplace_back(std::move(leafs));

    return kernels.size() - 1;
}


int Composite::GetGlobalFrequency(int kernelIndex) const {
    return kernels[kernelIndex].GetGlobalFrequency();
}


int Composite::IsKernelExists(const std::vector<uint32_t>& kernel) {
    for (int i = 0; i < kernels.size(); ++i) {
        if (kernel.size() != kernels[i].leafs.size()) continue;
        int j;
        for (j = 0; j < kernels[i].leafs.size(); ++j) {
            if (kernels[i].leafs[j].color != kernel[j]) {
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
