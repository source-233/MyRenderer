#pragma once

#include "rhi_enums.h"
#include <cstdint>

struct SwapChainDesc {
    void*           windowHandle = nullptr;
    uint32_t        width = 800;
    uint32_t        height = 600;
    uint32_t        bufferCount = 2;
    bool            vsync = true;
};

class IImage;

class ISwapChain {
public:
    virtual ~ISwapChain() = default;
    virtual bool            present() = 0;
    virtual uint32_t        getCurrentImageIndex() const = 0;
    virtual IImage*         getCurrentImage() const = 0;
    virtual uint32_t        getWidth() const = 0;
    virtual uint32_t        getHeight() const = 0;
    virtual Format          getFormat() const = 0;
};
