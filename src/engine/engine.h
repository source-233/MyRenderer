#pragma once

#include <cstdint>
#include "engine/api/engine_api.h"

class IRHI;
class Renderer;
class Scene;
namespace platform { class Window; }

class Engine {
public:
    Engine() = default;
    ~Engine();

    bool init(int argc, char* argv[]);
    void shutdown();
    void tick();
    bool isRunning() const { return m_running; }

    EngineAPI*          getAPI() { return &m_api; }
    IRHI*               getRHI() const { return m_rhi; }
    Renderer*           getRenderer() const { return m_renderer; }
    Scene*              getScene() const { return m_scene; }
    platform::Window*   getWindow() const { return m_window; }

private:
    platform::Window* m_window = nullptr;
    IRHI* m_rhi = nullptr;
    Renderer* m_renderer = nullptr;
    Scene* m_scene = nullptr;
    bool m_running = false;
    EngineAPI m_api{};
};
