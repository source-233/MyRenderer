#pragma once

#include "../rhi_pipeline_layout.h"
#include "vulkan_common.h"

class VulkanPipelineLayout : public IPipelineLayout {
public:
    VulkanPipelineLayout(VkDevice device, VkPipelineLayout layout,
                         const PipelineLayoutDesc& desc)
        : m_device(device), m_layout(layout), m_desc(desc) {}
    ~VulkanPipelineLayout() override;

    VkPipelineLayout getHandle() const { return m_layout; }
    const PipelineLayoutDesc& getDesc() const { return m_desc; }

private:
    VkDevice            m_device = VK_NULL_HANDLE;
    VkPipelineLayout    m_layout = VK_NULL_HANDLE;
    PipelineLayoutDesc  m_desc{};
};
