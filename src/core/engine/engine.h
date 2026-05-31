#pragma once

#include <cstdint>

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
    void run();

    IRHI* getRHI() const { return m_rhi; }
    Renderer* getRenderer() const { return m_renderer; }
    Scene* getScene() const { return m_scene; }
    platform::Window* getWindow() const { return m_window; }

private:
    platform::Window* m_window = nullptr;
    IRHI* m_rhi = nullptr;
    Renderer* m_renderer = nullptr;
    Scene* m_scene = nullptr;
    bool m_running = false;
};
