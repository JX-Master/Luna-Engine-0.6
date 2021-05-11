cmake_minimum_required (VERSION 3.3)

set(SRC_FILES
    ITexture.hpp
    Texture.hpp

    Source/Texture.hpp
    Source/Texture.cpp
    Source/TextureHeader.hpp
    )

if(LIB)
    add_library(Texture STATIC ${SRC_FILES})
else()
    add_library(Texture SHARED ${SRC_FILES})
endif()

target_link_libraries(Texture Runtime Core Asset Gfx Renderer Image)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})