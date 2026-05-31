#pragma once

#include "aabb.h"
#include <vector>
#include <cstdint>

struct BVHNode {
    AABB bounds;
    uint32_t firstChild = 0;
    uint32_t primitiveCount = 0;
};

class BVH {
public:
    BVH() = default;
    ~BVH() = default;

    void build(const std::vector<AABB>& primitives);
    void rebuild() {}

private:
    std::vector<BVHNode> m_nodes;
};
