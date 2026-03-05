#pragma once

#define MAX_DISTINCT_OBJECTS 32 // Maximum number of distinct objects to constraint

#include <cstdint>
#include <vector>

#include "raylib.h"


class Ruleset {
public:

    Ruleset(int size);

    void AddColor(const Color& color);
    Color GetColor(int id) const; 
    void AddConstraint(int id, int other, int direction);
    uint32_t GetConstraints(int id, int direction) const;
    int GetNumberOfObjects() const;
    int GetConstraintArea() const;
    int GetConstraintExpand() const;

private:
    int area;
    int expand;
    int numObjects;
    std::vector<Color> colors;
    std::vector<uint32_t> constraints;

};
