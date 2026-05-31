#pragma once

#include "../rhi_buffer.h"
#include "vulkan_common.h"

class VulkanBuffer : public IBuffer {
public:
    VulkanBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory memory,
                 const BufferDesc& desc)
        : m_device(device), m_buffer(buffer), m_memory(memory), m_desc(desc) {}
    ~VulkanBuffer() override;

    void*       map() override;
    void        unmap() override;
    BufferDesc  getDesc() const override { return m_desc; }
    uint64_t    getDeviceAddress() const override;

    VkBuffer    getHandle() const { return m_buffer; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkBuffer        m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory  m_memory = VK_NULL_HANDLE;
    BufferDesc      m_desc{};
};
