cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    Font.hpp
    IFontAtlas.hpp
    IFontFile.hpp
    Source/DefaultFont.hpp
    Source/DefaultFont.cpp
    Source/FontAtlas.hpp
    Source/FontAtlas.cpp
    Source/FontFileTTF.hpp
    Source/FontFIleTTF.cpp
    Source/FontHeader.hpp
    Source/FontSystem.hpp
    Source/FontSystem.cpp
    Source/StbTrueType.hpp)

if(LIB)
    add_library(Font STATIC ${SRC_FILES})
else()
    add_library(Font SHARED ${SRC_FILES})
endif()

target_link_libraries(Font Core Gfx Image RectPack)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})