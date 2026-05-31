#include "vulkan_pipeline.h"

VulkanPipeline::~VulkanPipeline() {
    if (m_pipeline) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
    }
}
