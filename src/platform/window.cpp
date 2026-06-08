#include "window.h"
#include <GLFW/glfw3.h>
#include <cstdio>

namespace platform {

static void glfwErrorCallback(int error, const char* desc) {
    fprintf(stderr, "[GLFW] %d: %s\n", error, desc);
}

Window::~Window() {
    shutdown();
}

bool Window::init(const WindowDesc& desc) {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(
        static_cast<int>(desc.width),
        static_cast<int>(desc.height),
        desc.title,
        desc.fullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr);

    if (!m_window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    m_width = desc.width;
    m_height = desc.height;

    return true;
}

void Window::shutdown() {
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

void Window::pollEvents() {
    glfwPollEvents();
}

bool Window::shouldClose() const {
    return m_window ? glfwWindowShouldClose(m_window) : true;
}

} // namespace platform
