cmake_minimum_required (VERSION 3.3)

set(SRC_FILES
    Source/main.cpp)

add_executable(GfxTest ${SRC_FILES})

target_link_libraries(GfxTest Runtime Core Input Gfx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})