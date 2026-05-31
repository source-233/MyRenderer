#pragma once

#include "component.h"
#include <cstdint>

class EnvironmentComponent : public Component {
public:
    void update(float dt) override {}

    uint32_t skyboxTextureId = UINT32_MAX;
};
