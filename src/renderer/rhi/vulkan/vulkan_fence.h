#pragma once

#include "../rhi_fence.h"
#include "vulkan_common.h"

class VulkanFence : public IFence {
public:
    VulkanFence(VkDevice device, VkFence fence)
        : m_device(device), m_fence(fence) {}
    ~VulkanFence() override;

    void    wait(uint64_t timeout = UINT64_MAX) override;
    void    reset() override;
    bool    isSignaled() const override;

    VkFence getHandle() const { return m_fence; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkFence  m_fence = VK_NULL_HANDLE;
};
