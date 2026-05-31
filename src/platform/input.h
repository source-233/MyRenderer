#pragma once

#include <cstdint>

namespace platform {

struct InputState {
    bool keys[512]{};
    double mouseX = 0, mouseY = 0;
    bool mouseButtons[8]{};

    bool isKeyDown(int key) const { return key >= 0 && key < 512 && keys[key]; }
    bool isMouseDown(int btn) const { return btn >= 0 && btn < 8 && mouseButtons[btn]; }
};

} // namespace platform
