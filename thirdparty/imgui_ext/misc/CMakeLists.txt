cmake_minimum_required(VERSION 3.10)
project(imgui_misc)

set(IMGUI_MISC_SRC
    TextEditor.cpp
)

add_library(imgui_misc STATIC ${IMGUI_MISC_SRC})
target_include_directories(imgui_misc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui_misc PUBLIC imgui)
