cmake_minimum_required(VERSION 3.10)
project(implot)

set(IMPLOT_SRC
    implot.cpp
    implot_items.cpp
)

add_library(implot STATIC ${IMPLOT_SRC})
target_include_directories(implot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(implot PUBLIC imgui)
target_compile_definitions(implot PRIVATE IMGUI_DEFINE_MATH_OPERATORS)
