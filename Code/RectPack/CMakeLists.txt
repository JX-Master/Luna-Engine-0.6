cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    IRectPackContext.hpp
    RectPack.hpp
    Source/RectPack.cpp
    Source/RectPackContext.hpp
    Source/RectPackContext.cpp
    Source/RectPackHeader.hpp)

if(LIB)
    add_library(RectPack STATIC ${SRC_FILES})
else()
    add_library(RectPack SHARED ${SRC_FILES})
endif()

target_link_libraries(RectPack Runtime Core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})