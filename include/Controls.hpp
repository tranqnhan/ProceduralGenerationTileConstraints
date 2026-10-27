#pragma once

#include <functional>

class Controls {
    public:
        std::function<void(int)> onColorSelected = nullptr;
        std::function<void()> onChangeScene = nullptr;
};

extern Controls CONTROLS;