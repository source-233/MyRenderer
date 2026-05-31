#pragma once

#include "component.h"
#include "../../core/math/transform.h"

class TransformComponent : public Component {
public:
    void update(float dt) override {}

    Transform get_transform() const { return transform; }
    void set_transform(const Transform& transform) { this->transform = transform; }

private:
    Transform transform;
};
