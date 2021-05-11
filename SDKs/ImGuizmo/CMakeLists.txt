cmake_minimum_required (VERSION 3.3)

include_directories("../ImGuiLib")

set(SRC_FILES
    ImGuizmo.h
    ImGuizmo.cpp)

add_library(ImGuizmo STATIC ${SRC_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})