#pragma once

#include "component.h"
#include "../../core/math/math.h"

class DirectionalLightComponent : public Component {
public:
    void update(float dt) override {}

    Vector3 color{1, 1, 1};
    float intensity = 1.0f;
};
