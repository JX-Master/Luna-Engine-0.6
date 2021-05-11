// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontAtlas.hpp
* @author JXMaster
* @date 2019/10/6
*/
#pragma once
#include "../IFontAtlas.hpp"
#include "../IFontFile.hpp"
#include "FontSystem.hpp"
#include <Runtime/Vector.hpp>
#include <Runtime/HashMap.hpp>
namespace Luna
{
	namespace Font
	{
		class FontAtlas : public IFontAtlas
		{
		public:
			lucid("{7b909560-9b30-476e-9abf-ce77bc8013e4}");
			luiimpl(FontAtlas, IFontAtlas, IObject);

			P<IFontFile> m_font_file;
			P<Image::IImage> m_image;

			struct FontChar
			{
				// Unscaled size and offset.
				Float4U m_sf;	
				f32 m_advance_width;
				f32 m_left_side_bearing;
				// The texture coordinate.
				Float4U m_rect;
			};

			// Stores glyphs information.
			HashMap<codepoint_t, FontChar> m_glyphs;
			// Stores the size of the texture.
			u32 m_size;
			u32 m_font_index;
			
			f32 m_ascent;
			f32 m_descent;
			f32 m_line_gap;
			f32 m_scale;

			f32 m_font_height;
			f32 m_pixel_scale;

			bool m_is_dirty;

			FontAtlas() :
				m_pixel_scale(1.0f),
				m_is_dirty(false) {}

			bool init(IFontFile* fontfile, u32 font_index, f32 char_height);

			virtual IFontFile* get_font_file() override
			{
				m_font_file->add_ref();
				return m_font_file;
			}

			virtual u32 get_font_index() override
			{
				return m_font_index;
			}

			virtual void set_font(IFontFile* fontfile, u32 font_index) override;

			virtual f32 get_font_height() override
			{
				return m_font_height;
			}

			virtual void set_font_height(f32 font_height) override;

			virtual f32 get_scale_factor() override
			{
				return m_pixel_scale;
			}

			virtual void set_scale_factor(f32 scale_factor) override;

			virtual void add_glyph_ranges(u32 num_ranges, const FontAtlasRange* ranges) override;
			virtual void add_glyphs_from_string(const char* str) override;
			virtual void add_glyphs_from_lstring(const char* str, usize len) override;
			virtual void remove_glyph_ranges(u32 num_ranges, const FontAtlasRange* ranges) override;
			virtual void remove_glyphs_from_string(const char* str) override;
			virtual void remove_glyphs_from_lstring(const char* str, usize len) override;
			virtual bool is_dirty() override
			{
				return m_is_dirty;
			}

			virtual void render() override;
			virtual Image::IImage* get_rendered_image() override
			{
				return m_image;
			}
			virtual void discard_rendered_image() override
			{
				m_image = nullptr;
				m_is_dirty = true;
			}
			virtual void get_vmetrics(f32* ascent, f32* descent, f32* line_gap) override
			{
				*ascent = m_ascent;
				*descent = m_descent;
				*line_gap = m_line_gap;
			}
			virtual bool contain_char(codepoint_t char_codepoint) override;
			virtual f32 get_kern_advance(codepoint_t ch1, codepoint_t ch2) override;
			virtual RV get_char(codepoint_t char_codepoint, FontAtlasChar& ch) override;
			virtual f32 get_line_width(const char* text, u32 num_chars, f32 spacing) override;
			virtual void get_draw_region(f32 region_width, const char* text, f32* width, f32* height, u32 num_chars, f32 spacing_x, f32 spacing_y) override;
		};
	}
}