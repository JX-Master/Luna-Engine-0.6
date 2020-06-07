// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFontFile.hpp
* @author JXMaster
* @date 2019/10/7
*/
#pragma once
#include <Base/IObject.hpp>
#include <Core/IBlob.hpp>

namespace luna
{
	namespace font
	{
		using glyph_t = int32;
		constexpr glyph_t invalid_glyph_v = -1;
		using codepoint_t = uint32;
		//! @interface IFontFile
		//! Represents a font file that may contain one or multiple fonts.
		struct IFontFile : public IObject
		{
			luiid("{989fe385-6d98-480d-89ab-6b7dd3ec5624}");

			//! Gets the data of the font file.
			virtual IBlob* data() = 0;

			//! Gets the number of fonts in the font file.
			virtual uint32 count_fonts() = 0;

			//! Gets the glyph index of the specified character in specified font.
			virtual glyph_t find_glyph(uint32 font_index, codepoint_t codepoint) = 0;

			//! Computes a scale factor to produce a font whose "height" is 'pixels' tall.
			//! Height is measured as the distance from the highest ascender to the lowest
			//! descender; in other words, it's equivalent to calling get_vmetrics
			//! and computing:
			//!       scale = pixels / (ascent - descent)
			//! so if you prefer to measure height by the ascent only, use a similar calculation.
			virtual float32 scale_for_pixel_height(uint32 font_index, float32 pixels) = 0;

			//! Gets the metrics information for a font in the vertical side.
			//! ascent is the coordinate above the baseline the font extends; descent
			//! is the coordinate below the baseline the font extends (i.e. it is typically negative)
			//! line_gap is the spacing between one row's descent and the next row's ascent...
			//! so you should advance the vertical position by "*ascent - *descent + *line_gap"
			//!   these are expressed in unscaled coordinates, so you must multiply by
			//!   the scale factor for a given size
			virtual void get_vmetrics(uint32 font_index, int32* ascent, int32* descent, int32* line_gap) = 0;

			//! Gets the metrics information for a glyph in the horizontal side.
			//! left_side_bearing is the offset from the current horizontal position to the left edge of the character
			//! advance_width is the offset from the current horizontal position to the next horizontal position
			//!   these are expressed in unscaled coordinates.
			virtual void get_glyph_hmetrics(uint32 font_index, glyph_t glyph, int32 *advance_width, int32 *left_side_bearing) = 0;

			//! Get an additional amount to add to the 'advance' value between ch1 and ch2.
			virtual int32 get_kern_advance(uint32 font_index, glyph_t ch1, glyph_t ch2) = 0;

			//! Gets the bounding box of the visible part of the glyph, in unscaled coordinates.
			virtual void get_glyph_box(uint32 font_index, glyph_t glyph, int32* x0, int32* y0, int32* x1, int32* y1) = 0;

			//! Gets the bounding box of the bitmap centered around the glyph origin; so the
			//! bitmap width is ix1-ix0, height is iy1-iy0, and location to place the bitmap top left is (left_side_bearing * scale,iy0).
			//! (Note that the bitmap uses y-increases-down, but the shape uses y-increases-up, so get_glyph_bitmap_box and get_glyph_box are inverted.)
			virtual void get_glyph_bitmap_box(uint32 font_index, glyph_t glyph, float32 scale_x, float32 scale_y, float32 shift_x, float32 shift_y,
				int32* ix0, int32* iy0, int32* ix1, int32* iy1) = 0;

			//! Renders a bitmap of the specified glyph into the buffer specified by the `output`, where `out_w` and `out_h` is the width and height
			//! of region of the buffer to be used, and `out_row_pitch` is the stride size of one row of the buffer.
			//! `scale_x` and `scale_y` is the scale factor computed through `scale_for_pixel_height` call, and `shift_x`, `shify_y` is the shift of
			//! the glyph in pixels.
			//!
			//! The rendered bitmap is a one-channel bitmap with each pixel take one byte. The value ranges in 0-255.
			//! 
			//! This call uses CPU to rasterize the glyph, since the glyph bitmap is usually very small, this usually does not cause performance issue,
			//! but you should save the render result whenever possible.
			virtual void render_glyph_bitmap(uint32 font_index, glyph_t glyph, void* output, int32 out_w, int32 out_h, int32 out_row_pitch,
				float32 scale_x, float32 scale_y, float32 shift_x, float32 shift_y) = 0;
		};
	}
}