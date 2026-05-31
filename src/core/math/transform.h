#pragma once

#include "math.h"

struct Transform {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale{1, 1, 1};

    Matrix4 toMatrix() const;
};
