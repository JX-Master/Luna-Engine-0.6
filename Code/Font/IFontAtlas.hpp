// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFontAtlas.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include <Base/IObject.hpp>
#include <Base/Math.hpp>
#include "IFontFile.hpp"
#include <Image/IImage.hpp>

namespace luna
{
	namespace font
	{
		struct FontAtlasChar
		{
			// ---------------------------- Metrics size for the character ----------------------------

			//! The unscaled size of the character in pixels.
			//! This is used for measuring in layout, not for sampling the atlas image.
			Float2U size;
			//! The position of the top-left point of the character rect based on the character 
			//! origin point.
			//! One you have decided the original point of the character, you compute the top-left
			//! corner of the character rect by (original_point + pos_top_left), and compute the 
			//! bottom-right corner character rect by (original_point + pos_top_left + size).
			//! 
			//! This is used for measuring in layout, not for sampling the atlas image.
			Float2U pos_top_left;
			//! The unscaled width from this character's origin point to next character's origin point.
			//! This is used for measuring in layout, not for sampling the atlas image.
			float32 advance_width;
			//! The unscaled width between the x axis of the origin point to the x of the leftest pixel.
			//! This is used for measuring in layout, not for sampling the atlas image.
			float32 left_side_bearing;

			// -------------------------- Rendering UV rect for the character --------------------------

			//! The top-left UV coordinate of the character in the texture atlas.
			//! This is normalized to [0.0f-1.0f], with original point at top-left of the atlas image.
			Float2U uv_top_left;
			//! The bottom-right UV coordinate of the character in the texture atlas.
			//! This is normalized to [0.0f-1.0f], with original point at top-left of the atlas image.
			Float2U uv_bottom_right;
		};

		//! Describes a font codepoint range when building font atlas.
		struct FontAtlasRange
		{
			//! The codepoint of the first character in the range.
			uint32 start_codepoint;
			//! The number of characters in the range.
			uint32 num_chars;
		};

		//! @interface IFontAtlas
		//! Represents a font atlas texture that is used to render texts. A font atlas pre-bakes all
		//! character bitmaps needed for one specified font and size into one big texture, which can be used to draw 
		//! texts using this font & size later.
		struct IFontAtlas : public IObject
		{
			luiid("{0bf9ceef-15d4-4302-993f-b2eca0a9941d}");

			//! Gets the bound font file.
			virtual IFontFile* get_font_file() = 0;

			//! Gets the index of the font in the font file that this font atlas is rendered.
			virtual uint32 get_font_index() = 0;

			//! Sets the font file and index. 
			//! Call `render` after this to apply changes to the bound bitmap.
			virtual void set_font(IFontFile* fontfile, uint32 font_index) = 0;

			//! Gets the height of the font in the atlas.
			virtual float32 get_font_height() = 0;

			//! Sets the height of the font in the atlas.
			//! Call `render` after this to apply changes to the bound bitmap.
			virtual void set_font_height(float32 font_height) = 0;

			//! Gets the font scale factor. 
			//! The scale factor is used for rendering fonts in High-DPI monitor, where the metrics size
			//! and the rendered size (the size used as UV to sample the texture) is not equal. 
			//! For example, for a 12x18 pixels size character with scale factor 1.5f, the actual render 
			//! size on the font image will be 18x27 pixels, but the character still gets 12x18 sizes when 
			//! you measuring the character to arrange them on screen.
			//! The default scale factor is 1.0f.
			virtual float32 get_scale_factor() = 0;
			
			//! Sets the font scale factor.
			//! Call `render` after this to apply changes to the bound bitmap.
			virtual void set_scale_factor(float32 scale_factor) = 0;

			//! Adds all character glyphs in the specified range into the font atlas. The font atlas
			//! removes duplicated glyphs automatically.
			//! Call `render` after this to apply changes to the bound bitmap.
			virtual void add_glyph_ranges(uint32 num_ranges, const FontAtlasRange* ranges) = 0;

			//! Adds all character glyphs in the provided UTF-8 string into the font atlas. The font 
			//! atlas removes duplicated glyphs automatically.
			//! Call `render` after this to apply changes to the bound bitmap.
			//! The string must be null-terminated.
			virtual void add_glyphs_from_string(const char* str) = 0;

			//! Same as `add_glyphs_from_string`, but takes a non-null-terminated string as parameter.
			//! @param[in] `str` The string to add.
			//! @param[in] `len` The length of the string to scan, which is the number of `char`s in the string.
			virtual void add_glyphs_from_lstring(const char* str, size_t len) = 0;

			//! Removes all glyphs in the specified range from the font atlas. The font atlas
			//! ignores glyphs that do not exist in the atlas automatically.
			//! Call `render` after this to apply changes to the bound bitmap.
			virtual void remove_glyph_ranges(uint32 num_ranges, const FontAtlasRange* ranges) = 0;

