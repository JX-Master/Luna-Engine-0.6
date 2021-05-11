// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontSystem.cpp
* @author JXMaster
* @date 2019/10/7
*/
#include "FontSystem.hpp"
#include "FontFileTTF.hpp"
#include "FontAtlas.hpp"
#include "DefaultFont.hpp"
#include <Runtime/Module.hpp>
namespace Luna
{
	namespace Font
	{
		P<IFontFile> g_default_font;
		void deinit()
		{
			g_default_font = nullptr;
		}
		RV init()
		{
			auto r = load_font_file(Blob(opensans_regular_ttf, opensans_regular_ttf_size), EFontFileFormat::ttf);
			if (failed(r))
			{
				return r.errcode();
			}
			g_default_font = r.get();
			return RV();
		}
		StaticRegisterModule m("Font", "Core;Gfx;Image;RectPack", init, deinit);
		RP<IFontFile> load_font_file(const Blob& data, EFontFileFormat format)
		{
			switch (format)
			{
			case EFontFileFormat::ttf:
			{
				P<FontFileTTF> f = newobj<FontFileTTF>();
				lutry
				{
					luexp(f->init(Blob(data)));
				}
				lucatchret;
				return f;
			}
			default:
				lupanic();
				break;
			}
			return BasicError::not_supported();
		}
		RP<IFontAtlas> new_font_atlas(Font::IFontFile* fontfile, u32 font_index, f32 char_height)
		{
			P<FontAtlas> atlas = newobj<FontAtlas>();
			if (!atlas->init(fontfile, font_index, char_height))
			{
				return BasicError::failure();
			}
			return atlas;
		}
		P<IFontFile> get_default_font()
		{
			return g_default_font;
		}
	}
}