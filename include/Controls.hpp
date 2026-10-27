#pragma once

#include <functional>

class Controls {
    public:
        std::function<void(int)> onColorSelected = nullptr;
        std::function<void()> onChangeScene = nullptr;
        std::function<void(bool)> onToggleFill = nullptr;
};

extern Controls CONTROLS;