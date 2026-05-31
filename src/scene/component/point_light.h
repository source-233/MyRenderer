#pragma once

#include "component.h"
#include "../../core/math/math.h"

class PointLightComponent : public Component {
public:
    void update(float dt) override {}

    Vector3 color{1, 1, 1};
    float intensity = 1.0f;
    float range = 10.0f;
};
