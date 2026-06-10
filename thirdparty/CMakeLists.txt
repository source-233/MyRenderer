# ─── Libraries with their own CMake ───

add_subdirectory(glfw-3.4)
add_subdirectory(glm)
add_subdirectory(Vulkan-Headers)
add_subdirectory(glslang)
add_subdirectory(imgui_ext/imguizmo)
add_subdirectory(imgui_ext/implot)
add_subdirectory(imgui_ext/misc)

# ─── ImGui ───

set(IMGUI_SRC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_vulkan.cpp
)
add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)
target_link_libraries(imgui PUBLIC glfw Vulkan::Headers)

# ─── volk ───

add_library(volk STATIC volk/volk.c)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/volk)
target_link_libraries(volk PUBLIC Vulkan::Headers)

# ─── VMA ───

add_library(Vma STATIC VulkanMemoryAllocator/vk_mem_alloc.cpp)
target_include_directories(Vma PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/VulkanMemoryAllocator)
target_link_libraries(Vma PUBLIC Vulkan::Headers)

# ─── SPIRV-Reflect ───

add_library(SPIRV-Reflect STATIC SPIRV-Reflect/spirv_reflect.c)
target_include_directories(SPIRV-Reflect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/SPIRV-Reflect)

# ─── Header-only (INTERFACE) ───

add_library(glad INTERFACE)
target_include_directories(glad INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)

add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)

add_library(tinygltf INTERFACE)
target_include_directories(tinygltf INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/tinygltf)

add_library(libacc INTERFACE)
target_include_directories(libacc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/libacc)
