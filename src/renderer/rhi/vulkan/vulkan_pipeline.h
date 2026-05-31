#pragma once

#include "../rhi_pipeline.h"
#include "vulkan_common.h"

class VulkanPipeline : public IPipeline {
public:
    VulkanPipeline(VkDevice device, VkPipeline pipeline, VkPipelineLayout layout)
        : m_device(device), m_pipeline(pipeline), m_layout(layout) {}
    ~VulkanPipeline() override;

    VkPipeline       getHandle() const { return m_pipeline; }
    VkPipelineLayout getLayout() const { return m_layout; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkPipeline      m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_layout = VK_NULL_HANDLE;
};
