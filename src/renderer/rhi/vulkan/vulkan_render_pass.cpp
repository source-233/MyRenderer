#include "vulkan_render_pass.h"

VulkanRenderPass::~VulkanRenderPass() {
    if (m_renderPass) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    }
}
