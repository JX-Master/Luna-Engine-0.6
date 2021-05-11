// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontFileTTF.hpp
* @author JXMaster
* @date 2019/10/7
*/
#pragma once
#include "FontHeader.hpp"
#include "StbTrueType.hpp"
#include <Runtime/Vector.hpp>
#include "FontSystem.hpp"
#include <Core/Interface.hpp>

namespace Luna
{
	namespace Font
	{
		class FontFileTTF : public IFontFile
		{
		public:
			lucid("{5ead091d-2a74-49b8-9201-f97b7d19685f}");
			luiimpl(FontFileTTF, IFontFile, IObject);

			Blob m_data;
			Vector<stbtt_fontinfo> m_infos;

			FontFileTTF() {}

			RV init(Blob&& data);
			virtual const Blob& data() override
			{
				return m_data;
			}
			virtual u32 count_fonts() override;
			virtual glyph_t find_glyph(u32 font_index, codepoint_t codepoint) override;
			virtual f32 scale_for_pixel_height(u32 font_index, f32 pixels) override;
			virtual void get_vmetrics(u32 font_index, i32* ascent, i32* descent, i32* lineGap) override;
			virtual void get_glyph_hmetrics(u32 font_index, glyph_t glyph, i32 *advance_width, i32 *left_side_bearing) override;
			virtual i32 get_kern_advance(u32 font_index, glyph_t ch1, glyph_t ch2) override;
			virtual void get_glyph_box(u32 font_index, glyph_t glyph, i32* x0, i32* y0, i32* x1, i32* y1) override;
			virtual void get_glyph_bitmap_box(u32 font_index, glyph_t glyph, f32 scale_x, f32 scale_y, f32 shift_x, f32 shift_y,
				i32* ix0, i32* iy0, i32* ix1, i32* iy1) override;
			virtual void render_glyph_bitmap(u32 font_index, glyph_t glyph, void* output, i32 out_w, i32 out_h, i32 out_row_pitch,
				f32 scale_x, f32 scale_y, f32 shift_x, f32 shift_y) override;
			
		};
	}
}