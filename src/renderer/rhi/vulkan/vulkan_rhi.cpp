#include "vulkan_rhi.h"
#include "vulkan_render_pass.h"
#include "vulkan_frame_buffer.h"
#include "vulkan_shader.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_descriptor_set_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"
#include "vulkan_swap_chain.h"
#include "../rhi_buffer.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <climits>

// For now, all create/destroy methods are skeletons.
// The initialize() and shutdown() methods are fully implemented.

// ──── Lifecycle ────

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT /*type*/,
    const VkDebugUtilsMessengerCallbackDataEXT* data,
    void* /*userData*/)
{
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf(stderr, "[Vulkan] %s\n", data->pMessage);
    }
    return VK_FALSE;
}

bool VulkanRHI::createInstance(const RHIDesc& desc) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = desc.appName;
    appInfo.applicationVersion = desc.appVersion;
    appInfo.apiVersion = VK_API_VERSION_1_3;

    m_instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    // Platform-specific surface extension
#ifdef VK_USE_PLATFORM_WIN32_KHR
    m_instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    m_instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    m_instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif

    if (desc.enableValidation) {
        m_instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = (uint32_t)m_instanceExtensions.size();
    createInfo.ppEnabledExtensionNames = m_instanceExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
    if (desc.enableValidation) {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = layers;

        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;

        createInfo.pNext = &debugCreateInfo;
    }

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_instance));
    volkLoadInstance(m_instance);
    return true;
}

bool VulkanRHI::setupDebugMessenger() {
    // Already set up via pNext in createInstance
    // Fetch the messenger handle
    auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
    if (!vkCreateDebugUtilsMessengerEXT) return false;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;

    VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger));
    return true;
}

bool VulkanRHI::pickPhysicalDevice() {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(m_instance, &count, nullptr);
    if (count == 0) return false;

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(m_instance, &count, devices.data());

    // Prefer discrete GPU
    for (const auto& dev : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(dev, &props);
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            m_physicalDevice = dev;
            return true;
        }
    }

    // Fallback to first device
    m_physicalDevice = devices[0];
    return true;
}

bool VulkanRHI::createDevice() {
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &props);

    // Find queue families
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueCount, queueProps.data());

    for (uint32_t i = 0; i < queueCount; ++i) {
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            m_queues.graphicsFamily = i;
        }
        if (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            m_queues.computeFamily = i;
        }
        if (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            m_queues.transferFamily = i;
        }
    }

    // Require graphics queue
    if (m_queues.graphicsFamily == UINT32_MAX) return false;

    // Use same queue for all if only one family exists
    if (m_queues.computeFamily == UINT32_MAX)  m_queues.computeFamily  = m_queues.graphicsFamily;
    if (m_queues.transferFamily == UINT32_MAX) m_queues.transferFamily = m_queues.graphicsFamily;
    m_queues.presentFamily = m_queues.graphicsFamily;

    const float queuePriority = 1.0f;
    std::vector<uint32_t> uniqueFamilies = {
        m_queues.graphicsFamily, m_queues.computeFamily,
        m_queues.transferFamily, m_queues.presentFamily
    };
    std::sort(uniqueFamilies.begin(), uniqueFamilies.end());
    uniqueFamilies.erase(std::unique(uniqueFamilies.begin(), uniqueFamilies.end()), uniqueFamilies.end());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t fam : uniqueFamilies) {
        VkDeviceQueueCreateInfo qci{};
        qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        qci.queueFamilyIndex = fam;
        qci.queueCount = 1;
        qci.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(qci);
    }

    m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    // Enable features
    VkPhysicalDeviceFeatures features{};
    features.samplerAnisotropy = VK_TRUE;

    VkPhysicalDeviceVulkan12Features features12{};
    features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features12.bufferDeviceAddress = VK_TRUE;
    features12.descriptorIndexing = VK_TRUE;
    features12.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
    features12.shaderStorageBufferArrayNonUniformIndexing = VK_TRUE;
    features12.runtimeDescriptorArray = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = &features12;
    createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledExtensionCount = (uint32_t)m_deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();
    createInfo.pEnabledFeatures = &features;

    VK_CHECK(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device));
    volkLoadDevice(m_device);

    // Get queues
    vkGetDeviceQueue(m_device, m_queues.graphicsFamily, 0, &m_queues.graphicsQueue);
    vkGetDeviceQueue(m_device, m_queues.computeFamily, 0, &m_queues.computeQueue);
    vkGetDeviceQueue(m_device, m_queues.transferFamily, 0, &m_queues.transferQueue);
    m_queues.presentQueue = m_queues.graphicsQueue;

    // Command pool
    VkCommandPoolCreateInfo poolCI{};
    poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolCI.queueFamilyIndex = m_queues.graphicsFamily;
    VK_CHECK(vkCreateCommandPool(m_device, &poolCI, nullptr, &m_commandPool));

    return true;
}

