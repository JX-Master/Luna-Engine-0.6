cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
            Source/TestCommon.hpp
            Source/TestCommon.cpp
            Source/main.cpp
            Source/VectorTest.cpp
            Source/HashTest.cpp
            Source/RingDequeTest.cpp
            Source/StringTest.cpp
            Source/NameTest.cpp
            Source/PathTest.cpp
            Source/TimeTest.cpp
            Source/LexicalAnalyzerTest.cpp
            )

add_executable(RuntimeTest ${SRC_FILES})
target_link_libraries(RuntimeTest Runtime)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})