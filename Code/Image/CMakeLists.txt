cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    IImage.hpp
    Image.hpp
    Source/Image.hpp
    Source/Image.cpp
    Source/ImageHeader.hpp
    Source/ImageSystem.hpp
    Source/ImageSystem.cpp
    Source/IO/STBImage.hpp
    Source/IO/STBImage.cpp
    Source/IO/STBImageWrite.hpp
    Source/IO/STBImageWrite.cpp)

if(LIB)
    add_library(Image STATIC ${SRC_FILES})
else()
    add_library(Image SHARED ${SRC_FILES})
endif()

target_link_libraries(Image Runtime Core Gfx)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})