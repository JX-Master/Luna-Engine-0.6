// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicSystemWindows.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include "../GfxHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include "CommonInclude.hpp"
#include <Base/Vector.hpp>
#include "WindowInputDevice.hpp"
#include <Input/Input.hpp>
namespace luna
{
	namespace gfx
	{
		namespace win
		{
			class GraphicAdapter;
			class ShaderBlob;
			class WindowSystem;

			extern ComPtr<IDXGIFactory1> m_dxgi;
			extern Unconstructed<Vector<P<IGraphicAdapter>>> m_adapters;
			extern P<ILogger> m_logger;
			extern P<WindowInputDevice> m_window_input;
			extern HINSTANCE m_instance;

			RV init_graphic_system_windows();
			void deinit_graphic_system_windows();
		}
	}
}

#endif