#pragma once

#include <cstdint>

struct GLFWwindow;

namespace platform {

struct WindowDesc {
    const char* title = "MyRenderer";
    uint32_t width = 1280;
    uint32_t height = 720;
    bool resizable = true;
    bool fullscreen = false;
};

class Window {
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool init(const WindowDesc& desc);
    void shutdown();
    void pollEvents();
    bool shouldClose() const;

    GLFWwindow* getNativeHandle() const { return m_window; }
    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

private:
    GLFWwindow* m_window = nullptr;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

} // namespace platform
