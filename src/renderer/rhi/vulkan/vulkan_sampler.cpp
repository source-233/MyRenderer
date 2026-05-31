#include "vulkan_sampler.h"

VulkanSampler::~VulkanSampler() {
    if (m_sampler) {
        vkDestroySampler(m_device, m_sampler, nullptr);
    }
}
