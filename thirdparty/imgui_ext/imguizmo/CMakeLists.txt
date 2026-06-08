cmake_minimum_required(VERSION 3.10)
project(imguizmo)

set(IMGUIZMO_SRC
    ImGuizmo.cpp
    ImSequencer.cpp
    ImCurveEdit.cpp
    ImGradient.cpp
)

add_library(imguizmo STATIC ${IMGUIZMO_SRC})
target_include_directories(imguizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imguizmo PUBLIC imgui)
target_compile_definitions(imguizmo PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
