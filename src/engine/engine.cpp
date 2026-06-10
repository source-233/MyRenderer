#include "engine.h"
#include "platform/window.h"
#include "renderer/rhi/vulkan/vulkan_rhi.h"
#include "renderer/renderer.h"
#include "core/log/log.h"

Engine::~Engine() {
    shutdown();
}

bool Engine::init(int /*argc*/, char* /*argv*/[]) {
    m_window = new platform::Window();
    platform::WindowDesc winDesc;
    if (!m_window->init(winDesc)) {
        LOG_ERROR("Failed to create window");
        return false;
    }

    m_rhi = new VulkanRHI();
    RHIDesc rhiDesc;
    rhiDesc.appName = "MyRenderer";
    rhiDesc.enableValidation = true;
    if (!m_rhi->initialize(rhiDesc)) {
        LOG_ERROR("Failed to initialize RHI");
        return false;
    }

    m_renderer = new Renderer(m_rhi);
    if (!m_renderer->init(m_window->getNativeHandle(),
                          m_window->getWidth(),
                          m_window->getHeight())) {
        LOG_ERROR("Failed to initialize renderer");
        return false;
    }

    // Fill EngineAPI
    m_api.version = 1;
    m_api.user_data = this;
    m_api.log = [](void*, const char* level, const char* msg) {
        fprintf(stdout, "[%s] %s\n", level, msg);
    };
    m_api.getDeltaTime = [](void*) -> double { return 1.0 / 60.0; };
    m_api.getRHI = [](void* user) -> IRHI* {
        return static_cast<Engine*>(user)->m_rhi;
    };
    m_api.getRenderer = [](void* user) -> Renderer* {
        return static_cast<Engine*>(user)->m_renderer;
    };
    m_api.getScene = [](void* user) -> Scene* {
        return static_cast<Engine*>(user)->m_scene;
    };
    m_api.findPlugin = [](void*, const char*) -> void* { return nullptr; };

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

void Engine::tick() {
    if (!m_running) return;

    m_window->pollEvents();
    if (m_window->shouldClose()) {
        m_running = false;
        return;
    }

    // Update plugins (placeholder)
    // m_pluginManager->update(/* deltaTime */);

    // Render frame
    m_renderer->render(m_scene);
}
