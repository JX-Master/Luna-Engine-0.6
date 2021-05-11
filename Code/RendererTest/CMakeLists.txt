cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    Source/main.cpp
    Source/TestHeader.hpp
    Source/Tests/TestImage.hpp
    Source/Tests/TestImage.cpp
    Source/Tests/TestImageData.cpp
    Source/Tests/TestNone.hpp
    Source/Tests/TestNone.cpp
    Source/Tests/TestTriangle.hpp
    Source/Tests/TestTriangle.cpp)

add_executable(RendererTest ${SRC_FILES})

target_link_libraries(RendererTest Runtime Core Input Gfx Renderer)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})