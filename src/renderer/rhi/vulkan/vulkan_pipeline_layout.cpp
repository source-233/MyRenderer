#include "vulkan_pipeline_layout.h"

VulkanPipelineLayout::~VulkanPipelineLayout() {
    if (m_layout) {
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
    }
}
