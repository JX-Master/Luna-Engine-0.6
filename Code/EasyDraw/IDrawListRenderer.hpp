// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IDrawListRenderer.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include <Base/IObject.hpp>
#include <Gfx/ICommandQueue.hpp>
#include <Gfx/IResource.hpp>

namespace luna
{
	namespace edraw
	{
		struct IDrawList;
		//! @interface IDrawListRenderer
		//! A simple renderer to render IDrawList primitives.
		struct IDrawListRenderer : public IObject
		{
			luiid("{f373cd47-d5fa-4812-bbfa-628fafb518e8}");

			//! Gets the device bound with this renderer.
			virtual gfx::IGraphicDevice* get_device() = 0;

			//! Gets the render pass bound with this renderer.
			virtual gfx::IRenderPass* get_render_pass() = 0;

			//! Adds all draw calls of the draw list into the renderer. 
			//! The render holds reference to the draw list until `submit` is called.
			//! @param[in] draw_list The draw list to add.
			virtual void add_draw_list(IDrawList* draw_list) = 0;

			//! Adds a specified range of draw calls into the renderer.
			//! @param[in] draw_list The draw list that holds the draw calls.
			//! @param[in] first The index of the first draw call to add.
			//! @param[in] num_draw_calls The number of draw calls to add.
			virtual void add_draw_calls(IDrawList* draw_list, uint32 first, uint32 num_draw_calls) = 0;

			//! Flushes all recorded draw lists and generate draw calls for the lists to the command buffer. 
			//! @param[in] target_cmd_buffer The command buffer to generate render commands to.
			//! @param[in] rt The render target resource.
			//! @param[in] width The scaled width of the render target. This can be different from the size of the frame buffer to enable High-DPI.
			//! @param[in] height The scaled width of the render target. This can be different from the size of the frame buffer to enable High-DPI.
			virtual RV render(gfx::ICommandBuffer* target_cmd_buffer, gfx::IResource* rt, uint32 width, uint32 height) = 0;
		};
	}
}