void VulkanRHI::initBindlessHeap(uint32_t capacity) {
    if (capacity == 0) return;

    // TODO: Create a descriptor pool large enough for bindless usage.
    // This requires VK_EXT_descriptor_indexing loaded.
}

bool VulkanRHI::initialize(const RHIDesc& desc) {
    if (volkInitialize() != VK_SUCCESS) {
        fprintf(stderr, "Failed to load Vulkan loader\n");
        return false;
    }
    if (!createInstance(desc)) {
        fprintf(stderr, "Failed to create Vulkan instance\n");
        return false;
    }
    if (!setupDebugMessenger()) {
        fprintf(stderr, "Failed to setup debug messenger\n");
        // Non-fatal
    }
    if (!pickPhysicalDevice()) {
        fprintf(stderr, "Failed to find a Vulkan physical device\n");
        return false;
    }
    if (!createDevice()) {
        fprintf(stderr, "Failed to create Vulkan device\n");
        return false;
    }
    initBindlessHeap(desc.bindlessCapacity);
    return true;
}

void VulkanRHI::shutdown() {
    waitIdle();

    if (m_commandPool) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
    if (m_descriptorPool) {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }
    if (m_device) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
    if (m_surface) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
    if (m_debugMessenger) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
            vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func) func(m_instance, m_debugMessenger, nullptr);
        m_debugMessenger = VK_NULL_HANDLE;
    }
    if (m_instance) {
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}

void VulkanRHI::waitIdle() {
    if (m_device) {
        vkDeviceWaitIdle(m_device);
    }
}

VulkanRHI::~VulkanRHI() {
    shutdown();
}

// ──── Buffer ────

static uint32_t findMemoryType(VkPhysicalDevice physDev, uint32_t typeFilter, VkMemoryPropertyFlags props) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physDev, &memProps);
    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }
    return UINT32_MAX;
}

