#pragma once

#include <cstdint>
#include <vector>

#include "Kernel.hpp"

// Datastructure to map the kernels to an integer (the order seen)
class Composite {
public:
    Composite(int kernelLength);
    
    // Returns index of kernel. If kernel does not exist, create one and return new index
    int NextKernel(std::vector<uint32_t>&& leafs); 

    std::vector<Kernel>& GetKernels();
    Kernel& GetKernel(int kernelId);
    const int GetNumberOfKernels() const;

private:
    std::vector<Kernel> kernels;

    int kernelLength;

    int AppendKernel(std::vector<uint32_t>&& leafs);

};