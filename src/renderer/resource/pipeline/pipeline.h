#pragma once

#include "../../rhi/rhi.h"

class PipelineManager {
public:
    explicit PipelineManager(IRHI* rhi);
    ~PipelineManager();

    IPipeline* createGraphics(const GraphicsPipelineDesc& desc);
    IPipeline* createCompute(const ComputePipelineDesc& desc);
    void destroy(IPipeline* pipeline);

private:
    IRHI* m_rhi = nullptr;
};
