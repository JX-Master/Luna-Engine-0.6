cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    Source/ImGuiDemo.cpp)

add_executable(ImGuiDemo ${SRC_FILES})

target_link_libraries(ImGuiDemo Runtime Core Gfx Renderer ImGui Input RectPack Image Font)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})