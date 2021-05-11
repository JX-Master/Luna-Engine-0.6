cmake_minimum_required (VERSION 3.20)

set(SRC_FILES
    Source/AssetBrowser.hpp
    Source/AssetBrowser.cpp
    Source/IAssetEditor.hpp
    Source/IAssetEditorType.hpp
    Source/IAssetImporterType.hpp
    Source/IComponentEditor.hpp
    Source/IComponentEditorType.hpp
    Source/ISceneComponentEditor.hpp
    Source/ISceneComponentEditorType.hpp
    Source/main.cpp
    Source/MainEditor.hpp
    Source/MainEditor.cpp
    Source/ProjectSelector.hpp
    Source/ProjectSelector.cpp
    Source/StudioHeader.hpp

    Source/Assets/MaterialEditor.hpp
    Source/Assets/MaterialEditor.cpp
    Source/Assets/ModelEditor.hpp
    Source/Assets/ModelEditor.cpp
    Source/Assets/ObjImporter.hpp
    Source/Assets/ObjImporter.cpp
    Source/Assets/SceneEditor.hpp
    Source/Assets/SceneEditor.cpp
    Source/Assets/TextureEditor.hpp
    Source/Assets/TextureEditor.cpp
    Source/Assets/TextureEditorType.hpp
    Source/Assets/TextureEditorType.cpp
    Source/Assets/TextureImporter.hpp
    Source/Assets/TextureImporter.cpp
    
    Source/ComponentEditors/CameraComponentEditor.hpp
    Source/ComponentEditors/CameraComponentEditor.cpp
    Source/ComponentEditors/LightComponentEditor.hpp
    Source/ComponentEditors/LightComponentEditor.cpp
    Source/ComponentEditors/ModelRendererComponentEditor.hpp
    Source/ComponentEditors/ModelRendererComponentEditor.cpp
    Source/ComponentEditors/SceneRendererComponentEditor.hpp
    Source/ComponentEditors/SceneRendererComponentEditor.cpp
    Source/ComponentEditors/TransformComponentEditor.hpp
    Source/ComponentEditors/TransformComponentEditor.cpp

    Source/Shaders/LightingPassPixel.hlsl
    Source/Shaders/LumCS.hlsl
    Source/Shaders/LumFirstCS.hlsl
    Source/Shaders/MipmapGenerationCS.hlsl
    Source/Shaders/SkyboxCS.hlsl
    Source/Shaders/ToneMappingCS.hlsl
    )

add_executable(Studio ${SRC_FILES})

target_link_libraries(Studio Runtime Core Input Gfx Renderer RectPack Image Font ImGui Asset Texture 3DEngine ObjLoader Scene)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

set_property(SOURCE Source/Shaders/LightingPassPixel.hlsl PROPERTY VS_SHADER_TYPE "Pixel")
set_property(SOURCE Source/Shaders/LumCS.hlsl PROPERTY VS_SHADER_TYPE "Compute")
set_property(SOURCE Source/Shaders/LumFirstCS.hlsl PROPERTY VS_SHADER_TYPE "Compute")
set_property(SOURCE Source/Shaders/MipmapGenerationCS.hlsl PROPERTY VS_SHADER_TYPE "Compute")
set_property(SOURCE Source/Shaders/SkyboxCS.hlsl PROPERTY VS_SHADER_TYPE "Compute")
set_property(SOURCE Source/Shaders/ToneMappingCS.hlsl PROPERTY VS_SHADER_TYPE "Compute")

set_property(SOURCE Source/Shaders/LightingPassPixel.hlsl PROPERTY VS_SHADER_MODEL "5.1")
set_property(SOURCE Source/Shaders/LumCS.hlsl PROPERTY VS_SHADER_MODEL "5.1")
set_property(SOURCE Source/Shaders/LumFirstCS.hlsl PROPERTY VS_SHADER_MODEL "5.1")
set_property(SOURCE Source/Shaders/MipmapGenerationCS.hlsl PROPERTY VS_SHADER_MODEL "5.1")
set_property(SOURCE Source/Shaders/SkyboxCS.hlsl PROPERTY VS_SHADER_MODEL "5.1")
set_property(SOURCE Source/Shaders/ToneMappingCS.hlsl PROPERTY VS_SHADER_MODEL "5.1")