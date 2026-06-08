#pragma once

#include "../rhi.h"
#include "vulkan_common.h"
#include <vector>
#include <cstdint>

class VulkanBuffer;
class VulkanImage;
class VulkanImageView;
class VulkanSampler;
class VulkanShader;
class VulkanPipeline;
class VulkanPipelineLayout;
class VulkanDescriptorSetLayout;
class VulkanDescriptorSet;
class VulkanCommandList;
class VulkanFence;
class VulkanSwapChain;

struct RenderPassDesc;
struct FrameBufferDesc;
class IRenderPass;
class IFrameBuffer;

struct VulkanQueues {
    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t computeFamily  = UINT32_MAX;
    uint32_t transferFamily = UINT32_MAX;
    uint32_t presentFamily  = UINT32_MAX;
    VkQueue  graphicsQueue  = VK_NULL_HANDLE;
    VkQueue  computeQueue   = VK_NULL_HANDLE;
    VkQueue  transferQueue  = VK_NULL_HANDLE;
    VkQueue  presentQueue   = VK_NULL_HANDLE;
};

class VulkanRHI : public IRHI {
public:
    VulkanRHI() = default;
    ~VulkanRHI() override;

    bool initialize(const RHIDesc& desc) override;
    void shutdown() override;
    void waitIdle() override;

    // Buffer
    IBuffer*        createBuffer(const BufferDesc& desc) override;
    void            destroyBuffer(IBuffer* buffer) override;

    // Image
    IImage*         createImage(const ImageDesc& desc) override;
    void            destroyImage(IImage* image) override;

    // ImageView
    IImageView*     createImageView(IImage* image, const ImageViewDesc& desc) override;
    void            destroyImageView(IImageView* view) override;

    // Sampler
    ISampler*       createSampler(const SamplerDesc& desc) override;
    void            destroySampler(ISampler* sampler) override;

    // Shader
    IShader*        createShader(const ShaderDesc& desc) override;
    void            destroyShader(IShader* shader) override;

    // Pipeline
    IPipeline*      createGraphicsPipeline(const GraphicsPipelineDesc& desc) override;
    IPipeline*      createComputePipeline(const ComputePipelineDesc& desc) override;
    void            destroyPipeline(IPipeline* pipeline) override;

    // PipelineLayout
    IPipelineLayout* createPipelineLayout(const PipelineLayoutDesc& desc) override;
    void             destroyPipelineLayout(IPipelineLayout* layout) override;

    // DescriptorSetLayout
    IDescriptorSetLayout* createDescriptorSetLayout(const DescriptorSetLayoutDesc& desc) override;
    void                  destroyDescriptorSetLayout(IDescriptorSetLayout* layout) override;

    // DescriptorSet
    IDescriptorSet* allocateDescriptorSet(IDescriptorSetLayout* layout) override;
    void            freeDescriptorSet(IDescriptorSet* set) override;

    // Bindless
    IDescriptorSet* createBindlessDescriptorSet(uint32_t maxCapacity, DescriptorType type) override;
    uint32_t        allocateBindlessIndex(IDescriptorSet* set) override;
    void            freeBindlessIndex(IDescriptorSet* set, uint32_t index) override;
    uint64_t        getBindlessGPUHandle(IDescriptorSet* set, uint32_t index) const override;

    // Update
    void            updateDescriptorSet(IDescriptorSet* set, const DescriptorUpdate& update) override;

    // Fence
    IFence*         createFence(bool signaled) override;
    void            destroyFence(IFence* fence) override;

    // SwapChain
    ISwapChain*     createSwapChain(const SwapChainDesc& desc) override;
    void            destroySwapChain(ISwapChain* swapChain) override;

    // RenderPass
    IRenderPass*    createRenderPass(const RenderPassDesc& desc) override;
    void            destroyRenderPass(IRenderPass* pass) override;

    // FrameBuffer
    IFrameBuffer*   createFrameBuffer(const FrameBufferDesc& desc) override;
    void            destroyFrameBuffer(IFrameBuffer* fb) override;

    // CommandList
    ICommandList*   createCommandList() override;
    void            destroyCommandList(ICommandList* cmd) override;

    // Queue
    void            submit(ICommandList* cmd, IFence* fence = nullptr) override;
    void            present(ISwapChain* swapChain) override;

    // Vulkan-specific accessors
    VkDevice        getDevice() const { return m_device; }
    VkInstance      getInstance() const { return m_instance; }
    VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    const VulkanQueues& getQueues() const { return m_queues; }
    VkCommandPool   getCommandPool() const { return m_commandPool; }
    VkDescriptorPool getDescriptorPool() const { return m_descriptorPool; }

private:
    bool createInstance(const RHIDesc& desc);
    bool setupDebugMessenger();
    bool pickPhysicalDevice();
    bool createDevice();
    void initBindlessHeap(uint32_t capacity);

    VkInstance              m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR            m_surface = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice        m_physicalDevice = VK_NULL_HANDLE;
    VkDevice                m_device = VK_NULL_HANDLE;
    VulkanQueues            m_queues;
    VkCommandPool           m_commandPool = VK_NULL_HANDLE;
    VkDescriptorPool        m_descriptorPool = VK_NULL_HANDLE;

    VkPhysicalDeviceFeatures m_enabledFeatures{};
    VkPhysicalDeviceFeatures2 m_enabledFeatures2{};

    std::vector<const char*> m_instanceExtensions;
    std::vector<const char*> m_deviceExtensions;
};
