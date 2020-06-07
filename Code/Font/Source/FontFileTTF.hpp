// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontFileTTF.hpp
* @author JXMaster
* @date 2019/10/7
*/
#pragma once
#include "FontHeader.hpp"
#include "StbTrueType.hpp"
#include <Base/Vector.hpp>
#include "FontSystem.hpp"
#include <Base/Interface.hpp>

namespace luna
{
	namespace font
	{
		class FontFileTTF : public IFontFile
		{
		public:
			lucid("{5ead091d-2a74-49b8-9201-f97b7d19685f}");
			luiimpl(FontFileTTF, IFontFile, IObject);

			P<IBlob> m_data;
			Vector<stbtt_fontinfo> m_infos;

			FontFileTTF(IAllocator* alloc) :
				luibind(alloc),
				m_infos(alloc) {}

			RV init(IBlob* data);
			virtual IBlob* data() override
			{
				return m_data;
			}
			virtual uint32 count_fonts() override;
			virtual glyph_t find_glyph(uint32 font_index, codepoint_t codepoint) override;
			virtual float32 scale_for_pixel_height(uint32 font_index, float32 pixels) override;
			virtual void get_vmetrics(uint32 font_index, int32* ascent, int32* descent, int32* lineGap) override;
			virtual void get_glyph_hmetrics(uint32 font_index, glyph_t glyph, int32 *advance_width, int32 *left_side_bearing) override;
			virtual int32 get_kern_advance(uint32 font_index, glyph_t ch1, glyph_t ch2) override;
			virtual void get_glyph_box(uint32 font_index, glyph_t glyph, int32* x0, int32* y0, int32* x1, int32* y1) override;
			virtual void get_glyph_bitmap_box(uint32 font_index, glyph_t glyph, float32 scale_x, float32 scale_y, float32 shift_x, float32 shift_y,
				int32* ix0, int32* iy0, int32* ix1, int32* iy1) override;
			virtual void render_glyph_bitmap(uint32 font_index, glyph_t glyph, void* output, int32 out_w, int32 out_h, int32 out_row_pitch,
				float32 scale_x, float32 scale_y, float32 shift_x, float32 shift_y) override;
			
		};
	}
}