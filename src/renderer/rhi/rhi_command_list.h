#pragma once

#include "rhi_enums.h"
#include "rhi_resource_barrier.h"
#include <cstdint>

class IPipeline;
class IPipelineLayout;
class IDescriptorSet;
class IBuffer;
class IImage;
class IRenderPass;
class IFrameBuffer;

struct ClearColor { float r = 0, g = 0, b = 0, a = 1; };
struct ClearDepthStencil { float depth = 1.0f; uint32_t stencil = 0; };

struct Viewport {
    float x = 0, y = 0, width = 0, height = 0;
    float minDepth = 0, maxDepth = 1;
};

struct Rect2D {
    int32_t x = 0, y = 0;
    uint32_t width = 0, height = 0;
};

class ICommandList {
public:
    virtual ~ICommandList() = default;

    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void reset() = 0;

    virtual void resourceBarrier(uint32_t count, const ResourceBarrier* barriers) = 0;

    virtual void setPipeline(IPipeline* pipeline) = 0;
    virtual void bindDescriptorSet(IPipelineLayout* layout, uint32_t setIndex,
                                   IDescriptorSet* set) = 0;
    virtual void setPushConstants(IPipelineLayout* layout, ShaderStage stage,
                                  uint32_t offset, uint32_t size, const void* data) = 0;

    virtual void setVertexBuffer(uint32_t binding, IBuffer* buffer,
                                  uint64_t offset) = 0;
    virtual void setIndexBuffer(IBuffer* buffer, IndexType indexType) = 0;

    virtual void setViewport(const Viewport& viewport) = 0;
    virtual void setScissor(const Rect2D& scissor) = 0;

    virtual void draw(uint32_t vertexCount, uint32_t instanceCount,
                      uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                             uint32_t firstIndex, int32_t vertexOffset,
                             uint32_t firstInstance) = 0;

    virtual void dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;

    virtual void copyBuffer(IBuffer* src, IBuffer* dst, uint64_t srcOffset,
                             uint64_t dstOffset, uint64_t size) = 0;
    virtual void copyBufferToImage(IBuffer* src, IImage* dst,
                                    uint64_t srcOffset, const ImageCopyRegion& region) = 0;
    virtual void copyImageToBuffer(IImage* src, IBuffer* dst,
                                    const ImageCopyRegion& region, uint64_t dstOffset) = 0;

    virtual void beginRenderPass(IRenderPass* renderPass,
                                  IFrameBuffer* framebuffer,
                                  const ClearColor* clearColors,
                                  uint32_t clearColorCount,
                                  ClearDepthStencil clearDepthStencil) = 0;
    virtual void endRenderPass() = 0;
};
