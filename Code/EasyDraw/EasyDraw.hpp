// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file EasyDraw.hpp
* @author JXMaster
* @date 2019/10/6
*/
#pragma once
#include <Core/IObject.hpp>
#include <Gfx/Gfx.hpp>
#include "IDrawPath.hpp"
#include "IFontTexture.hpp"
#include "IDrawList.hpp"
#include "IDrawListRenderer.hpp"

#ifndef LUNA_EASY_DRAW_API
#define LUNA_EASY_DRAW_API
#endif

namespace Luna
{
	namespace EasyDraw
	{
		//! Creates a new draw list.
		LUNA_EASY_DRAW_API P<IDrawList> new_draw_list();

		//! Creates a new draw list renderer.
		//! @param[in] render_pass The render pass object for this renderer. This cannot be changed after the renderer is created.
		LUNA_EASY_DRAW_API RP<IDrawListRenderer> new_draw_list_renderer(Gfx::IRenderPass* render_pass);

		//! Create a new path object.
		LUNA_EASY_DRAW_API P<IDrawPath> new_draw_path();

		//! Creates a new font texture.
		//! @param[in] atlas The font atlas used for generating font texture.The font atlas must be prepared so that
		//! `IFontAtlas::is_dirty` is `false`.
		//! @param[in] command_buffer The command buffer used to upload textures. This call returns after the texture has
		//! been uploaded, and this command buffer will be reset.
		//! @remark The texture is created for the device that owns the command buffer used.
		LUNA_EASY_DRAW_API RP<IFontTexture> new_font_texture(Font::IFontAtlas* atlas, Gfx::ICommandBuffer* command_buffer);
	}
}