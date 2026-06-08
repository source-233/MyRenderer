#pragma once

#include "../rhi_image.h"
#include "vulkan_common.h"

class VulkanImageView : public IImageView {
public:
    VulkanImageView(VkDevice device, IImage* image, VkImageView view,
                    const ImageViewDesc& desc)
        : m_device(device), m_image(image), m_view(view), m_desc(desc) {}
    ~VulkanImageView() override;

    IImage*         getImage() const override { return m_image; }
    ImageViewDesc   getDesc() const override { return m_desc; }
    VkImageView     getHandle() const { return m_view; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    IImage*         m_image = nullptr;
    VkImageView     m_view = VK_NULL_HANDLE;
    ImageViewDesc   m_desc{};
};

class VulkanImage : public IImage {
public:
    VulkanImage(VkDevice device, VkImage image, VkDeviceMemory memory,
                const ImageDesc& desc)
        : m_device(device), m_image(image), m_memory(memory), m_desc(desc) {}
    ~VulkanImage() override;

    ImageDesc       getDesc() const override { return m_desc; }
    IImageView*     createView(const ImageViewDesc& desc) override;

    VkImage         getHandle() const { return m_image; }

    void            setOwnsImage(bool owns) { m_ownsImage = owns; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkImage         m_image = VK_NULL_HANDLE;
    VkDeviceMemory  m_memory = VK_NULL_HANDLE;
    ImageDesc       m_desc{};
    bool            m_ownsImage = true;
};