IBuffer* VulkanRHI::createBuffer(const BufferDesc& desc) {
    VkBufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    ci.size = desc.size;
    ci.usage = bufferUsageToVk(desc.usage);
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VK_CHECK(vkCreateBuffer(m_device, &ci, nullptr, &buffer));

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(m_device, buffer, &memReq);

    VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (desc.hostVisible) {
        props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    uint32_t memType = findMemoryType(m_physicalDevice, memReq.memoryTypeBits, props);
    if (memType == UINT32_MAX) {
        vkDestroyBuffer(m_device, buffer, nullptr);
        return nullptr;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = memType;

    VkMemoryAllocateFlagsInfo flagsInfo{};
    if (desc.usage & BufferUsage::STORAGE) {
        flagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        flagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
        allocInfo.pNext = &flagsInfo;
    }

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateMemory(m_device, &allocInfo, nullptr, &memory));
    VK_CHECK(vkBindBufferMemory(m_device, buffer, memory, 0));

    return new VulkanBuffer(m_device, buffer, memory, desc);
}

void VulkanRHI::destroyBuffer(IBuffer* buffer) {
    delete buffer;
}

// ──── Image ────

IImage* VulkanRHI::createImage(const ImageDesc& /*desc*/) {
    // TODO: Implement actual Vulkan image creation
    return nullptr;
}

void VulkanRHI::destroyImage(IImage* image) {
    delete image;
}

IImageView* VulkanRHI::createImageView(IImage* /*image*/, const ImageViewDesc& /*desc*/) {
    // TODO: Implement actual Vulkan image view creation
    return nullptr;
}

void VulkanRHI::destroyImageView(IImageView* view) {
    delete view;
}

// ──── Sampler ────

ISampler* VulkanRHI::createSampler(const SamplerDesc& /*desc*/) {
    // TODO: Implement actual Vulkan sampler creation
    return nullptr;
}

void VulkanRHI::destroySampler(ISampler* sampler) {
    delete sampler;
}

// ──── Shader ────

IShader* VulkanRHI::createShader(const ShaderDesc& desc) {
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = desc.codeSize;
    ci.pCode = (const uint32_t*)desc.code;

    VkShaderModule module = VK_NULL_HANDLE;
    VK_CHECK(vkCreateShaderModule(m_device, &ci, nullptr, &module));
    return new VulkanShader(m_device, module, desc);
}

void VulkanRHI::destroyShader(IShader* shader) {
    delete shader;
}

// ──── Pipeline ────

IPipeline* VulkanRHI::createGraphicsPipeline(const GraphicsPipelineDesc& desc) {
    auto* vert = static_cast<VulkanShader*>(desc.vertexShader);
    auto* frag = static_cast<VulkanShader*>(desc.fragmentShader);
    auto* layout = static_cast<VulkanPipelineLayout*>(desc.layout);

    VkPipelineShaderStageCreateInfo stages[2] = {};
    stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vert->getModule();
    stages[0].pName = "main";
    stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = frag->getModule();
    stages[1].pName = "main";

    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    std::vector<VkVertexInputBindingDescription> bindings;
    for (const auto& vs : desc.vertexStreams) {
        VkVertexInputBindingDescription b{};
        b.binding = vs.binding;
        b.stride = vs.stride;
        b.inputRate = vs.inputRateInstance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        bindings.push_back(b);
    }

    std::vector<VkVertexInputAttributeDescription> attributes;
    for (const auto& va : desc.vertexAttributes) {
        VkVertexInputAttributeDescription a{};
        a.location = va.location;
        a.binding = 0;
        a.format = formatToVk(va.format);
        a.offset = va.offset;
        attributes.push_back(a);
    }

    vertexInput.vertexBindingDescriptionCount = (uint32_t)bindings.size();
    vertexInput.pVertexBindingDescriptions = bindings.data();
    vertexInput.vertexAttributeDescriptionCount = (uint32_t)attributes.size();
    vertexInput.pVertexAttributeDescriptions = attributes.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo raster{};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode = polygonModeToVk(desc.rasterization.polygonMode);
    raster.cullMode = cullModeToVk(desc.rasterization.cullMode);
    raster.frontFace = frontFaceToVk(desc.rasterization.frontFace);
    raster.lineWidth = desc.rasterization.lineWidth;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = desc.depthStencil.depthTest;
    depthStencil.depthWriteEnable = desc.depthStencil.depthWrite;
    depthStencil.depthCompareOp = compareOpToVk(desc.depthStencil.depthCompareOp);
    depthStencil.stencilTestEnable = desc.depthStencil.stencilTest;

    VkPipelineColorBlendAttachmentState blendAtt{};
    blendAtt.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachments;
    for (const auto& ba : desc.blendAttachments) {
        VkPipelineColorBlendAttachmentState b{};
        b.blendEnable = ba.blendEnable;
        b.srcColorBlendFactor = blendFactorToVk(ba.srcColorBlendFactor);
        b.dstColorBlendFactor = blendFactorToVk(ba.dstColorBlendFactor);
        b.colorBlendOp = blendOpToVk(ba.colorBlendOp);
        b.srcAlphaBlendFactor = blendFactorToVk(ba.srcAlphaBlendFactor);
        b.dstAlphaBlendFactor = blendFactorToVk(ba.dstAlphaBlendFactor);
        b.alphaBlendOp = blendOpToVk(ba.alphaBlendOp);
        b.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                           VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachments.push_back(b);
    }

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.attachmentCount = (uint32_t)blendAttachments.size();
    colorBlend.pAttachments = blendAttachments.data();

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamic{};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamic.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.stageCount = 2;
    ci.pStages = stages;
    ci.pVertexInputState = &vertexInput;
    ci.pInputAssemblyState = &inputAssembly;
    ci.pViewportState = &viewportState;
    ci.pRasterizationState = &raster;
    ci.pMultisampleState = &multisampling;
    ci.pDepthStencilState = &depthStencil;
    ci.pColorBlendState = &colorBlend;
    ci.pDynamicState = &dynamic;
    ci.layout = layout->getHandle();
    ci.renderPass = VK_NULL_HANDLE;

    VkPipeline pipeline = VK_NULL_HANDLE;
    VK_CHECK(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &ci, nullptr, &pipeline));
    return new VulkanPipeline(m_device, pipeline, layout->getHandle());
}

