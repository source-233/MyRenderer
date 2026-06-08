#include "vulkan_frame_buffer.h"

VulkanFrameBuffer::~VulkanFrameBuffer() {
    if (m_framebuffer) {
        vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
    }
}
