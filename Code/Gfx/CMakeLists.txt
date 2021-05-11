cmake_minimum_required (VERSION 3.3)

if(WIN32)

option(GFX_D3D12 "Use D3D12 on Windows." ON)

if(GFX_D3D12)
    add_definitions("-DLUNA_GFX_D3D12")
else()
    add_definitions("-DLUNA_GFX_D3D11")
endif()

endif() #WIN32

set(SRC_FILES
        Gfx.hpp
        GraphicDefines.hpp
        ICommandBuffer.hpp
        ICommandQueue.hpp
        IFrameBuffer.hpp
        IGraphicAdapter.hpp
        IGraphicDevice.hpp
        IGraphicDeviceChild.hpp
        IPipelineState.hpp
        IRenderPass.hpp
        IResource.hpp
        IResourceHeap.hpp
        IShaderInputLayout.hpp
        ISwapChain.hpp
        IViewSet.hpp
        IWindow.hpp
        IWindowEventListener.hpp
        IWindowInputDevice.hpp
        Views.hpp
        WindowEvent.hpp

        Source/GfxHeader.hpp
        )

if(WIN32)
    set(SRC_FILES ${SRC_FILES}
            Source/Windows/CommonInclude.hpp
            Source/Windows/GraphicAdapter.hpp
            Source/Windows/GraphicAdapter.cpp
            Source/Windows/GraphicSystemWindows.hpp
            Source/Windows/GraphicSystemWindows.cpp
            Source/Windows/Window.hpp
            Source/Windows/Window.cpp
            Source/Windows/WindowInputDevice.hpp
            Source/Windows/WindowInputDevice.cpp
            )

    if(GFX_D3D12)
        set(SRC_FILES ${SRC_FILES}
            Source/D3D12/CommandBuffer.hpp
            Source/D3D12/CommandBuffer.cpp
            Source/D3D12/CommandQueue.hpp
            Source/D3D12/CommandQueue.cpp
            Source/D3D12/D3D12Common.hpp
            Source/D3D12/FrameBuffer.hpp
            Source/D3D12/FrameBuffer.cpp
            Source/D3D12/GraphicDevice.hpp
            Source/D3D12/GraphicDevice.cpp
            Source/D3D12/GraphicSystem.hpp
            Source/D3D12/GraphicSystem.cpp
            Source/D3D12/PipelineState.hpp
            Source/D3D12/PipelineState.cpp
            Source/D3D12/RenderPass.hpp
            Source/D3D12/Resource.hpp
            Source/D3D12/Resource.cpp
            Source/D3D12/ResourceHeap.hpp
            Source/D3D12/ResourceHeap.cpp
            Source/D3D12/ShaderInputLayout.hpp
            Source/D3D12/ShaderInputLayout.cpp
            Source/D3D12/SwapChain.hpp
            Source/D3D12/SwapChain.cpp
            Source/D3D12/ViewSet.hpp
            Source/D3D12/ViewSet.cpp
            )
    endif()
endif()

if(LIB)
    add_library(Gfx STATIC ${SRC_FILES})
else()
    add_library(Gfx SHARED ${SRC_FILES})
endif()

target_link_libraries(Gfx Runtime Core Input)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})