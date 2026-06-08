#pragma once

#include "../rhi_swap_chain.h"
#include "../rhi_image.h"
#include "vulkan_common.h"
#include "vulkan_image.h"
#include <vector>

class VulkanSwapChain : public ISwapChain {
public:
    VulkanSwapChain(VkDevice device, VkQueue presentQueue, VkSwapchainKHR swapChain,
                    uint32_t width, uint32_t height, Format format)
        : m_device(device), m_presentQueue(presentQueue), m_swapChain(swapChain)
        , m_width(width), m_height(height), m_format(format) {}
    ~VulkanSwapChain() override;

    bool            present() override;
    uint32_t        getCurrentImageIndex() const override { return m_currentImage; }
    IImage*         getCurrentImage() const override;
    uint32_t        getWidth() const override { return m_width; }
    uint32_t        getHeight() const override { return m_height; }
    Format          getFormat() const override { return m_format; }

    void            setImages(VkImage* images, uint32_t count);
    VkSwapchainKHR  getSwapChain() const { return m_swapChain; }
    VkQueue         getPresentQueue() const { return m_presentQueue; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkQueue         m_presentQueue = VK_NULL_HANDLE;
    VkSwapchainKHR  m_swapChain = VK_NULL_HANDLE;
    uint32_t        m_width = 0;
    uint32_t        m_height = 0;
    Format          m_format = Format::UNKNOWN;
    uint32_t        m_currentImage = 0;

    std::vector<VkImage>        m_vkImages;
    std::vector<VulkanImage*>   m_images;
};
