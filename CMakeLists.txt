cmake_minimum_required(VERSION 3.16)
project(MyRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ──── Thirdparty ────

set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)

add_subdirectory(thirdparty/glfw-3.4)
add_subdirectory(thirdparty/glm)

# ──── Vulkan SDK ────

find_package(Vulkan REQUIRED)

# ──── Entry point ────

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    thirdparty/
    thirdparty/volk
    thirdparty/glad/include
    thirdparty/stb_image
    thirdparty/tinygltf
    src
)

# ──── Renderer subdirectories ────

add_subdirectory(src/renderer/rhi/vulkan)
add_subdirectory(src/renderer/rhi)
add_subdirectory(src/renderer)

# ──── volk (Vulkan loader) ────

target_sources(${PROJECT_NAME} PRIVATE
    thirdparty/volk/volk.c
)

# ──── Link libraries ────

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
