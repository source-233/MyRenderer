#include "vulkan_shader.h"

VulkanShader::~VulkanShader() {
    if (m_module) {
        vkDestroyShaderModule(m_device, m_module, nullptr);
    }
}
