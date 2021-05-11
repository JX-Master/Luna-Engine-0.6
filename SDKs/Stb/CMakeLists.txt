cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    stb_image_resize.h
    stb_image_write.h
    stb_image.h
    stb_rect_pack.h
    stb_truetype.h)

add_library(Stb STATIC ${SRC_FILES})

set_target_properties(Stb PROPERTIES LINKER_LANGUAGE CXX)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})