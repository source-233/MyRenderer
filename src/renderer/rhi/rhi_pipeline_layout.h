#pragma once

#include "rhi_enums.h"
#include <vector>
#include <cstdint>

class IDescriptorSetLayout;

struct PushConstantRange {
    ShaderStage     stage = ShaderStage::ALL_GRAPHICS;
    uint32_t        offset = 0;
    uint32_t        size = 0;
};

struct PipelineLayoutDesc {
    std::vector<IDescriptorSetLayout*>  setLayouts;
    std::vector<PushConstantRange>      pushConstants;
};

class IPipelineLayout {
public:
    virtual ~IPipelineLayout() = default;
};
