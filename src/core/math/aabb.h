#pragma once

#include "math.h"

struct AABB {
    Vector3 min{-1, -1, -1};
    Vector3 max{ 1,  1,  1};

    AABB transformed(const Matrix4& m) const;
};
