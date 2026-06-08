#include "vulkan_command_list.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_descriptor_set.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"
#include "vulkan_render_pass.h"
#include "vulkan_frame_buffer.h"

VulkanCommandList::~VulkanCommandList() {
    if (m_cmd && m_pool) {
        vkFreeCommandBuffers(m_device, m_pool, 1, &m_cmd);
    }
}

void VulkanCommandList::begin() {
    VkCommandBufferBeginInfo bi{};
    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_cmd, &bi);
}

void VulkanCommandList::end() {
    vkEndCommandBuffer(m_cmd);
}

void VulkanCommandList::reset() {
    vkResetCommandBuffer(m_cmd, 0);
}

void VulkanCommandList::resourceBarrier(uint32_t /*count*/,
                                         const ResourceBarrier* /*barriers*/) {
    // TODO: Implement pipeline barriers
}

void VulkanCommandList::setPipeline(IPipeline* pipeline) {
    auto* vkPipeline = static_cast<VulkanPipeline*>(pipeline);
    vkCmdBindPipeline(m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      vkPipeline->getHandle());
}

void VulkanCommandList::bindDescriptorSet(IPipelineLayout* layout,
                                           uint32_t setIndex,
                                           IDescriptorSet* set) {
    auto* vkLayout = static_cast<VulkanPipelineLayout*>(layout);
    auto* vkSet = static_cast<VulkanDescriptorSet*>(set);
    VkDescriptorSet sets[] = { vkSet->getHandle() };
    vkCmdBindDescriptorSets(m_cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vkLayout->getHandle(), setIndex, 1, sets, 0, nullptr);
}

void VulkanCommandList::setPushConstants(IPipelineLayout* layout,
                                          ShaderStage stage,
                                          uint32_t offset, uint32_t size,
                                          const void* data) {
    auto* vkLayout = static_cast<VulkanPipelineLayout*>(layout);
    vkCmdPushConstants(m_cmd, vkLayout->getHandle(),
                       shaderStageToVk(stage), offset, size, data);
}

void VulkanCommandList::setVertexBuffer(uint32_t binding, IBuffer* buffer,
                                         uint64_t offset) {
    auto* vkBuf = static_cast<VulkanBuffer*>(buffer);
    VkBuffer buffers[] = { vkBuf->getHandle() };
    VkDeviceSize offsets[] = { offset };
    vkCmdBindVertexBuffers(m_cmd, binding, 1, buffers, offsets);
}

void VulkanCommandList::setIndexBuffer(IBuffer* buffer, IndexType indexType) {
    auto* vkBuf = static_cast<VulkanBuffer*>(buffer);
    vkCmdBindIndexBuffer(m_cmd, vkBuf->getHandle(), 0, indexTypeToVk(indexType));
}

void VulkanCommandList::setViewport(const Viewport& viewport) {
    VkViewport vkViewport{};
    vkViewport.x = viewport.x;
    vkViewport.y = viewport.y;
    vkViewport.width = viewport.width;
    vkViewport.height = viewport.height;
    vkViewport.minDepth = viewport.minDepth;
    vkViewport.maxDepth = viewport.maxDepth;
    vkCmdSetViewport(m_cmd, 0, 1, &vkViewport);
}

void VulkanCommandList::setScissor(const Rect2D& scissor) {
    VkRect2D vkRect{};
    vkRect.offset.x = scissor.x;
    vkRect.offset.y = scissor.y;
    vkRect.extent.width = scissor.width;
    vkRect.extent.height = scissor.height;
    vkCmdSetScissor(m_cmd, 0, 1, &vkRect);
}

void VulkanCommandList::draw(uint32_t vertexCount, uint32_t instanceCount,
                              uint32_t firstVertex, uint32_t firstInstance) {
    vkCmdDraw(m_cmd, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandList::drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                     uint32_t firstIndex, int32_t vertexOffset,
                                     uint32_t firstInstance) {
    vkCmdDrawIndexed(m_cmd, indexCount, instanceCount, firstIndex,
                     vertexOffset, firstInstance);
}

void VulkanCommandList::dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) {
    vkCmdDispatch(m_cmd, groupX, groupY, groupZ);
}

