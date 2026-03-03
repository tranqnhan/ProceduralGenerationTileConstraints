#include "Ruleset.hpp"

Ruleset::Ruleset() {
    numObjects = 0;
    constraints.fill(0);
}

void Ruleset::AddColor(const Color& color){
    if (numObjects >= MAX_DISTINCT_OBJECTS) return;
    colors[numObjects++] = color;
}

void Ruleset::AddConstraint(int id, int other) {
    constraints[id] |= (1 << other);
}

uint32_t Ruleset::GetConstraints(int id) const {
    return constraints[id];
}

int Ruleset::GetNumberOfObjects() const {
    return numObjects;
}

Color Ruleset::GetColor(int id) const {
    if (id >= numObjects) return colors[0];
    return colors[id];
} 
