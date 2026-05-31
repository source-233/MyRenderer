#pragma once

#include "rhi_enums.h"
#include <cstdint>
#include <vector>

class IImageView;

struct AttachmentDesc {
    Format          format = Format::R8G8B8A8_UNORM;
    bool            loadOpClear = true;
    bool            storeOpTrue = true;
    ImageLayout     initialLayout = ImageLayout::UNDEFINED;
    ImageLayout     finalLayout = ImageLayout::COLOR_ATTACHMENT;
};

struct SubpassDesc {
    std::vector<uint32_t> colorAttachments;
    uint32_t              depthStencilAttachment = UINT32_MAX;
};

struct RenderPassDesc {
    std::vector<AttachmentDesc> attachments;
    std::vector<SubpassDesc>    subpasses;
};

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
};

struct FrameBufferDesc {
    IRenderPass*                renderPass = nullptr;
    std::vector<IImageView*>   attachments;
    uint32_t                    width = 0;
    uint32_t                    height = 0;
};

class IFrameBuffer {
public:
    virtual ~IFrameBuffer() = default;
};
