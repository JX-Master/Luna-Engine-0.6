// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontTexture.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include "EasyDrawHeader.hpp"

namespace luna
{
	namespace edraw
	{
		class FontTexture : public IFontTexture
		{
		public:
			luiid("{769ce73d-1b69-4e62-b715-e86a9e31e240}");
			luiimpl(FontTexture, IFontTexture, IObject);

			P<gfx::IResource> m_tex;
			P<font::IFontAtlas> m_font;

			FontTexture(IAllocator* alloc) :
				luibind(alloc) {}

			virtual RV reset(font::IFontAtlas* atlas, gfx::ICommandBuffer* command_buffer) override;
			virtual gfx::IResource* get_font_texture() override
			{
				return m_tex;
			}
			virtual font::IFontAtlas* get_font_atlas() override
			{
				return m_font;
			}
		};
	}
}