target_sources(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_rhi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_sampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_pipeline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_pipeline_layout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_descriptor_set_layout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_descriptor_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_command_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_fence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vulkan_swap_chain.cpp
)
