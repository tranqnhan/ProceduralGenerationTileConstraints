#include "Kernel.hpp"


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


const std::vector<uint32_t>& Kernel::GetAdjacentOverlap(int direction) const {
    return this->overlaps[direction];
}

