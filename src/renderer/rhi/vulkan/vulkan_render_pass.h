#pragma once

#include "../rhi_render_pass.h"
#include "vulkan_common.h"

class VulkanRenderPass : public IRenderPass {
public:
    VulkanRenderPass(VkDevice device, VkRenderPass pass, const RenderPassDesc& desc)
        : m_device(device), m_renderPass(pass), m_desc(desc) {}
    ~VulkanRenderPass() override;

    VkRenderPass getHandle() const { return m_renderPass; }
    const RenderPassDesc& getDesc() const { return m_desc; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkRenderPass    m_renderPass = VK_NULL_HANDLE;
    RenderPassDesc  m_desc{};
};
