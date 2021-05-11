cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    tiny_obj_loader.h
    tiny_obj_loader.cc)

add_library(TinyObjLoader STATIC ${SRC_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})