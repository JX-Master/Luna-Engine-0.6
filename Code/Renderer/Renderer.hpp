// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Renderer.hpp
* @author JXMaster
* @date 2020/3/9
*/
#pragma once
#include <Gfx/Gfx.hpp>

#ifndef LUNA_RENDERER_API
#define LUNA_RENDERER_API
#endif

namespace luna
{
	namespace renderer
	{
		struct RenderSystemStartupParams
		{
			//! The graphic adapter to be used for the main device.
			gfx::IGraphicAdapter* adapter = nullptr;
		};

		//! Initializes the renderer module.
		LUNA_RENDERER_API RV init(const RenderSystemStartupParams* params = nullptr);

		//! Gets the main graphic device.
		LUNA_RENDERER_API gfx::IGraphicDevice* device();

		//! Gets the main graphic command queue for the main graphic device.
		LUNA_RENDERER_API gfx::ICommandQueue* main_graphic_queue();

		//! Gets the main compute command queue for the main graphic device.
		LUNA_RENDERER_API gfx::ICommandQueue* main_compute_queue();

		//! Gets the main copy command queue for the main graphic device.
		LUNA_RENDERER_API gfx::ICommandQueue* main_copy_queue();
	}
}