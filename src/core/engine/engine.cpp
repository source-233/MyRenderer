#include "core/engine/engine.h"
#include "platform/window.h"
#include "renderer/rhi/vulkan/vulkan_rhi.h"
#include "renderer/renderer.h"

Engine::~Engine() {
    shutdown();
}

bool Engine::init(int /*argc*/, char* /*argv*/[]) {
    m_window = new platform::Window();
    platform::WindowDesc winDesc;
    if (!m_window->init(winDesc)) {
        fprintf(stderr, "Failed to create window\n");
        return false;
    }

    m_rhi = new VulkanRHI();
    RHIDesc rhiDesc;
    rhiDesc.appName = "MyRenderer";
    rhiDesc.enableValidation = true;
    if (!m_rhi->initialize(rhiDesc)) {
        fprintf(stderr, "Failed to initialize RHI\n");
        return false;
    }

    m_renderer = new Renderer(m_rhi);
    if (!m_renderer->init()) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return false;
    }

    return true;
}

void Engine::shutdown() {
    m_running = false;

    delete m_renderer;
    m_renderer = nullptr;

    if (m_rhi) {
        m_rhi->shutdown();
        delete m_rhi;
        m_rhi = nullptr;
    }

    delete m_window;
    m_window = nullptr;
}

void Engine::run() {
    m_running = true;

    while (m_running && m_window && !m_window->shouldClose()) {
        m_window->pollEvents();
        m_renderer->render(m_scene);
    }
}
