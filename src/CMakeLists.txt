# ─── Core sub-libraries ───
add_subdirectory(core)

# ─── Platform ───
add_library(platform STATIC
    platform/window.cpp
)
target_include_directories(platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(platform PUBLIC glfw)

# ─── Scene (header-only) ───
add_library(scene INTERFACE)
target_include_directories(scene INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(scene INTERFACE core_math)

# ─── Editor (header-only) ───
add_library(editor INTERFACE)
target_include_directories(editor INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editor INTERFACE core_math)

# ─── Renderer ───
add_subdirectory(renderer)

# ─── Engine (composition root) ───
add_subdirectory(engine)

# ─── Executable ───
add_executable(${PROJECT_NAME} main.cpp)

if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE engine)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)
