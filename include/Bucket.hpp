#pragma once

#include <vector>

#include "ankerl/unordered_dense.h"

// Bucket for unique ids with similar items
class Bucket {
public:
    Bucket(int item);
    void SetItem(int item);
    int GetItem() const;
    void AppendUniqueId(int uniqueId);
    void RemoveUniqueId(int uniqueId);
    int PopRandom();
    int GetSize() const;

private:
    std::vector<int> bucket;
    ankerl::unordered_dense::map<int, int> mapUniqueIdBucketIndex;
    int item;
};



