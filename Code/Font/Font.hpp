// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Font.hpp
* @author JXMaster
* @date 2019/10/7
*/
#pragma once
#include "IFontFile.hpp"
#include "IFontAtlas.hpp"

#ifndef LUNA_FONT_API
#define LUNA_FONT_API
#endif

namespace Luna
{
	namespace Font
	{
		enum class EFontFileFormat : u32
		{
			ttf = 1,	// True Type Font, including .ttf and .ttc
		};

		//! Creates a font file object by parsing the provided font file data.
		//! To load font file, you should first creates a blob that owns the file data (.ttf or .ttc), then you pass the 
		//! data to the font system to create a font object for it. The font data will be referenced by font data and should
		//! not be changed during the font lifetime.
		//! @param[in] data The data of the font file.
		//! @param[in] format The file format of the data font.
		//! @return If succeeded, returns the created font file object which is initialized using the
		//! font data.
		LUNA_FONT_API RP<IFontFile> load_font_file(const Blob& data, EFontFileFormat format);

		//! Creates a new font atlas.
		//! @param[in] fontfile The font file to use.
		//! @param[in] font_index The index of the font in the font file to use.
		//! @param[in] font_height The height of the font to render in pixels, the width of the characters will be automatically scaled.
		LUNA_FONT_API RP<IFontAtlas> new_font_atlas(IFontFile* fontfile, u32 font_index, f32 font_height);

		//! Gets the default font object, which is embedded into the engine and only supports for ASCII codepoint range.
		LUNA_FONT_API P<IFontFile> get_default_font();
	}
}