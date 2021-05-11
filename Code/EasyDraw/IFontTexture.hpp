// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFontTexture.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include <Font/Font.hpp>

namespace Luna
{
	namespace EasyDraw
	{
		//! @interface IFontTexture
		//! Represents a texture resource that contains font information so that it can be used to draw characters.
		//! This is mainly used by `IDrawList`.
		struct IFontTexture : public IObject
		{
			luiid("{3dde422f-118d-4dce-9512-654c07cf92f7}");

			//! Resets the font with a new atlas. The original texture will be discarded.
			//! This should be called after the font atlas image has been changed.
			//! @param[in] atlas The font atlas used for generating font texture. The font atlas must be prepared so that
			//! `IFontAtlas::is_dirty` is `false`.
			//! @param[in] command_buffer The command buffer used to upload textures. The command buffer will be submitted, 
			//! waited and reset before this call returns.
			virtual RV reset(Font::IFontAtlas* atlas, Gfx::ICommandBuffer* command_buffer) = 0;

			//! Gets the generated font texture.
			virtual Gfx::IResource* get_font_texture() = 0;

			//! Gets the font atlas.
			virtual Font::IFontAtlas* get_font_atlas() = 0;
		};
	}
}