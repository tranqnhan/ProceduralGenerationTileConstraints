#pragma once

#include <functional>

class Controls {
    public:
        std::function<void(int)> onColorSelected = nullptr;
};

extern Controls CONTROLS;