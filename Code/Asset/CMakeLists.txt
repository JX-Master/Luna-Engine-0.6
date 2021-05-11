cmake_minimum_required (VERSION 3.3)

set(SRC_FILES 
    IAsset.hpp
    IAssetMeta.hpp
    IAssetSaveRequest.hpp
    IAssetType.hpp
    Asset.hpp
    
    Source/AssetHeader.hpp
    Source/AssetMeta.hpp
    Source/AssetMeta.cpp
    Source/AssetRequests.hpp
    Source/AssetRequests.cpp
    Source/AssetSystem.hpp
    Source/AssetSystem.cpp)

if(LIB)
    add_library(Asset STATIC ${SRC_FILES})
else()
    add_library(Asset SHARED ${SRC_FILES})
endif()

target_link_libraries(Asset Runtime Core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})