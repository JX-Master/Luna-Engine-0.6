cmake_minimum_required (VERSION 3.3)

set(SRC_FILES
    3DEngine.hpp
    ICamera.hpp
    ILight.hpp
    IMaterial.hpp
    IMesh.hpp
    IModel.hpp
    IModelRenderer.hpp
    ISceneRenderer.hpp
    ITransform.hpp

    Source/3DEngine.cpp
    Source/3DEngineHeader.hpp
    Source/Camera.hpp
    Source/Camera.cpp
    Source/Lights.hpp
    Source/Lights.cpp
    Source/Material.hpp
    Source/Material.cpp
    Source/Mesh.hpp
    Source/Mesh.cpp
    Source/Model.hpp
    Source/Model.cpp
    Source/ModelRenderer.hpp
    Source/ModelRenderer.cpp
    Source/SceneRenderer.hpp
    Source/SceneRenderer.cpp
    Source/Transform.hpp
    Source/Transform.cpp
    )

if(LIB)
    add_library(3DEngine STATIC ${SRC_FILES})
else()
    add_library(3DEngine SHARED ${SRC_FILES})
endif()

target_link_libraries(3DEngine Runtime Core Asset Gfx Renderer Scene)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})