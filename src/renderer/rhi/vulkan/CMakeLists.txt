add_library(vulkan_rhi STATIC
    vulkan_rhi.cpp
    vulkan_buffer.cpp
    vulkan_image.cpp
    vulkan_sampler.cpp
    vulkan_shader.cpp
    vulkan_pipeline.cpp
    vulkan_pipeline_layout.cpp
    vulkan_descriptor_set_layout.cpp
    vulkan_descriptor_set.cpp
    vulkan_command_list.cpp
    vulkan_fence.cpp
    vulkan_swap_chain.cpp
    vulkan_render_pass.cpp
    vulkan_frame_buffer.cpp
)
target_include_directories(vulkan_rhi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(vulkan_rhi PUBLIC rhi volk Vulkan::Headers)
target_link_libraries(vulkan_rhi PRIVATE glfw)
