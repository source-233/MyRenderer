add_library(rhi STATIC
    rhi.cpp
    rhi_command_list.cpp
)
target_include_directories(rhi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(rhi PUBLIC Vulkan::Headers)

add_subdirectory(vulkan)
