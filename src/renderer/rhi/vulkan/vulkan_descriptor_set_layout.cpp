#include "vulkan_descriptor_set_layout.h"

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
    if (m_layout) {
        vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
    }
}
