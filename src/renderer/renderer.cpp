#include "renderer.h"
#include "rhi/rhi.h"
#include "rhi/rhi_shader.h"
#include "rhi/rhi_swap_chain.h"
#include "rhi/rhi_render_pass.h"
#include "rhi/rhi_image.h"
#include "rhi/rhi_pipeline.h"
#include "rhi/rhi_pipeline_layout.h"
#include "rhi/rhi_command_list.h"
#include "rhi/rhi_fence.h"
#include <glslang/Include/glslang_c_interface.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Embedded shader sources (simplifies file path issues)
static const char* s_vertSrc = R"(
#version 450
vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
)";

static const char* s_fragSrc = R"(
#version 450
layout(location = 0) out vec4 outColor;
void main() {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)";

// ──── glslang helpers ────

static glslang_resource_t makeDefaultResources() {
    glslang_resource_t r{};
    r.max_lights = 32;
    r.max_clip_planes = 6;
    r.max_texture_units = 32;
    r.max_texture_coords = 32;
    r.max_vertex_attribs = 64;
    r.max_vertex_uniform_components = 4096;
    r.max_varying_floats = 64;
    r.max_vertex_texture_image_units = 32;
    r.max_combined_texture_image_units = 80;
    r.max_texture_image_units = 32;
    r.max_fragment_uniform_components = 4096;
    r.max_draw_buffers = 32;
    r.max_vertex_uniform_vectors = 128;
    r.max_varying_vectors = 8;
    r.max_fragment_uniform_vectors = 16;
    r.max_vertex_output_vectors = 16;
    r.max_fragment_input_vectors = 15;
    r.min_program_texel_offset = -8;
    r.max_program_texel_offset = 7;
    r.max_clip_distances = 8;
    r.max_compute_work_group_count_x = 65535;
    r.max_compute_work_group_count_y = 65535;
    r.max_compute_work_group_count_z = 65535;
    r.max_compute_work_group_size_x = 1024;
    r.max_compute_work_group_size_y = 1024;
    r.max_compute_work_group_size_z = 64;
    r.max_compute_uniform_components = 1024;
    r.max_compute_texture_image_units = 16;
    r.max_compute_image_uniforms = 16;
    r.max_compute_atomic_counters = 8;
    r.max_compute_atomic_counter_buffers = 1;
    r.max_compute_atomic_counters = 8;
    r.max_compute_atomic_counter_buffers = 1;
    r.max_compute_atomic_counter_buffers = 1;
    r.max_compute_atomic_counter_buffers = 1;
    r.max_compute_image_uniforms = 16;
    r.max_compute_texture_image_units = 16;
    r.max_compute_uniform_components = 1024;
    r.max_fragment_input_vectors = 15;
    r.max_fragment_uniform_vectors = 16;
    r.max_fragment_uniform_components = 4096;
    r.max_combined_texture_image_units = 80;
    r.max_texture_image_units = 32;
    r.max_vertex_output_vectors = 16;
    r.max_vertex_uniform_vectors = 128;
    r.max_vertex_uniform_components = 4096;
    r.max_varying_vectors = 8;
    r.max_varying_floats = 64;
    r.max_vertex_texture_image_units = 32;
    r.max_vertex_attribs = 64;
    r.max_texture_coords = 32;
    r.max_texture_units = 32;
    r.max_clip_planes = 6;
    r.max_lights = 32;
    return r;
}

