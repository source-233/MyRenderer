#pragma once

#include <string>
#include <vector>
#include "../component/transform.h"

struct NodeDesc {
    std::string name;
    TransformComponent transform;
    uint32_t parentIndex = UINT32_MAX;
};

struct SceneDescription {
    std::vector<NodeDesc> nodes;
};
