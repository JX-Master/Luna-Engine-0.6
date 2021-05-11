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
#include <Runtime/Vector.hpp>
#include "WindowInputDevice.hpp"
#include <Input/Input.hpp>
namespace Luna
{
	namespace Gfx
	{
		namespace Win
		{
			class GraphicAdapter;
			class ShaderBlob;
			class WindowSystem;

			extern ComPtr<IDXGIFactory1> m_dxgi;
			extern Unconstructed<Vector<P<IGraphicAdapter>>> m_adapters;
			extern P<WindowInputDevice> m_window_input;
			extern HINSTANCE m_instance;

			RV init_graphic_system_windows();
			void deinit_graphic_system_windows();
		}
	}
}

#endif