IPipeline* VulkanRHI::createComputePipeline(const ComputePipelineDesc& /*desc*/) {
    // TODO: Implement actual compute pipeline creation
    return nullptr;
}

void VulkanRHI::destroyPipeline(IPipeline* pipeline) {
    delete pipeline;
}

// ──── PipelineLayout ────

IPipelineLayout* VulkanRHI::createPipelineLayout(const PipelineLayoutDesc& desc) {
    std::vector<VkDescriptorSetLayout> layouts;
    layouts.reserve(desc.setLayouts.size());
    for (auto* sl : desc.setLayouts) {
        layouts.push_back(static_cast<VulkanDescriptorSetLayout*>(sl)->getHandle());
    }

    std::vector<VkPushConstantRange> pcRanges;
    pcRanges.reserve(desc.pushConstants.size());
    for (const auto& pc : desc.pushConstants) {
        VkPushConstantRange range{};
        range.stageFlags = shaderStageToVk(pc.stage);
        range.offset = pc.offset;
        range.size = pc.size;
        pcRanges.push_back(range);
    }

    VkPipelineLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    ci.setLayoutCount = (uint32_t)layouts.size();
    ci.pSetLayouts = layouts.data();
    ci.pushConstantRangeCount = (uint32_t)pcRanges.size();
    ci.pPushConstantRanges = pcRanges.data();

    VkPipelineLayout layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(m_device, &ci, nullptr, &layout));
    return new VulkanPipelineLayout(m_device, layout, desc);
}

void VulkanRHI::destroyPipelineLayout(IPipelineLayout* layout) {
    delete layout;
}

// ──── DescriptorSetLayout ────

IDescriptorSetLayout* VulkanRHI::createDescriptorSetLayout(const DescriptorSetLayoutDesc& /*desc*/) {
    // TODO: Implement actual descriptor set layout creation
    return nullptr;
}

void VulkanRHI::destroyDescriptorSetLayout(IDescriptorSetLayout* layout) {
    delete layout;
}

// ──── DescriptorSet (traditional) ────

IDescriptorSet* VulkanRHI::allocateDescriptorSet(IDescriptorSetLayout* /*layout*/) {
    // TODO: Implement
    return nullptr;
}

void VulkanRHI::freeDescriptorSet(IDescriptorSet* set) {
    delete set;
}

// ──── Bindless ────

