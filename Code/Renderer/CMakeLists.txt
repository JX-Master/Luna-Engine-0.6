cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    Renderer.hpp
    Source/RendererHeader.hpp
    Source/RenderSystem.hpp
    Source/RenderSystem.cpp)

if(LIB)
    add_library(Renderer STATIC ${SRC_FILES})
else()
    add_library(Renderer SHARED ${SRC_FILES})
endif()

target_link_libraries(Renderer Runtime Core Gfx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})