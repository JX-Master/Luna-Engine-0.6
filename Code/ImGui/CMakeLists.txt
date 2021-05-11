cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    IContext.hpp
    ImGui.hpp
    Source/Context.hpp
    Source/Context.cpp
    Source/ImGuiHeader.hpp
    Source/ImGuiInstance.hpp
    Source/ImGuiInstance.cpp)

if(LIB)
    add_library(ImGui STATIC ${SRC_FILES})
else()
    add_library(ImGui SHARED ${SRC_FILES})
endif()

target_link_libraries(ImGui Runtime Core Gfx Input Font ImGuiLib ImGuizmo)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})