#pragma once

#include "rhi_enums.h"
#include <vector>
#include <cstdint>

class IShader;
class IPipelineLayout;

struct VertexAttributeDesc {
    uint32_t        location = 0;
    Format          format = Format::R32G32B32_SFLOAT;
    uint32_t        offset = 0;
};

struct VertexStreamDesc {
    uint32_t        binding = 0;
    uint32_t        stride = 0;
    bool            inputRateInstance = false;
};

struct RasterizationDesc {
    PolygonMode     polygonMode = PolygonMode::FILL;
    CullMode        cullMode = CullMode::BACK;
    FrontFace       frontFace = FrontFace::COUNTER_CLOCKWISE;
    float           lineWidth = 1.0f;
};

struct DepthStencilDesc {
    bool            depthTest = true;
    bool            depthWrite = true;
    CompareOp       depthCompareOp = CompareOp::LESS;
    bool            stencilTest = false;
};

struct BlendAttachmentDesc {
    bool            blendEnable = false;
    BlendFactor     srcColorBlendFactor = BlendFactor::ONE;
    BlendFactor     dstColorBlendFactor = BlendFactor::ZERO;
    BlendOp         colorBlendOp = BlendOp::ADD;
    BlendFactor     srcAlphaBlendFactor = BlendFactor::ONE;
    BlendFactor     dstAlphaBlendFactor = BlendFactor::ZERO;
    BlendOp         alphaBlendOp = BlendOp::ADD;
};

struct GraphicsPipelineDesc {
    IShader*                        vertexShader = nullptr;
    IShader*                        fragmentShader = nullptr;
    IPipelineLayout*                layout = nullptr;

    std::vector<VertexStreamDesc>   vertexStreams;
    std::vector<VertexAttributeDesc> vertexAttributes;

    RasterizationDesc               rasterization;
    DepthStencilDesc                depthStencil;
    std::vector<BlendAttachmentDesc> blendAttachments;

    Format                          renderTargetFormats[8] = {};
    uint32_t                        renderTargetCount = 1;
    Format                          depthStencilFormat = Format::UNKNOWN;
};

struct ComputePipelineDesc {
    IShader*            computeShader = nullptr;
    IPipelineLayout*    layout = nullptr;
};

class IPipeline {
public:
    virtual ~IPipeline() = default;
};
