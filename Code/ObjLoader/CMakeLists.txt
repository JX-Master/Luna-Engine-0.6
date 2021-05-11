cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    IObjFile.hpp
    ObjLoader.hpp

    Source/ObjFile.hpp
    Source/ObjLoader.cpp
    Source/ObjLoaderHeader.hpp
    Source/tiny_obj_loader.h
    Source/tiny_obj_loader.cpp
    )

if(LIB)
    add_library(ObjLoader STATIC ${SRC_FILES})
else()
    add_library(ObjLoader SHARED ${SRC_FILES})
endif()

target_link_libraries(ObjLoader Runtime Core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})