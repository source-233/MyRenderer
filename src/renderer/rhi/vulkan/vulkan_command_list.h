#pragma once

#include "../rhi_command_list.h"
#include "vulkan_common.h"

class VulkanCommandList : public ICommandList {
public:
    VulkanCommandList(VkDevice device, VkCommandBuffer cmd, VkCommandPool pool)
        : m_device(device), m_cmd(cmd), m_pool(pool) {}
    ~VulkanCommandList() override;

    void begin() override;
    void end() override;
    void reset() override;

    void resourceBarrier(uint32_t count, const ResourceBarrier* barriers) override;

    void setPipeline(IPipeline* pipeline) override;
    void bindDescriptorSet(IPipelineLayout* layout, uint32_t setIndex,
                           IDescriptorSet* set) override;
    void setPushConstants(IPipelineLayout* layout, ShaderStage stage,
                          uint32_t offset, uint32_t size, const void* data) override;

    void setVertexBuffer(uint32_t binding, IBuffer* buffer,
                          uint64_t offset) override;
    void setIndexBuffer(IBuffer* buffer, IndexType indexType) override;

    void setViewport(const Viewport& viewport) override;
    void setScissor(const Rect2D& scissor) override;

    void draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance) override;
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance) override;

    void dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

    void copyBuffer(IBuffer* src, IBuffer* dst, uint64_t srcOffset,
                     uint64_t dstOffset, uint64_t size) override;
    void copyBufferToImage(IBuffer* src, IImage* dst,
                            uint64_t srcOffset, const ImageCopyRegion& region) override;
    void copyImageToBuffer(IImage* src, IBuffer* dst,
                            const ImageCopyRegion& region, uint64_t dstOffset) override;

    void beginRenderPass(IRenderPass* renderPass,
                          IFrameBuffer* framebuffer,
                          const ClearColor* clearColors,
                          uint32_t clearColorCount,
                          ClearDepthStencil clearDepthStencil) override;
    void endRenderPass() override;

    VkCommandBuffer getHandle() const { return m_cmd; }

private:
    VkDevice        m_device = VK_NULL_HANDLE;
    VkCommandBuffer m_cmd = VK_NULL_HANDLE;
    VkCommandPool   m_pool = VK_NULL_HANDLE;
};
