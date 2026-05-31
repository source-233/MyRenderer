#pragma once

#include "../rhi_shader.h"
#include "vulkan_common.h"

class VulkanShader : public IShader {
public:
    VulkanShader(VkDevice device, VkShaderModule module, const ShaderDesc& desc)
        : m_device(device), m_module(module), m_desc(desc) {}
    ~VulkanShader() override;

    ShaderStage getStage() const override { return m_desc.stage; }

    VkShaderModule      getModule() const { return m_module; }
    VkShaderStageFlagBits getStageVk() const { return shaderStageToVk(m_desc.stage); }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkShaderModule  m_module = VK_NULL_HANDLE;
    ShaderDesc      m_desc{};
};
