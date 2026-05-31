#include "renderer.h"
#include "rhi/rhi.h"

Renderer::Renderer(IRHI* rhi) : m_rhi(rhi) {}
Renderer::~Renderer() { shutdown(); }

bool Renderer::init() { return true; }
void Renderer::shutdown() {}
void Renderer::render(Scene*) {}
