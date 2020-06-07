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

namespace luna
{
	namespace font
	{
		RV FontFileTTF::init(IBlob* data)
		{
			int32 i = stbtt_GetNumberOfFonts((const unsigned char*)data->data());
			if (i <= 0)
			{
				set_err(e_bad_arguments, "Create TTF font file failed because there is no font in the specified font file data.");
				return e_user_failure;
			}
			m_data = data;
			m_infos.reserve((uint32)i);
			for (uint32 j = 0; j < (uint32)i; ++j)
			{
				stbtt_fontinfo info;
				stbtt_InitFont(&info, (const unsigned char*)m_data->data(),
					stbtt_GetFontOffsetForIndex((const unsigned char*)m_data->data(), j));
				info.userdata = get_module_allocator();
				m_infos.push_back(info);
			}
			return s_ok;
		}
		uint32 FontFileTTF::count_fonts()
		{
			return uint32(m_infos.size());
		}
		glyph_t FontFileTTF::find_glyph(uint32 font_index, codepoint_t codepoint)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			int glyph = stbtt_FindGlyphIndex(&m_infos[font_index], codepoint);
			return glyph ? glyph : invalid_glyph_v;
		}
		float32 FontFileTTF::scale_for_pixel_height(uint32 font_index, float32 pixels)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			return stbtt_ScaleForPixelHeight(&m_infos[font_index], pixels);
		}
		void FontFileTTF::get_vmetrics(uint32 font_index, int32* ascent, int32* descent, int32* lineGap)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			stbtt_GetFontVMetrics(&m_infos[font_index], ascent, descent, lineGap);
		}
		void FontFileTTF::get_glyph_hmetrics(uint32 font_index, glyph_t glyph, int32* advance_width, int32* left_side_bearing)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_GetGlyphHMetrics(&m_infos[font_index], glyph, advance_width, left_side_bearing);
		}
		int32 FontFileTTF::get_kern_advance(uint32 font_index, glyph_t ch1, glyph_t ch2)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			return stbtt_GetGlyphKernAdvance(&m_infos[font_index], ch1, ch2);
		}
		void FontFileTTF::get_glyph_box(uint32 font_index, glyph_t glyph, int32* x0, int32* y0, int32* x1, int32* y1)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_GetGlyphBox(&m_infos[font_index], glyph, x0, y0, x1, y1);
		}
		void FontFileTTF::get_glyph_bitmap_box(uint32 font_index, glyph_t glyph, float32 scale_x, float32 scale_y, float32 shift_x, float32 shift_y, int32* ix0, int32* iy0, int32* ix1, int32* iy1)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_GetGlyphBitmapBoxSubpixel(&m_infos[font_index], glyph, scale_x, scale_y, shift_x, shift_y, ix0, iy0, ix1, iy1);
		}
		void FontFileTTF::render_glyph_bitmap(uint32 font_index, glyph_t glyph, void* output, int32 out_w, int32 out_h, int32 out_row_pitch, float32 scale_x, float32 scale_y, float32 shift_x, float32 shift_y)
		{
			luassert_msg_usr(font_index < m_infos.size(), "Invalid font index.");
			if (glyph == invalid_glyph_v)
			{
				glyph = 0;
			}
			stbtt_MakeGlyphBitmapSubpixel(&m_infos[font_index], (unsigned char*)output, out_w, out_h, out_row_pitch, scale_x, scale_y, shift_x, shift_y, glyph);
		}
	}
}