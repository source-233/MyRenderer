#pragma once

#include "rhi_enums.h"
#include <vector>
#include <cstdint>

struct DescriptorSetLayoutBinding {
    uint32_t        binding = 0;
    DescriptorType  type = DescriptorType::SAMPLED_IMAGE;
    uint32_t        count = 1;
    ShaderStage     stage = ShaderStage::ALL_GRAPHICS;
};

struct DescriptorSetLayoutDesc {
    std::vector<DescriptorSetLayoutBinding> bindings;
    bool                                    bindless = false;
};

class IDescriptorSetLayout {
public:
    virtual ~IDescriptorSetLayout() = default;
};
