#pragma once

#include <cstdint>

class IFence {
public:
    virtual ~IFence() = default;
    virtual void        wait(uint64_t timeout = UINT64_MAX) = 0;
    virtual void        reset() = 0;
    virtual bool        isSignaled() const = 0;
};
