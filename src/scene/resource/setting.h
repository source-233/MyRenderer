#pragma once

#include <cstdint>

struct SceneSettings {
    uint32_t shadowMapSize = 2048;
    bool enableShadows = true;
    bool enablePostProcess = true;
};
