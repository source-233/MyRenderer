#pragma once

#include "../rhi_render_pass.h"
#include "vulkan_common.h"

class VulkanFrameBuffer : public IFrameBuffer {
public:
    VulkanFrameBuffer(VkDevice device, VkFramebuffer fb, const FrameBufferDesc& desc)
        : m_device(device), m_framebuffer(fb), m_desc(desc) {}
    ~VulkanFrameBuffer() override;

    VkFramebuffer getHandle() const { return m_framebuffer; }
    uint32_t getWidth() const { return m_desc.width; }
    uint32_t getHeight() const { return m_desc.height; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkFramebuffer   m_framebuffer = VK_NULL_HANDLE;
    FrameBufferDesc m_desc{};
};
