#pragma once

#include "rhi_enums.h"

struct SamplerDesc {
    Filter          minFilter = Filter::LINEAR;
    Filter          magFilter = Filter::LINEAR;
    MipmapMode      mipmapMode = MipmapMode::LINEAR;
    AddressMode     addressU = AddressMode::REPEAT;
    AddressMode     addressV = AddressMode::REPEAT;
    AddressMode     addressW = AddressMode::REPEAT;
    float           maxAnisotropy = 1.0f;
    CompareOp       compareOp = CompareOp::NEVER;
    float           minLod = 0.0f;
    float           maxLod = 1000.0f;
};

class ISampler {
public:
    virtual ~ISampler() = default;
};
