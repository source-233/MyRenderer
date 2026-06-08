#include "vulkan_swap_chain.h"

VulkanSwapChain::~VulkanSwapChain() {
    for (auto* img : m_images) {
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
        auto* img = new VulkanImage(m_device, images[i], VK_NULL_HANDLE, desc);
        img->setOwnsImage(false);
        m_images.push_back(img);
    }
}

bool VulkanSwapChain::present() {
    VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX,
                                    VK_NULL_HANDLE, VK_NULL_HANDLE, &m_currentImage));
    return true;
}

IImage* VulkanSwapChain::getCurrentImage() const {
    if (m_currentImage < m_images.size()) {
        return m_images[m_currentImage];
    }
    return nullptr;
}

IImage* VulkanSwapChain::getImage(uint32_t index) const {
    if (index < m_images.size()) {
        return m_images[index];
    }
    return nullptr;
}