static bool compileGlslToSpirv(const char* source, ShaderStage stage, std::vector<uint32_t>& spirv) {
    glslang_stage_t gStage;
    switch (stage) {
        case ShaderStage::VERTEX:   gStage = GLSLANG_STAGE_VERTEX; break;
        case ShaderStage::FRAGMENT: gStage = GLSLANG_STAGE_FRAGMENT; break;
        default: return false;
    }

    glslang_input_t input{};
    input.language = GLSLANG_SOURCE_GLSL;
    input.stage = gStage;
    input.client = GLSLANG_CLIENT_VULKAN;
    input.client_version = GLSLANG_TARGET_VULKAN_1_3;
    input.target_language = GLSLANG_TARGET_SPV;
    input.target_language_version = GLSLANG_TARGET_SPV_1_6;
    input.code = source;
    input.default_version = 100;
    input.default_profile = GLSLANG_NO_PROFILE;
    input.force_default_version_and_profile = 0;
    input.forward_compatible = 0;
    input.messages = (glslang_messages_t)(GLSLANG_MSG_DEFAULT_BIT | GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT);
    input.resource = nullptr;

    glslang_shader_t* shader = glslang_shader_create(&input);
    if (!shader) return false;

    glslang_shader_set_entry_point(shader, "main");

    if (!glslang_shader_preprocess(shader, &input)) {
        fprintf(stderr, "glslang preprocess error: %s\n", glslang_shader_get_info_log(shader));
        glslang_shader_delete(shader);
        return false;
    }

    if (!glslang_shader_parse(shader, &input)) {
        fprintf(stderr, "glslang parse error: %s\n", glslang_shader_get_info_log(shader));
        glslang_shader_delete(shader);
        return false;
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_DEFAULT_BIT)) {
        fprintf(stderr, "glslang link error: %s\n", glslang_program_get_info_log(program));
        glslang_program_delete(program);
        glslang_shader_delete(shader);
        return false;
    }

    glslang_program_SPIRV_generate(program, gStage);

    size_t count = glslang_program_SPIRV_get_size(program);
    spirv.resize(count);
    glslang_program_SPIRV_get(program, spirv.data());

    glslang_program_delete(program);
    glslang_shader_delete(shader);
    return true;
}

// ──── Renderer ────

Renderer::Renderer(IRHI* rhi) : m_rhi(rhi) {}
Renderer::~Renderer() { shutdown(); }

bool Renderer::init(void* windowHandle, uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;

    // Initialize glslang
    glslang_initialize_process();

    // Compile shaders
    std::vector<uint32_t> vertSPIRV, fragSPIRV;
    if (!compileGlslToSpirv(s_vertSrc, ShaderStage::VERTEX, vertSPIRV)) {
        fprintf(stderr, "Failed to compile vertex shader\n");
        return false;
    }
    if (!compileGlslToSpirv(s_fragSrc, ShaderStage::FRAGMENT, fragSPIRV)) {
        fprintf(stderr, "Failed to compile fragment shader\n");
        return false;
    }

    ShaderDesc vertDesc, fragDesc;
    vertDesc.stage = ShaderStage::VERTEX;
    vertDesc.code = vertSPIRV.data();
    vertDesc.codeSize = vertSPIRV.size() * sizeof(uint32_t);
    fragDesc.stage = ShaderStage::FRAGMENT;
    fragDesc.code = fragSPIRV.data();
    fragDesc.codeSize = fragSPIRV.size() * sizeof(uint32_t);

    m_vertShader = m_rhi->createShader(vertDesc);
    m_fragShader = m_rhi->createShader(fragDesc);
    if (!m_vertShader || !m_fragShader) return false;

    // Create swap chain
    SwapChainDesc scDesc;
    scDesc.windowHandle = windowHandle;
    scDesc.width = width;
    scDesc.height = height;
    scDesc.bufferCount = 2;
    scDesc.vsync = true;
    m_swapChain = m_rhi->createSwapChain(scDesc);
    if (!m_swapChain) {
        fprintf(stderr, "Failed to create swap chain\n");
        return false;
    }

    Format swapFormat = m_swapChain->getFormat();

    // Create render pass
    RenderPassDesc rpDesc;
    AttachmentDesc attDesc;
    attDesc.format = swapFormat;
    attDesc.loadOpClear = true;
    attDesc.storeOpTrue = true;
    attDesc.initialLayout = ImageLayout::UNDEFINED;
    attDesc.finalLayout = ImageLayout::PRESENT;
    rpDesc.attachments.push_back(attDesc);

    SubpassDesc subpass;
    subpass.colorAttachments.push_back(0);
    rpDesc.subpasses.push_back(subpass);

    m_renderPass = m_rhi->createRenderPass(rpDesc);
    if (!m_renderPass) {
        fprintf(stderr, "Failed to create render pass\n");
        return false;
    }

    // Create pipeline layout (empty — no descriptor sets, no push constants)
    PipelineLayoutDesc plDesc;
    m_pipelineLayout = m_rhi->createPipelineLayout(plDesc);
    if (!m_pipelineLayout) return false;

    // Create graphics pipeline
    GraphicsPipelineDesc gpDesc;
    gpDesc.vertexShader = m_vertShader;
    gpDesc.fragmentShader = m_fragShader;
    gpDesc.layout = m_pipelineLayout;
    gpDesc.renderTargetFormats[0] = swapFormat;
    gpDesc.renderTargetCount = 1;
    gpDesc.rasterization.cullMode = CullMode::NONE;
    gpDesc.blendAttachments.push_back({});

    m_pipeline = m_rhi->createGraphicsPipeline(gpDesc);
    if (!m_pipeline) {
        fprintf(stderr, "Failed to create graphics pipeline\n");
        return false;
    }

    // Create framebuffers (one per swap chain image)
    createFramebuffers();

    // Create command list
    m_commandList = m_rhi->createCommandList();
    if (!m_commandList) return false;

    // Create fence (start signaled so first frame doesn't wait)
    m_fence = m_rhi->createFence(true);
    if (!m_fence) return false;

    return true;
}

