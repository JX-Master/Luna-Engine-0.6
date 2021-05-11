// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontFileTTF.cpp
* @author JXMaster
* @date 2019/10/7
*/
#define STB_TRUETYPE_IMPLEMENTATION
#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#include "FontFileTTF.hpp"

namespace Luna
{
	namespace Font
	{
		RV FontFileTTF::init(Blob&& data)
		{
			i32 i = stbtt_GetNumberOfFonts((const unsigned char*)data.data());
			if (i <= 0)
			{
				get_error_object() = Error(BasicError::bad_arguments(), "Create TTF font file failed because there is no font in the specified font file data.");
				return BasicError::error_object();
			}
			m_data = move(data);
			m_infos.reserve((u32)i);
			for (u32 j = 0; j < (u32)i; ++j)
			{
				stbtt_fontinfo info;
				stbtt_InitFont(&info, (const unsigned char*)m_data.data(),
					stbtt_GetFontOffsetForIndex((const unsigned char*)m_data.data(), j));
				info.userdata = nullptr;
				m_infos.push_back(info);
			}
			return RV();
		}
		u32 FontFileTTF::count_fonts()
		{
			return u32(m_infos.size());
		}
		glyph_t FontFileTTF::find_glyph(u32 font_index, codepoint_t codepoint)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			int glyph = stbtt_FindGlyphIndex(&m_infos[font_index], codepoint);
			return glyph ? glyph : invalid_glyph_v;
		}
		f32 FontFileTTF::scale_for_pixel_height(u32 font_index, f32 pixels)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			return stbtt_ScaleForPixelHeight(&m_infos[font_index], pixels);
		}
		void FontFileTTF::get_vmetrics(u32 font_index, i32* ascent, i32* descent, i32* lineGap)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			stbtt_GetFontVMetrics(&m_infos[font_index], ascent, descent, lineGap);
		}
		void FontFileTTF::get_glyph_hmetrics(u32 font_index, glyph_t glyph, i32* advance_width, i32* left_side_bearing)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_GetGlyphHMetrics(&m_infos[font_index], glyph, advance_width, left_side_bearing);
		}
		i32 FontFileTTF::get_kern_advance(u32 font_index, glyph_t ch1, glyph_t ch2)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			return stbtt_GetGlyphKernAdvance(&m_infos[font_index], ch1, ch2);
		}
		void FontFileTTF::get_glyph_box(u32 font_index, glyph_t glyph, i32* x0, i32* y0, i32* x1, i32* y1)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_GetGlyphBox(&m_infos[font_index], glyph, x0, y0, x1, y1);
		}
		void FontFileTTF::get_glyph_bitmap_box(u32 font_index, glyph_t glyph, f32 scale_x, f32 scale_y, f32 shift_x, f32 shift_y, i32* ix0, i32* iy0, i32* ix1, i32* iy1)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_GetGlyphBitmapBoxSubpixel(&m_infos[font_index], glyph, scale_x, scale_y, shift_x, shift_y, ix0, iy0, ix1, iy1);
		}
		void FontFileTTF::render_glyph_bitmap(u32 font_index, glyph_t glyph, void* output, i32 out_w, i32 out_h, i32 out_row_pitch, f32 scale_x, f32 scale_y, f32 shift_x, f32 shift_y)
		{
			lucheck_msg(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_MakeGlyphBitmapSubpixel(&m_infos[font_index], (unsigned char*)output, out_w, out_h, out_row_pitch, scale_x, scale_y, shift_x, shift_y, glyph);
		}
	}
}