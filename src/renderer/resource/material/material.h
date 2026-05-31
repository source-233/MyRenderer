#pragma once

#include "../../rhi/rhi_enums.h"
#include <cstdint>

class IShader;
class IImageView;
class ISampler;

struct MaterialDesc {
    IShader* vertexShader = nullptr;
    IShader* fragmentShader = nullptr;
    IImageView* albedoTexture = nullptr;
    ISampler* sampler = nullptr;
};

class Material {
public:
    Material() = default;
    ~Material() = default;

    void setDesc(const MaterialDesc& desc) { m_desc = desc; }
    const MaterialDesc& getDesc() const { return m_desc; }

private:
    MaterialDesc m_desc{};
};
