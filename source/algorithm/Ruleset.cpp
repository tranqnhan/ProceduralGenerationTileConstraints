#include "Ruleset.hpp"

Ruleset::Ruleset(int expand) {
    this->numObjects = 0;
    this->expand = expand < 1 ? 1 : expand;
    this->area = (2 * this->expand + 1) * (2 * this->expand + 1) - 1;
}

void Ruleset::AddColor(const Color& color){
    if (this->colors.size() >= MAX_DISTINCT_OBJECTS) return;
    this->constraints.resize((this->colors.size() + 1) * this->area, 0);
    this->colors.emplace_back(color);
}

void Ruleset::AddConstraint(int id, int other, int direction) {
    this->constraints[id * this->area + direction] |= (1 << other);
}

uint32_t Ruleset::GetConstraints(int id, int direction) const {
    return this->constraints[id * this->area + direction];
}

int Ruleset::GetNumberOfObjects() const {
    return this->colors.size();
}

Color Ruleset::GetColor(int id) const {
    if (id >= colors.size()) return colors[0];
    return colors[id];
} 

int Ruleset::GetConstraintArea() const {
    return this->area;
}

int Ruleset::GetConstraintExpand() const {
    return this->expand;
}