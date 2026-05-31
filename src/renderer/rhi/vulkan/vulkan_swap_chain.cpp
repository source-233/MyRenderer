#include "vulkan_swap_chain.h"

VulkanSwapChain::~VulkanSwapChain() {
    for (auto* img : m_images) {
        // Only free the VulkanImage wrapper, not the VkImage
        // (swapchain images are owned by the swapchain)
        delete img;
    }
    m_images.clear();
    if (m_swapChain) {
        vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
    }
}

void VulkanSwapChain::setImages(VkImage* images, uint32_t count) {
    m_images.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        ImageDesc desc;
        desc.width = m_width;
        desc.height = m_height;
        desc.format = m_format;
        desc.usage = ImageUsage::COLOR_ATTACHMENT | ImageUsage::SAMPLED;
        // Create with VK_NULL_HANDLE memory since images are owned by swapchain
        auto* img = new VulkanImage(m_device, images[i], VK_NULL_HANDLE, desc);
        m_images.push_back(img);
    }
}

bool VulkanSwapChain::present() {
    // TODO: Implement full present using VkQueue + VkSemaphore
    return true;
}

IImage* VulkanSwapChain::getCurrentImage() const {
    if (m_currentImage < m_images.size()) {
        return m_images[m_currentImage];
    }
    return nullptr;
}
