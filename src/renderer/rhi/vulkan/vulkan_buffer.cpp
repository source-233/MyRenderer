#include "vulkan_buffer.h"

VulkanBuffer::~VulkanBuffer() {
    if (m_buffer) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
    }
    if (m_memory) {
        vkFreeMemory(m_device, m_memory, nullptr);
    }
}

void* VulkanBuffer::map() {
    void* data = nullptr;
    vkMapMemory(m_device, m_memory, 0, VK_WHOLE_SIZE, 0, &data);
    return data;
}

void VulkanBuffer::unmap() {
    vkUnmapMemory(m_device, m_memory);
}

uint64_t VulkanBuffer::getDeviceAddress() const {
    VkBufferDeviceAddressInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.buffer = m_buffer;
    return vkGetBufferDeviceAddress(m_device, &info);
}
