cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    IComponent.hpp
    IComponentType.hpp
    IEntity.hpp
    IScene.hpp
    ISceneComponent.hpp
    ISceneComponentType.hpp
    
    Source/Entity.hpp
    Source/Entity.cpp
    Source/Scene.hpp
    Source/Scene.cpp
    Source/SceneAssetType.hpp
    Source/SceneAssetType.cpp
    Source/SceneHeader.hpp
    Source/SceneManager.hpp
    Source/SceneManager.cpp)

if(LIB)
    add_library(Scene STATIC ${SRC_FILES})
else()
    add_library(Scene SHARED ${SRC_FILES})
endif()

target_link_libraries(Scene Runtime Core Asset)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})