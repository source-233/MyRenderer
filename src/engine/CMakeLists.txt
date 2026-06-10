add_library(engine STATIC
    engine.cpp
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(engine PUBLIC
    core_math
    core_io
    core_log
    core_uid
    core_time
    core_profiling
    core_delegate
    platform
    renderer
    vulkan_rhi
    scene
    editor
    imgui
    imguizmo
    implot
    imgui_misc
)
