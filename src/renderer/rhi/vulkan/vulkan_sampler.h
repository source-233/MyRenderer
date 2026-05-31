#pragma once

#include "../rhi_sampler.h"
#include "vulkan_common.h"

class VulkanSampler : public ISampler {
public:
    VulkanSampler(VkDevice device, VkSampler sampler, const SamplerDesc& desc)
        : m_device(device), m_sampler(sampler), m_desc(desc) {}
    ~VulkanSampler() override;

    VkSampler getHandle() const { return m_sampler; }

private:
    VkDevice    m_device = VK_NULL_HANDLE;
    VkSampler   m_sampler = VK_NULL_HANDLE;
    SamplerDesc m_desc{};
};
