#pragma once

#include "rhi_enums.h"

class IBuffer;
class IImage;

struct BufferBarrier {
    IBuffer*        buffer = nullptr;
    ResourceState   srcState = ResourceState::UNDEFINED;
    ResourceState   dstState = ResourceState::UNDEFINED;
    uint64_t        offset = 0;
    uint64_t        size = 0;
};

struct ImageBarrier {
    IImage*         image = nullptr;
    ResourceState   srcState = ResourceState::UNDEFINED;
    ResourceState   dstState = ResourceState::UNDEFINED;
    ImageLayout     srcLayout = ImageLayout::UNDEFINED;
    ImageLayout     dstLayout = ImageLayout::UNDEFINED;
    ImageAspect     aspectMask = ImageAspect::COLOR;
};

struct ResourceBarrier {
    enum Type { BUFFER, IMAGE };
    Type type;
    union {
        BufferBarrier buffer;
        ImageBarrier  image;
    };
};

struct ImageCopyRegion {
    uint32_t    srcOffsetX = 0, srcOffsetY = 0, srcOffsetZ = 0;
    uint32_t    dstOffsetX = 0, dstOffsetY = 0, dstOffsetZ = 0;
    uint32_t    width = 0, height = 0, depth = 1;
};