void Renderer::createFramebuffers() {
    uint32_t count = m_swapChain->getImageCount();
    for (uint32_t i = 0; i < count; ++i) {
        IImage* img = m_swapChain->getImage(i);
        if (!img) break;

        ImageViewDesc viewDesc;
        viewDesc.format = m_swapChain->getFormat();
        viewDesc.aspect = ImageAspect::COLOR;
        IImageView* view = img->createView(viewDesc);
        if (!view) break;
        m_framebufferViews.push_back(view);

        FrameBufferDesc fbDesc;
        fbDesc.renderPass = m_renderPass;
        fbDesc.attachments.push_back(view);
        fbDesc.width = m_width;
        fbDesc.height = m_height;

        IFrameBuffer* fb = m_rhi->createFrameBuffer(fbDesc);
        if (!fb) break;
        m_framebuffers.push_back(fb);
    }
}

void Renderer::shutdown() {
    if (m_rhi) m_rhi->waitIdle();

    for (auto* fb : m_framebuffers) {
        m_rhi->destroyFrameBuffer(fb);
    }
    m_framebuffers.clear();
    for (auto* view : m_framebufferViews) {
        m_rhi->destroyImageView(view);
    }
    m_framebufferViews.clear();

    delete m_fence;           m_fence = nullptr;
    delete m_commandList;     m_commandList = nullptr;
    delete m_pipeline;        m_pipeline = nullptr;
    delete m_pipelineLayout;  m_pipelineLayout = nullptr;
    delete m_renderPass;      m_renderPass = nullptr;
    delete m_swapChain;       m_swapChain = nullptr;
    delete m_fragShader;      m_fragShader = nullptr;
    delete m_vertShader;      m_vertShader = nullptr;

    glslang_finalize_process();
}

void Renderer::render(Scene*) {
    // Acquire next swap chain image
    m_swapChain->present();

    uint32_t fbIndex = m_swapChain->getCurrentImageIndex();
    // VulkanSwapChain returns index relative to its internal array
    // Our framebuffers are ordered the same way as swap chain images
    // Map using a simple modulo
    IFrameBuffer* fb = m_framebuffers[fbIndex % m_framebuffers.size()];
    IImage* img = m_swapChain->getCurrentImage();

    // Wait for previous frame
    m_fence->wait();
    m_fence->reset();

    // Record command buffer
    m_commandList->reset();
    m_commandList->begin();

    Viewport vp{};
    vp.width = (float)m_width;
    vp.height = (float)m_height;
    m_commandList->setViewport(vp);

    Rect2D scissor{};
    scissor.width = m_width;
    scissor.height = m_height;
    m_commandList->setScissor(scissor);

    ClearColor clear{ 0.0f, 0.0f, 0.0f, 1.0f };
    ClearDepthStencil ds{ 1.0f, 0 };

    m_commandList->beginRenderPass(m_renderPass, fb, &clear, 1, ds);
    m_commandList->setPipeline(m_pipeline);
    m_commandList->draw(3, 1, 0, 0);
    m_commandList->endRenderPass();

    m_commandList->end();

    // Submit
    m_rhi->submit(m_commandList, m_fence);

    // Present
    m_rhi->present(m_swapChain);
}
