#include "vulkan_fence.h"

VulkanFence::~VulkanFence() {
    if (m_fence) {
        vkDestroyFence(m_device, m_fence, nullptr);
    }
}

void VulkanFence::wait(uint64_t timeout) {
    vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, timeout);
}

void VulkanFence::reset() {
    vkResetFences(m_device, 1, &m_fence);
}

bool VulkanFence::isSignaled() const {
    VkResult result = vkGetFenceStatus(m_device, m_fence);
    return result == VK_SUCCESS;
}
