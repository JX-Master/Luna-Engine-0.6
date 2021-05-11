cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    EasyDraw.hpp
    IDrawList.hpp
    IDrawListRenderer.hpp
    IDrawPath.hpp
    IFontTexture.hpp

    Source/DrawList.hpp
    Source/DrawList.cpp
    Source/DrawListRenderer.hpp
    Source/DrawListRenderer.cpp
    Source/DrawPath.hpp
    Source/DrawPath.cpp
    Source/EasyDraw.hpp
    Source/EasyDraw.cpp
    Source/EasyDrawHeader.hpp
    Source/FontTexture.hpp
    Source/FontTexture.cpp
    )

if(LIB)
    add_library(EasyDraw STATIC ${SRC_FILES})
else()
    add_library(EasyDraw SHARED ${SRC_FILES})
endif()

target_link_libraries(EasyDraw Runtime Core Gfx Font)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})