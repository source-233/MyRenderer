#pragma once

#include "component.h"
#include <cstdint>

class MeshComponent : public Component {
public:
    void update(float dt) override {}

    uint32_t meshResourceId = UINT32_MAX;
    uint32_t materialId = UINT32_MAX;
};
