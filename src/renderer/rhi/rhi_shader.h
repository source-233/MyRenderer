#pragma once

#include "rhi_enums.h"
#include <cstdint>

struct ShaderDesc {
    ShaderStage     stage = ShaderStage::VERTEX;
    const void*     code = nullptr;
    uint32_t        codeSize = 0;
    const char*     entryPoint = "main";
    const char*     debugName = nullptr;
};

class IShader {
public:
    virtual ~IShader() = default;
    virtual ShaderStage getStage() const = 0;
};
