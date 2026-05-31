#pragma once

#include "rhi_enums.h"
#include <cstdint>

struct ImageDesc {
    uint32_t        width = 1;
    uint32_t        height = 1;
    uint32_t        depth = 1;
    uint32_t        mipLevels = 1;
    uint32_t        arrayLayers = 1;
    Format          format = Format::R8G8B8A8_UNORM;
    ImageUsage      usage = ImageUsage::SAMPLED;
    const char*     debugName = nullptr;
};

struct ImageViewDesc {
    Format          format = Format::UNKNOWN;
    ImageAspect     aspect = ImageAspect::COLOR;
    uint32_t        baseMipLevel = 0;
    uint32_t        mipLevelCount = 1;
    uint32_t        baseArrayLayer = 0;
    uint32_t        layerCount = 1;
};

class IImage;

class IImageView {
public:
    virtual ~IImageView() = default;
    virtual IImage*         getImage() const = 0;
    virtual ImageViewDesc   getDesc() const = 0;
};

class IImage {
public:
    virtual ~IImage() = default;
    virtual ImageDesc    getDesc() const = 0;
    virtual IImageView*  createView(const ImageViewDesc& desc) = 0;
};
