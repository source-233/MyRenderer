#pragma once

#include <cstdint>
#include <vector>
#include <string>

class IRHI;
class IImage;

struct RenderGraphPass {
    std::string name;
    uint32_t readCount = 0;
    uint32_t writeCount = 0;
};

class RenderGraph {
public:
    RenderGraph() = default;
    ~RenderGraph() = default;

    void compile();
    void execute(IRHI* rhi);

private:
    std::vector<RenderGraphPass> m_passes;
};
