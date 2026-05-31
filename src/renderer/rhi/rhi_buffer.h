#pragma once

#include "rhi_enums.h"
#include <cstdint>

struct BufferDesc {
    uint64_t        size = 0;
    BufferUsage     usage = BufferUsage::VERTEX;
    ResourceState   initialState = ResourceState::UNDEFINED;
    bool            hostVisible = false;
    const char*     debugName = nullptr;
};

class IBuffer {
public:
    virtual ~IBuffer() = default;
    virtual void*       map() = 0;
    virtual void        unmap() = 0;
    virtual BufferDesc  getDesc() const = 0;
    virtual uint64_t    getDeviceAddress() const = 0;
};
