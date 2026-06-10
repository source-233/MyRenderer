add_library(core_math STATIC
    math/bvh.cpp
)
target_include_directories(core_math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(core_math PRIVATE libacc glm)

add_library(core_io INTERFACE)
target_include_directories(core_io INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core_log INTERFACE)
target_include_directories(core_log INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core_uid INTERFACE)
target_include_directories(core_uid INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core_time INTERFACE)
target_include_directories(core_time INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core_profiling INTERFACE)
target_include_directories(core_profiling INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core_delegate INTERFACE)
target_include_directories(core_delegate INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(core INTERFACE)
target_link_libraries(core INTERFACE core_math core_io core_log core_uid core_time core_profiling core_delegate)
