cmake_minimum_required(VERSION 3.22)
project(MyRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ──── Thirdparty ────

set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)

add_subdirectory(thirdparty/glfw-3.4)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/Vulkan-Headers)

# ──── Dear ImGui ────

set(IMGUI_SRC
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/backends/imgui_impl_glfw.cpp
    thirdparty/imgui/backends/imgui_impl_vulkan.cpp
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    thirdparty/imgui
    thirdparty/imgui/backends
)

target_link_libraries(imgui PUBLIC glfw Vulkan::Headers)

add_subdirectory(thirdparty/imgui_ext/imguizmo)
add_subdirectory(thirdparty/imgui_ext/implot)
add_subdirectory(thirdparty/imgui_ext/misc)

# ──── glslang (GLSL/HLSL to SPIR-V) ────

set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(ENABLE_PCH OFF CACHE BOOL "" FORCE)
set(ALLOW_EXTERNAL_SPIRV_TOOLS OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glslang)

# ──── Entry point ────

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    thirdparty/
    thirdparty/volk
    thirdparty/VulkanMemoryAllocator
    thirdparty/SPIRV-Reflect
    thirdparty/glad/include
    thirdparty/stb_image
    thirdparty/tinygltf
    thirdparty/imgui_ext/imguizmo
    thirdparty/imgui_ext/implot
    thirdparty/imgui_ext/misc
    thirdparty/imgui_ext/fonts
    src
)

# ──── Renderer subdirectories ────

add_subdirectory(src/renderer/rhi/vulkan)
add_subdirectory(src/renderer/rhi)
add_subdirectory(src/renderer)

# ──── volk / VMA / SPIRV-Reflect ────

target_sources(${PROJECT_NAME} PRIVATE
    thirdparty/volk/volk.c
    thirdparty/VulkanMemoryAllocator/vk_mem_alloc.cpp
    thirdparty/SPIRV-Reflect/spirv_reflect.c
)

# ──── Link libraries ────

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Headers
    glslang::glslang
    glslang::SPIRV
    glslang::glslang-default-resource-limits
    glfw
    imgui
    imguizmo
    implot
    imgui_misc
)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
