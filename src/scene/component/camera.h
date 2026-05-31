#pragma once

#include "component.h"
#include "../../core/math/math.h"

class CameraComponent : public Component {
public:
    void update(float dt) override {}

    Matrix4 getViewMatrix() const;
    Matrix4 getProjectionMatrix() const;

    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
};