			//! Removes all glyphs in the provided UTF-8 string from the font atlas. The font atlas
			//! ignores glyphs that do not exist in the atlas automatically.
			//! Call `render` after this to apply changes to the bound bitmap.
			virtual void remove_glyphs_from_string(const char* str) = 0;

			//! Same as `remove_glyphs_from_string`, but accepts a non-null-terminated string as parameter.
			//! @param[in] `str` The string to remove.
			//! @param[in] `len` The length of the string to scan, which is the number of `char`s in the string.
			virtual void remove_glyphs_from_lstring(const char* str, size_t len) = 0;

			//! Checks if any changes have been made to the font parameters or glyphs in the atlas and
			//! the atlas needs to be rendered again.
			virtual bool is_dirty() = 0;

			//! Renders the added glyphs using the specified parameters.
			//! The render operation will only happen when it is needed. If nothing is changed after
			//! the last `render` call, the atlas will not render the font another time.
			//! @remark Although the font atlas allows changing font parameters and ranges and renders
			//! them on the fly, you should know that rendering the font atlas is a costly operation
			//! and may cause program stalling. So prefer to do this in load time or in a background 
			//! thread for multiple frames.
			virtual void render() = 0;

			//! Gets the generated font atlas image.
			//! The returned image will only have one subimage, which is the rendered texture.
			//! This call returns `nullptr` if the image has been discarded.
			virtual image::IImage* get_rendered_image() = 0;

			//! Discards the generated font atlas image. `is_dirty` returns `true` after this call.
			virtual void discard_rendered_image() = 0;

			//! Gets the metrics information for a font in the vertical side.
			//! ascent is the coordinate above the baseline the font extends; descent
			//! is the coordinate below the baseline the font extends (i.e. it is typically negative)
			//! line_gap is the spacing between one row's descent and the next row's ascent...
			//! so you should advance the vertical position by "*ascent - *descent + *line_gap"
			//!   Unlike those in `IFontFile`, these values are expressed in SCALED coordinates.
			virtual void get_vmetrics(float32* ascent, float32* descent, float32* line_gap) = 0;

			//! Checks if the specified character is contained by this font atlas.
			virtual bool contain_char(codepoint_t char_codepoint) = 0;

			//! Get an additional amount to add to the 'advance' value between ch1 and ch2.
			//! Unlike in `IFontFile`, the returned value has been scaled and it accepts codepoint_t instead
			//! of glyph_t.
			virtual float32 get_kern_advance(codepoint_t ch1, codepoint_t ch2) = 0;

			//! Gets the character rect information used to draw character. The rect information includes the size of the 
			//! character and the texture coordinate rect to index in the font texture.
			//! The texture coordinate ranges in 0-1 and starts from top-left corner of the texture as in Direct3D.
			//! @param[in] char_codepoint The codepoint of the character.
			//! @param[out] ch The FontAtlasChar struct that receives the information for drawing this character.
			//! @return Returns success if the specified codepoint exists in this font atlas, returns failure otherwise.
			//! @remark For robustness, if the specified character does not exist but character "?" exists in the text atlas, 
			//! this function sets `top_left`, `bottom_right` and `size` to character "?". If the character "?" does not exist
			//! in the font atlas either, this function sets `top_left`, `bottom_right` and `size` to the first valid character 
			//! in the font atlas. In both cases, this function will return failure.
			virtual RV get_char(codepoint_t char_codepoint, FontAtlasChar& ch) = 0;

			//! Gets the width of the region to render the text in one line.
			//! @param[in] text The text string to check in UTF-8 encoding.
			//! @param[in] num_chars Number of UTF-16 characters to use in the `text`, if this is `0`, then the full text will be 
			//! used (the text stream until null terminator).
			//! @param[in] spacing An extra spacing value to add between chars.
			virtual float32 get_line_width(const char* text, uint32 num_chars = 0, float32 spacing = 0.0f) = 0;

			//! Gets the height of the region to render the text when the maximum width is specified.
			//! @param[in] region_width The fixed width of the region.
			//! @param[in] text The text string to check in UTF-8 encoding.
			//! @param[out] width The actual width required to render the text. Specify `nullptr` will ignore this parameter.
			//! @param[out] height The actual height required to render the text. Specify `nullptr` will ignore this parameter.
			//! @param[in] num_chars Number of UTF-32 characters to use in the `text`, if this is `0`, then the full text will be
			//! used (the text stream until null terminator).
			//! @param[in] spacing_x An extra spacing value to add between characters.
			//! @param[in] spacing_y An extra spacing value to add between lines.
			virtual void get_draw_region(float32 region_max_width, const char* text, float32* width, float32* height, uint32 num_chars = 0, float32 spacing_x = 0.0f, float32 spacing_y = 0.0f) = 0;
		};
	}
}