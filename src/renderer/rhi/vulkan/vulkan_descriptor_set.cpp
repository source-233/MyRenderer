#include "vulkan_descriptor_set.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"
#include "vulkan_sampler.h"

VulkanDescriptorSet::~VulkanDescriptorSet() {
    // Descriptor sets are freed via vkFreeDescriptorSets, handled by the pool
}

uint64_t VulkanDescriptorSet::getGPUHandle(uint32_t /*index*/) const {
    // TODO: For bindless, return the descriptor GPU handle.
    // In Vulkan, this would be the VkDescriptorSet cast to uint64_t (for non-bindless)
    // or an index into a GPU-visible descriptor heap.
    return reinterpret_cast<uint64_t>(m_set);
}

void VulkanDescriptorSet::write(const DescriptorWrite& write) {
    VkWriteDescriptorSet w{};
    w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    w.dstSet = m_set;
    w.dstBinding = write.binding;
    w.dstArrayElement = write.arrayIndex;
    w.descriptorCount = 1;
    w.descriptorType = descriptorTypeToVk(write.type);

    VkDescriptorBufferInfo bufferInfo{};
    VkDescriptorImageInfo  imageInfo{};

    switch (write.type) {
        case DescriptorType::UNIFORM_BUFFER:
        case DescriptorType::STORAGE_BUFFER: {
            auto* vkBuf = static_cast<VulkanBuffer*>(write.buffer);
            bufferInfo.buffer = vkBuf->getHandle();
            bufferInfo.offset = write.bufferOffset;
            bufferInfo.range = write.bufferRange;
            w.pBufferInfo = &bufferInfo;
            break;
        }
        case DescriptorType::SAMPLED_IMAGE:
        case DescriptorType::STORAGE_IMAGE: {
            auto* vkView = static_cast<VulkanImageView*>(write.imageView);
            imageInfo.imageView = vkView->getHandle();
            imageInfo.imageLayout = imageLayoutToVk(write.imageLayout);
            w.pImageInfo = &imageInfo;
            break;
        }
        case DescriptorType::COMBINED_IMAGE_SAMPLER: {
            auto* vkView = static_cast<VulkanImageView*>(write.imageView);
            imageInfo.imageView = vkView->getHandle();
            imageInfo.imageLayout = imageLayoutToVk(write.imageLayout);
            if (write.sampler) {
                auto* vkSampler = static_cast<VulkanSampler*>(write.sampler);
                imageInfo.sampler = vkSampler->getHandle();
            }
            w.pImageInfo = &imageInfo;
            break;
        }
        case DescriptorType::SAMPLER: {
            if (write.sampler) {
                auto* vkSampler = static_cast<VulkanSampler*>(write.sampler);
                imageInfo.sampler = vkSampler->getHandle();
            }
            w.pImageInfo = &imageInfo;
            break;
        }
    }

    vkUpdateDescriptorSets(m_device, 1, &w, 0, nullptr);
}
