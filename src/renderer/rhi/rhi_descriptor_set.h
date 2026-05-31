#pragma once

#include "rhi_enums.h"
#include <cstdint>
#include <vector>

class IBuffer;
class IImageView;
class ISampler;

struct DescriptorWrite {
    uint32_t            binding = 0;
    uint32_t            arrayIndex = 0;
    DescriptorType      type = DescriptorType::SAMPLED_IMAGE;
    IBuffer*            buffer = nullptr;
    uint64_t            bufferOffset = 0;
    uint64_t            bufferRange = 0;
    IImageView*         imageView = nullptr;
    ISampler*           sampler = nullptr;
    ImageLayout         imageLayout = ImageLayout::SHADER_READ;
};

struct DescriptorUpdate {
    std::vector<DescriptorWrite> writes;
};

class IDescriptorSet {
public:
    virtual ~IDescriptorSet() = default;
    virtual uint64_t    getGPUHandle(uint32_t index) const = 0;
    virtual void        write(const DescriptorWrite& write) = 0;
};
