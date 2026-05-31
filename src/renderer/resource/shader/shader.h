#pragma once

#include "../../rhi/rhi.h"

class ShaderManager {
public:
    explicit ShaderManager(IRHI* rhi);
    ~ShaderManager();

    IShader* create(const ShaderDesc& desc);
    void destroy(IShader* shader);

private:
    IRHI* m_rhi = nullptr;
};
