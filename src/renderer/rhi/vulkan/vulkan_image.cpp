#include "vulkan_image.h"

// ──── VulkanImage ────

VulkanImage::~VulkanImage() {
    if (m_image) {
        vkDestroyImage(m_device, m_image, nullptr);
    }
    if (m_memory) {
        vkFreeMemory(m_device, m_memory, nullptr);
    }
}

IImageView* VulkanImage::createView(const ImageViewDesc& desc) {
    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.image = m_image;
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ci.format = formatToVk(desc.format != Format::UNKNOWN ? desc.format : m_desc.format);
    ci.subresourceRange.aspectMask = imageAspectToVk(desc.aspect);
    ci.subresourceRange.baseMipLevel = desc.baseMipLevel;
    ci.subresourceRange.levelCount = desc.mipLevelCount;
    ci.subresourceRange.baseArrayLayer = desc.baseArrayLayer;
    ci.subresourceRange.layerCount = desc.layerCount;

    VkImageView view = VK_NULL_HANDLE;
    if (vkCreateImageView(m_device, &ci, nullptr, &view) != VK_SUCCESS) {
        return nullptr;
    }

    ImageViewDesc actualDesc = desc;
    if (actualDesc.format == Format::UNKNOWN) {
        actualDesc.format = m_desc.format;
    }
    return new VulkanImageView(m_device, this, view, actualDesc);
}

// ──── VulkanImageView ────

VulkanImageView::~VulkanImageView() {
    if (m_view) {
        vkDestroyImageView(m_device, m_view, nullptr);
    }
}
