// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RenderSystem.hpp
* @author JXMaster
* @date 2020/3/9
*/
#pragma once
#include "RendererHeader.hpp"

namespace Luna
{
	namespace Renderer
	{
		extern P<Gfx::IGraphicDevice> m_main_device;
		extern P<Gfx::ICommandQueue> m_main_graphic_queue;
		extern P<Gfx::ICommandQueue> m_main_compute_queue;
		extern P<Gfx::ICommandQueue> m_main_copy_queue;

		RV init();
		void deinit();
	}
}