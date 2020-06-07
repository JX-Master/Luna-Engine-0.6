// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RenderSystem.hpp
* @author JXMaster
* @date 2020/3/9
*/
#pragma once
#include "RendererHeader.hpp"

namespace luna
{
	namespace renderer
	{
		extern P<gfx::IGraphicDevice> m_main_device;
		extern P<gfx::ICommandQueue> m_main_graphic_queue;
		extern P<gfx::ICommandQueue> m_main_compute_queue;
		extern P<gfx::ICommandQueue> m_main_copy_queue;

		LUNA_RENDERER_API RV init(const RenderSystemStartupParams* params);
		void deinit();
	}
}