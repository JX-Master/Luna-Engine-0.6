// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontTexture.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include "EasyDrawHeader.hpp"

namespace Luna
{
	namespace EasyDraw
	{
		class FontTexture : public IFontTexture
		{
		public:
			luiid("{769ce73d-1b69-4e62-b715-e86a9e31e240}");
			luiimpl(FontTexture, IFontTexture, IObject);

			P<Gfx::IResource> m_tex;
			P<Font::IFontAtlas> m_font;

			FontTexture() {}

			virtual RV reset(Font::IFontAtlas* atlas, Gfx::ICommandBuffer* command_buffer) override;
			virtual Gfx::IResource* get_font_texture() override
			{
				return m_tex;
			}
			virtual Font::IFontAtlas* get_font_atlas() override
			{
				return m_font;
			}
		};
	}
}