IDescriptorSet* VulkanRHI::createBindlessDescriptorSet(uint32_t /*maxCapacity*/, DescriptorType /*type*/) {
    // TODO: Implement bindless descriptor set creation
    return nullptr;
}

uint32_t VulkanRHI::allocateBindlessIndex(IDescriptorSet* /*set*/) {
    // TODO: Implement
    return 0;
}

void VulkanRHI::freeBindlessIndex(IDescriptorSet* /*set*/, uint32_t /*index*/) {
    // TODO: Implement
}

uint64_t VulkanRHI::getBindlessGPUHandle(IDescriptorSet* /*set*/, uint32_t /*index*/) const {
    // TODO: Implement
    return 0;
}

// ──── Update ────

void VulkanRHI::updateDescriptorSet(IDescriptorSet* /*set*/, const DescriptorUpdate& /*update*/) {
    // TODO: Implement
}

// ──── Fence ────

IFence* VulkanRHI::createFence(bool /*signaled*/) {
    // TODO: Implement
    return nullptr;
}

void VulkanRHI::destroyFence(IFence* fence) {
    delete fence;
}

// ──── SwapChain ────

ISwapChain* VulkanRHI::createSwapChain(const SwapChainDesc& desc) {
    auto* window = static_cast<GLFWwindow*>(desc.windowHandle);
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VK_CHECK(glfwCreateWindowSurface(m_instance, window, nullptr, &surface));
    m_surface = surface;

    VkBool32 presentSupport = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_queues.graphicsFamily, surface, &presentSupport);
    if (!presentSupport) return nullptr;

    // Surface capabilities
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, surface, &caps);

    // Choose format
    uint32_t fmtCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &fmtCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(fmtCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &fmtCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = f;
            break;
        }
    }

    // Choose present mode
    uint32_t pmCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &pmCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(pmCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &pmCount, presentModes.data());

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& pm : presentModes) {
        if (pm == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = pm;
            break;
        }
    }

    // Extent
    VkExtent2D extent = caps.currentExtent;
    if (extent.width == UINT32_MAX) {
        extent.width = std::clamp(desc.width, caps.minImageExtent.width, caps.maxImageExtent.width);
        extent.height = std::clamp(desc.height, caps.minImageExtent.height, caps.maxImageExtent.height);
    }

    uint32_t imageCount = std::clamp(desc.bufferCount, caps.minImageCount, caps.maxImageCount);

    VkSwapchainCreateInfoKHR ci{};
    ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    ci.surface = surface;
    ci.minImageCount = imageCount;
    ci.imageFormat = surfaceFormat.format;
    ci.imageColorSpace = surfaceFormat.colorSpace;
    ci.imageExtent = extent;
    ci.imageArrayLayers = 1;
    ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ci.preTransform = caps.currentTransform;
    ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    ci.presentMode = presentMode;
    ci.clipped = VK_TRUE;

    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VK_CHECK(vkCreateSwapchainKHR(m_device, &ci, nullptr, &swapChain));

    // Get images
    uint32_t imgCount = 0;
    vkGetSwapchainImagesKHR(m_device, swapChain, &imgCount, nullptr);
    std::vector<VkImage> images(imgCount);
    vkGetSwapchainImagesKHR(m_device, swapChain, &imgCount, images.data());

    Format fmt = Format::UNKNOWN;
    for (auto f = Format::R8_UNORM; f <= Format::D32_SFLOAT_S8_UINT; f = (Format)((uint32_t)f + 1)) {
        if (formatToVk(f) == surfaceFormat.format) {
            fmt = f;
            break;
        }
    }

    auto* vkSC = new VulkanSwapChain(m_device, m_queues.presentQueue, swapChain, extent.width, extent.height, fmt);
    vkSC->setImages(images.data(), imgCount);
    return vkSC;
}

void VulkanRHI::destroySwapChain(ISwapChain* swapChain) {
    if (m_surface) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
    delete swapChain;
}

// ──── RenderPass ────

