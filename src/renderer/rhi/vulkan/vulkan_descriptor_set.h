#pragma once

#include "../rhi_descriptor_set.h"
#include "vulkan_common.h"

class VulkanDescriptorSet : public IDescriptorSet {
public:
    VulkanDescriptorSet(VkDevice device, VkDescriptorSet set,
                        VkDescriptorSetLayout layout)
        : m_device(device), m_set(set), m_layout(layout) {}
    ~VulkanDescriptorSet() override;

    uint64_t    getGPUHandle(uint32_t index) const override;
    void        write(const DescriptorWrite& write) override;

    VkDescriptorSet getHandle() const { return m_set; }

private:
    VkDevice            m_device = VK_NULL_HANDLE;
    VkDescriptorSet     m_set = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
};
