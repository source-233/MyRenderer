#pragma once

#include <cstdint>

class Component {
public:
    Component() = default;
    virtual ~Component() = default;

    virtual void update(float dt) = 0;
};
