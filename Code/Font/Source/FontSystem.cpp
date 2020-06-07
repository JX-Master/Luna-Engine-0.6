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
namespace luna
{
	namespace font
	{
		P<IFontFile> g_default_font;
		void deinit()
		{
			g_default_font = nullptr;
		}
		void init()
		{
			g_default_font = load_font_file(new_blob(opensans_regular_ttf, opensans_regular_ttf_size), EFontFileFormat::ttf).get();
			add_module("Font", deinit);
		}
		RP<IFontFile> load_font_file(IBlob* data, EFontFileFormat format)
		{
			switch (format)
			{
			case EFontFileFormat::ttf:
			{
				P<FontFileTTF> f = box_ptr(new_obj<FontFileTTF>(get_module_allocator()));
				lutry
				{
					luexp(f->init(data));
				}
				lucatchret;
				return f;
			}
			default:
				lupanic();
				break;
			}
			return e_failure;
		}
		RP<IFontAtlas> new_font_atlas(font::IFontFile* fontfile, uint32 font_index, float32 char_height)
		{
			P<FontAtlas> atlas = box_ptr(new_obj<FontAtlas>(get_module_allocator()));
			if (!atlas->init(fontfile, font_index, char_height))
			{
				return e_failure;
			}
			return atlas;
		}
		P<IFontFile> get_default_font()
		{
			return g_default_font;
		}
	}
}