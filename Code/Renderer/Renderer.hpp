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

namespace Luna
{
	namespace Renderer
	{

		//! Gets the main graphic device.
		LUNA_RENDERER_API Gfx::IGraphicDevice* device();

		//! Gets the main graphic command queue for the main graphic device.
		LUNA_RENDERER_API Gfx::ICommandQueue* main_graphic_queue();

		//! Gets the main compute command queue for the main graphic device.
		LUNA_RENDERER_API Gfx::ICommandQueue* main_compute_queue();

		//! Gets the main copy command queue for the main graphic device.
		LUNA_RENDERER_API Gfx::ICommandQueue* main_copy_queue();
	}
}