IRenderPass* VulkanRHI::createRenderPass(const RenderPassDesc& desc) {
    std::vector<VkAttachmentDescription> attachments;
    attachments.reserve(desc.attachments.size());
    for (const auto& a : desc.attachments) {
        VkAttachmentDescription vkAtt{};
        vkAtt.format = formatToVk(a.format);
        vkAtt.samples = VK_SAMPLE_COUNT_1_BIT;
        vkAtt.loadOp = a.loadOpClear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        vkAtt.storeOp = a.storeOpTrue ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
        vkAtt.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        vkAtt.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        vkAtt.initialLayout = imageLayoutToVk(a.initialLayout);
        vkAtt.finalLayout = imageLayoutToVk(a.finalLayout);
        attachments.push_back(vkAtt);
    }

    std::vector<VkSubpassDescription> subpasses;
    std::vector<std::vector<VkAttachmentReference>> colorRefs(desc.subpasses.size());
    std::vector<VkAttachmentReference> dsRefs(desc.subpasses.size());
    for (size_t i = 0; i < desc.subpasses.size(); ++i) {
        VkSubpassDescription sp{};
        sp.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

        for (auto idx : desc.subpasses[i].colorAttachments) {
            VkAttachmentReference ref{};
            ref.attachment = idx;
            ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorRefs[i].push_back(ref);
        }
        sp.colorAttachmentCount = (uint32_t)colorRefs[i].size();
        sp.pColorAttachments = colorRefs[i].data();

        if (desc.subpasses[i].depthStencilAttachment != UINT32_MAX) {
            dsRefs[i].attachment = desc.subpasses[i].depthStencilAttachment;
            dsRefs[i].layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            sp.pDepthStencilAttachment = &dsRefs[i];
        }

        subpasses.push_back(sp);
    }

    VkRenderPassCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    ci.attachmentCount = (uint32_t)attachments.size();
    ci.pAttachments = attachments.data();
    ci.subpassCount = (uint32_t)subpasses.size();
    ci.pSubpasses = subpasses.data();

    VkRenderPass pass = VK_NULL_HANDLE;
    VK_CHECK(vkCreateRenderPass(m_device, &ci, nullptr, &pass));
    return new VulkanRenderPass(m_device, pass, desc);
}

void VulkanRHI::destroyRenderPass(IRenderPass* pass) {
    delete pass;
}

// ──── FrameBuffer ────

IFrameBuffer* VulkanRHI::createFrameBuffer(const FrameBufferDesc& desc) {
    auto* vkPass = static_cast<VulkanRenderPass*>(desc.renderPass);
    std::vector<VkImageView> attachments;
    for (auto* imgView : desc.attachments) {
        auto* vkView = static_cast<VulkanImageView*>(imgView);
        attachments.push_back(vkView->getHandle());
    }

    VkFramebufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    ci.renderPass = vkPass->getHandle();
    ci.attachmentCount = (uint32_t)attachments.size();
    ci.pAttachments = attachments.data();
    ci.width = desc.width;
    ci.height = desc.height;
    ci.layers = 1;

    VkFramebuffer fb = VK_NULL_HANDLE;
    VK_CHECK(vkCreateFramebuffer(m_device, &ci, nullptr, &fb));
    return new VulkanFrameBuffer(m_device, fb, desc);
}

void VulkanRHI::destroyFrameBuffer(IFrameBuffer* fb) {
    delete fb;
}

// ──── CommandList ────

ICommandList* VulkanRHI::createCommandList() {
    // TODO: Implement
    return nullptr;
}

void VulkanRHI::destroyCommandList(ICommandList* cmd) {
    delete cmd;
}

// ──── Queue ────

void VulkanRHI::submit(ICommandList* /*cmd*/, IFence* /*fence*/) {
    // TODO: Implement
}

void VulkanRHI::present(ISwapChain* /*swapChain*/) {
    // TODO: Implement
}
