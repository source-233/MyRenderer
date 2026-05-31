#pragma once

#include <cstdint>

#include "rhi_enums.h"
#include "rhi_resource_barrier.h"

class IBuffer;
struct BufferDesc;

class IImage;
class IImageView;
struct ImageDesc;
struct ImageViewDesc;

class ISampler;
struct SamplerDesc;

class IShader;
struct ShaderDesc;

class IPipeline;
struct GraphicsPipelineDesc;
struct ComputePipelineDesc;

class IPipelineLayout;
struct PipelineLayoutDesc;

class IDescriptorSetLayout;
struct DescriptorSetLayoutDesc;

class IDescriptorSet;
struct DescriptorUpdate;

class IFence;

class ISwapChain;
struct SwapChainDesc;

class ICommandList;

struct RHIDesc {
    const char*     appName = "MyRenderer";
    uint32_t        appVersion = 1;
    uint32_t        bindlessCapacity = 0;
    bool            enableValidation = true;
};

class IRHI {
public:
    virtual ~IRHI() = default;

    virtual bool initialize(const RHIDesc& desc) = 0;
    virtual void shutdown() = 0;
    virtual void waitIdle() = 0;

    // Buffer
    virtual IBuffer*        createBuffer(const BufferDesc& desc) = 0;
    virtual void            destroyBuffer(IBuffer* buffer) = 0;

    // Image
    virtual IImage*         createImage(const ImageDesc& desc) = 0;
    virtual void            destroyImage(IImage* image) = 0;

    // ImageView
    virtual IImageView*     createImageView(IImage* image, const ImageViewDesc& desc) = 0;
    virtual void            destroyImageView(IImageView* view) = 0;

    // Sampler
    virtual ISampler*       createSampler(const SamplerDesc& desc) = 0;
    virtual void            destroySampler(ISampler* sampler) = 0;

    // Shader
    virtual IShader*        createShader(const ShaderDesc& desc) = 0;
    virtual void            destroyShader(IShader* shader) = 0;

    // Pipeline
    virtual IPipeline*      createGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
    virtual IPipeline*      createComputePipeline(const ComputePipelineDesc& desc) = 0;
    virtual void            destroyPipeline(IPipeline* pipeline) = 0;

    // PipelineLayout
    virtual IPipelineLayout* createPipelineLayout(const PipelineLayoutDesc& desc) = 0;
    virtual void             destroyPipelineLayout(IPipelineLayout* layout) = 0;

    // DescriptorSetLayout
    virtual IDescriptorSetLayout* createDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) = 0;
    virtual void                  destroyDescriptorSetLayout(IDescriptorSetLayout* layout) = 0;

    // DescriptorSet (traditional path)
    virtual IDescriptorSet* allocateDescriptorSet(IDescriptorSetLayout* layout) = 0;
    virtual void            freeDescriptorSet(IDescriptorSet* set) = 0;

    // DescriptorSet (bindless path)
    virtual IDescriptorSet* createBindlessDescriptorSet(uint32_t maxCapacity, DescriptorType type) = 0;
    virtual uint32_t        allocateBindlessIndex(IDescriptorSet* set) = 0;
    virtual void            freeBindlessIndex(IDescriptorSet* set, uint32_t index) = 0;
    virtual uint64_t        getBindlessGPUHandle(IDescriptorSet* set, uint32_t index) const = 0;

    // Update descriptor
    virtual void            updateDescriptorSet(IDescriptorSet* set, const DescriptorUpdate& update) = 0;

    // Fence
    virtual IFence*         createFence(bool signaled) = 0;
    virtual void            destroyFence(IFence* fence) = 0;

    // SwapChain
    virtual ISwapChain*     createSwapChain(const SwapChainDesc& desc) = 0;
    virtual void            destroySwapChain(ISwapChain* swapChain) = 0;

    // CommandList
    virtual ICommandList*   createCommandList() = 0;
    virtual void            destroyCommandList(ICommandList* cmd) = 0;

    // Queue
    virtual void            submit(ICommandList* cmd, IFence* fence = nullptr) = 0;
    virtual void            present(ISwapChain* swapChain) = 0;
};
