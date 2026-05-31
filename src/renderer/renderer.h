#pragma once

#include <cstdint>

class IRHI;
class Scene;

class Renderer {
public:
    Renderer(IRHI* rhi);
    ~Renderer();

    bool init();
    void shutdown();
    void render(Scene* scene);

    IRHI* getRHI() const { return m_rhi; }

private:
    IRHI* m_rhi = nullptr;
};
