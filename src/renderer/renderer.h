#pragma once

#include "rhi/rhi_enums.h"
#include <cstdint>
#include <vector>

class IRHI;
class IRenderPass;
class IFrameBuffer;
class IPipeline;
class IPipelineLayout;
class IShader;
class IBuffer;
class IImageView;
class ICommandList;
class IFence;
class ISwapChain;
struct SwapChainDesc;

class Scene;

class Renderer {
public:
    Renderer(IRHI* rhi);
    ~Renderer();

    bool init(void* windowHandle, uint32_t width, uint32_t height);
    void shutdown();
    void render(Scene* scene);

    IRHI* getRHI() const { return m_rhi; }

private:
    bool compileShader(const char* source, ShaderStage stage, std::vector<uint32_t>& spirv);
    void createFramebuffers();

    IRHI* m_rhi = nullptr;

    ISwapChain*             m_swapChain = nullptr;
    IRenderPass*            m_renderPass = nullptr;
    IPipeline*              m_pipeline = nullptr;
    IPipelineLayout*        m_pipelineLayout = nullptr;
    IShader*                m_vertShader = nullptr;
    IShader*                m_fragShader = nullptr;
    ICommandList*           m_commandList = nullptr;
    IFence*                 m_fence = nullptr;

    std::vector<IFrameBuffer*> m_framebuffers;
    std::vector<IImageView*>   m_framebufferViews;
    uint32_t                m_width = 0;
    uint32_t                m_height = 0;
};
