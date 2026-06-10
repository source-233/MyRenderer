add_subdirectory(rhi)

add_library(renderer STATIC
    renderer.cpp
    resource/shader/shader.cpp
    resource/pipeline/pipeline.cpp
)
target_include_directories(renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(renderer PUBLIC rhi)
target_link_libraries(renderer PRIVATE
    glslang::glslang
    glslang::SPIRV
    glslang::glslang-default-resource-limits
)
