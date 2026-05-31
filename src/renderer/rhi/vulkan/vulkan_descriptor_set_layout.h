#pragma once

#include "../rhi_descriptor_set_layout.h"
#include "vulkan_common.h"

class VulkanDescriptorSetLayout : public IDescriptorSetLayout {
public:
    VulkanDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout,
                              const DescriptorSetLayoutDesc& desc)
        : m_device(device), m_layout(layout), m_desc(desc) {}
    ~VulkanDescriptorSetLayout() override;

    VkDescriptorSetLayout      getHandle() const { return m_layout; }
    const DescriptorSetLayoutDesc& getDesc() const { return m_desc; }

private:
    VkDevice                m_device = VK_NULL_HANDLE;
    VkDescriptorSetLayout   m_layout = VK_NULL_HANDLE;
    DescriptorSetLayoutDesc m_desc{};
};
