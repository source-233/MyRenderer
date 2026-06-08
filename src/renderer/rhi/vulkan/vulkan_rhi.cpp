#include "vulkan_rhi.h"
#include <algorithm>

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

IBuffer* VulkanRHI::createBuffer(const BufferDesc& /*desc*/) {
    // TODO: Implement actual Vulkan buffer creation
    return nullptr;
}

void VulkanRHI::destroyBuffer(IBuffer* buffer) {
    // TODO: Implement buffer destruction
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

IShader* VulkanRHI::createShader(const ShaderDesc& /*desc*/) {
    // TODO: Implement actual shader module creation
    return nullptr;
}

void VulkanRHI::destroyShader(IShader* shader) {
    delete shader;
}

// ──── Pipeline ────

IPipeline* VulkanRHI::createGraphicsPipeline(const GraphicsPipelineDesc& /*desc*/) {
    // TODO: Implement actual graphics pipeline creation
    return nullptr;
}

IPipeline* VulkanRHI::createComputePipeline(const ComputePipelineDesc& /*desc*/) {
    // TODO: Implement actual compute pipeline creation
    return nullptr;
}

void VulkanRHI::destroyPipeline(IPipeline* pipeline) {
    delete pipeline;
}

// ──── PipelineLayout ────

IPipelineLayout* VulkanRHI::createPipelineLayout(const PipelineLayoutDesc& /*desc*/) {
    // TODO: Implement actual pipeline layout creation
    return nullptr;
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

ISwapChain* VulkanRHI::createSwapChain(const SwapChainDesc& /*desc*/) {
    // TODO: Implement
    return nullptr;
}

void VulkanRHI::destroySwapChain(ISwapChain* swapChain) {
    delete swapChain;
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
