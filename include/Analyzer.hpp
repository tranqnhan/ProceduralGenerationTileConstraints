#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "raylib.h"

#include "Ruleset.hpp"

class Analyzer {
public:
    Ruleset AnalyzeImage(const std::string& imageFile);

private:

    int GetColorID(const Color& color, Ruleset& ruleset, std::unordered_map<uint32_t, int>& colorMap);

};

