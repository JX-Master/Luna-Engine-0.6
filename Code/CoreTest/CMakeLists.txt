cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    Source/TestCommon.hpp
    Source/main.cpp
    Source/DataTest.cpp
    Source/VfsTest.cpp
            )

add_executable(CoreTest ${SRC_FILES})
target_link_libraries(CoreTest Runtime Core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})