void VulkanCommandList::copyBuffer(IBuffer* src, IBuffer* dst,
                                    uint64_t srcOffset, uint64_t dstOffset,
                                    uint64_t size) {
    auto* vkSrc = static_cast<VulkanBuffer*>(src);
    auto* vkDst = static_cast<VulkanBuffer*>(dst);
    VkBufferCopy region{};
    region.srcOffset = srcOffset;
    region.dstOffset = dstOffset;
    region.size = size;
    vkCmdCopyBuffer(m_cmd, vkSrc->getHandle(), vkDst->getHandle(), 1, &region);
}

void VulkanCommandList::copyBufferToImage(IBuffer* src, IImage* dst,
                                           uint64_t srcOffset,
                                           const ImageCopyRegion& region) {
    auto* vkSrc = static_cast<VulkanBuffer*>(src);
    auto* vkDst = static_cast<VulkanImage*>(dst);

    VkBufferImageCopy copy{};
    copy.bufferOffset = srcOffset;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = 0;
    copy.imageSubresource.baseArrayLayer = 0;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset.x = region.dstOffsetX;
    copy.imageOffset.y = region.dstOffsetY;
    copy.imageOffset.z = region.dstOffsetZ;
    copy.imageExtent.width = region.width;
    copy.imageExtent.height = region.height;
    copy.imageExtent.depth = region.depth;

    // TODO: Transition image layout before copy
    vkCmdCopyBufferToImage(m_cmd, vkSrc->getHandle(),
                           vkDst->getHandle(),
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &copy);
}

void VulkanCommandList::copyImageToBuffer(IImage* src, IBuffer* dst,
                                           const ImageCopyRegion& region,
                                           uint64_t dstOffset) {
    auto* vkSrc = static_cast<VulkanImage*>(src);
    auto* vkDst = static_cast<VulkanBuffer*>(dst);

    VkBufferImageCopy copy{};
    copy.bufferOffset = dstOffset;
    copy.bufferRowLength = 0;
    copy.bufferImageHeight = 0;
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.mipLevel = 0;
    copy.imageSubresource.baseArrayLayer = 0;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset.x = region.srcOffsetX;
    copy.imageOffset.y = region.srcOffsetY;
    copy.imageOffset.z = region.srcOffsetZ;
    copy.imageExtent.width = region.width;
    copy.imageExtent.height = region.height;
    copy.imageExtent.depth = region.depth;

    // TODO: Transition image layout before copy
    vkCmdCopyImageToBuffer(m_cmd, vkSrc->getHandle(),
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           vkDst->getHandle(), 1, &copy);
}

void VulkanCommandList::beginRenderPass(IRenderPass* renderPass,
                                         IFrameBuffer* framebuffer,
                                         const ClearColor* clearColors,
                                         uint32_t clearColorCount,
                                         ClearDepthStencil clearDepthStencil) {
    auto* vkRP = static_cast<VulkanRenderPass*>(renderPass);
    auto* vkFB = static_cast<VulkanFrameBuffer*>(framebuffer);

    std::vector<VkClearValue> clearValues(clearColorCount + 1);
    for (uint32_t i = 0; i < clearColorCount; ++i) {
        clearValues[i].color = { { clearColors[i].r, clearColors[i].g, clearColors[i].b, clearColors[i].a } };
    }
    clearValues[clearColorCount].depthStencil = { clearDepthStencil.depth, clearDepthStencil.stencil };

    VkRenderPassBeginInfo bi{};
    bi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    bi.renderPass = vkRP->getHandle();
    bi.framebuffer = vkFB->getHandle();
    bi.renderArea.extent.width = vkFB->getWidth();
    bi.renderArea.extent.height = vkFB->getHeight();
    bi.clearValueCount = (uint32_t)clearValues.size();
    bi.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_cmd, &bi, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandList::endRenderPass() {
    vkCmdEndRenderPass(m_cmd);
}
