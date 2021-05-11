cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    Source/EasyDrawDemo.cpp)

add_executable(EasyDrawDemo ${SRC_FILES})

target_link_libraries(EasyDrawDemo Runtime Core Gfx Renderer EasyDraw Font RectPack Image Input)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})