// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PrimitiveRenderer2D.cpp
* @author JXMaster
* @date 2019/9/24
*/
#include "DrawList.hpp"
#include "EasyDraw.hpp"
#include <Base/Unicode.hpp>

namespace luna
{
	namespace edraw
	{
		using namespace gfx;
		void DrawList::new_draw_call()
		{
			m_bufs.emplace_back(DrawCallBuf(get_module_allocator()));
			DrawCallBuf& dc = m_bufs.back();
			dc.m_texture = m_texture;
			dc.origin_x = m_left_offset;
			dc.origin_y = m_top_offset;
			dc.rotation = m_rotation;
			dc.clip_rect = m_clip_rect;
			dc.sampler = m_sampler;
		}
		DrawList::DrawCallBuf* DrawList::get_target_drawcall()
		{
			// If the state is changed, or barrier is issued, we need to make a new 
			// draw call.
			if (m_state_dirty || m_dc_barrier_index == m_bufs.size())
			{
				// If state is changed, we find one available draw call and sets it 
				// to be the current draw call.
				for (uint32 i = m_dc_barrier_index; i < m_bufs.size(); ++i)
				{
					DrawCallBuf& dc = m_bufs[i];
					if (state_equal(i))
					{
						m_state_dirty = false;
						m_target_dc_index = i;
						return &m_bufs[m_target_dc_index];
					}
				}
				new_draw_call();
				m_state_dirty = false;
				m_target_dc_index = (int32)m_bufs.size() - 1;
				return &m_bufs[m_target_dc_index];
			}
			else
			{
				return &m_bufs[m_target_dc_index];
			}
		}
		bool DrawList::state_equal(uint32 index)
		{
			DrawCallBuf& dc = m_bufs[index];
			if (dc.m_texture == m_texture &&
				dc.origin_x == m_left_offset &&
				dc.origin_y == m_top_offset &&
				dc.rotation == m_rotation &&
				dc.clip_rect == m_clip_rect &&
				dc.sampler == m_sampler)
			{
				return true;
			}
			return false;
		}
		void DrawList::init()
		{
			reset();
			m_path = new_draw_path();
		}
		void DrawList::drawcall_barrier()
		{
			m_dc_barrier_index = (uint32)m_bufs.size();
		}
		void DrawList::reset()
		{
			m_bufs.clear();
			m_texture = nullptr;
			m_top_offset = 0.0f;
			m_left_offset = 0.0f;
			m_rotation = 0.0f;
			m_clip_rect = RectI(0, 0, 0, 0);
			set_sampler(nullptr);
			m_dc_barrier_index = 0;
			m_target_dc_index = 0;
			m_state_dirty = false;
		}
		void DrawList::append_draw_list(IDrawList* draw_list, EDrawListAppendFlag flags)
		{
			drawcall_barrier();	// The appended draw calls does not get merged with previous draw calls.
			DrawList* d = static_cast<DrawList*>(draw_list);
			m_bufs.reserve(m_bufs.size() + m_bufs.size());
			for (size_t i = 0; i < d->m_bufs.size(); ++i)
			{
				m_bufs.emplace_back(get_module_allocator());
				DrawCallBuf& buf = m_bufs.back();
				if ((flags & EDrawListAppendFlag::copy_data) != EDrawListAppendFlag::none)
				{
					buf.m_vertices = d->m_bufs[i].m_vertices;
					buf.m_indices = d->m_bufs[i].m_indices;
				}
				else
				{
					buf.m_vertices = move(d->m_bufs[i].m_vertices);
					buf.m_indices = move(d->m_bufs[i].m_indices);
				}
				buf.m_texture = d->m_bufs[i].m_texture;
				buf.clip_rect = d->m_bufs[i].clip_rect;
				buf.sampler = d->m_bufs[i].sampler;
				if ((flags & EDrawListAppendFlag::inherit_transform) != EDrawListAppendFlag::none)
				{
					buf.origin_x = d->m_bufs[i].origin_x + m_left_offset;
					buf.origin_y = d->m_bufs[i].origin_y + m_top_offset;
					buf.rotation = d->m_bufs[i].rotation + m_rotation;
				}
				else
				{
					buf.origin_x = d->m_bufs[i].origin_x;
					buf.origin_y = d->m_bufs[i].origin_y;
					buf.rotation = d->m_bufs[i].rotation;
				}
			}
			drawcall_barrier();
			if ((flags & EDrawListAppendFlag::copy_data) == EDrawListAppendFlag::none)
			{
				d->m_bufs.clear();
				d->m_dc_barrier_index = 0;
				d->m_target_dc_index = 0;
				d->m_state_dirty = false;
			}
		}
		void DrawList::draw_triangle_list(uint32 num_vertices, const PrimitiveVertex* vertices,
			uint32 num_indices, const uint32* indices)
		{
			auto dc = get_target_drawcall();

			uint32 idx_offset = (uint32)dc->m_vertices.size();
			for (uint32 i = 0; i < num_vertices; ++i)
			{
				dc->m_vertices.push_back(vertices[i]);
			}
			for (uint32 i = 0; i < num_indices; ++i)
			{
				dc->m_indices.push_back(indices[i] + idx_offset);
			}
		}

		R<TextDrawResult> DrawList::draw_text(const char* text, const Float2& top_left, const Float2& region_size, const Color& col, const Float2& spacing)
		{
			if (!m_font)
			{
				set_err(e_failure, "IDrawList::draw_text_region - No font texture is bound, text will not be drawn.");
				return e_user_failure;
			}
			TextDrawResult res;
			// backup the previous state.
			P<IResource> prior_tex = m_texture;
			gfx::SamplerDesc prior_sampler = m_sampler;
			lutry
			{
				P<IResource> font_tex = m_font->get_font_texture();
				set_texture(font_tex);

				float32 c2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				set_sampler(&SamplerDesc(EFilter::min_mag_mip_linear, ETextureAddressMode::clamp, ETextureAddressMode::clamp,
					ETextureAddressMode::clamp, 0.0f, 1, EComparisonFunc::always, c2, 0.0f, 1.0f));

				float32 ascent, descent, line_gap;
				m_font->get_font_atlas()->get_vmetrics(&ascent, &descent, &line_gap);

				float32 row_gap = ascent - descent + line_gap;
				float32 row_adv = 0.0f;
				float32 char_adv = 0.0f;
				float32 max_char_adv = char_adv;
				const char* cur_char = text;
				while (*cur_char && (!region_size.y || (row_adv + row_gap) < region_size.y))
				{
					// The y value of the base line.
					float32 row_ori = row_adv + ascent;
					while (*cur_char && (!region_size.x || char_adv < region_size.x))
					{
						// Get information about the char.
						char32_t c = utf8_decode_char(cur_char);
						font::FontAtlasChar ch_info;
						luexp(m_font->get_font_atlas()->get_char(c, ch_info));
						if (region_size.x && (ch_info.size.x + ch_info.left_side_bearing + char_adv > region_size.x))
						{
							// Jump to next line.
							break;
						}
						// advance char string.
						cur_char += utf8_charspan(c);
						if (ch_info.size.x != 0.0f && ch_info.size.y != 0.0f)
						{
							/*
							char_x
							|
							char_y->0---1 -
							| / | | char_height / csize.y
							2---3 -
							|---|
							csize.x
							*/
							PrimitiveVertex v[4];
							float32 char_x = top_left.x + char_adv + ch_info.pos_top_left.x;
							float32 char_y = top_left.y + row_ori + ch_info.pos_top_left.y;
							v[0].pos.x = char_x;
							v[0].pos.y = char_y;
							v[1].pos.x = char_x + ch_info.size.x;
							v[1].pos.y = char_y;
							v[2].pos.x = char_x;
							v[2].pos.y = char_y + ch_info.size.y;
							v[3].pos.x = char_x + ch_info.size.x;
							v[3].pos.y = char_y + ch_info.size.y;
							v[0].color = col;
							v[1].color = col;
							v[2].color = col;
							v[3].color = col;
							v[0].uv = ch_info.uv_top_left;
							v[1].uv = Float2U(ch_info.uv_bottom_right.x, ch_info.uv_top_left.y);
							v[2].uv = Float2U(ch_info.uv_top_left.x, ch_info.uv_bottom_right.y);
							v[3].uv = ch_info.uv_bottom_right;
							uint32 indices[] = { 0, 1, 2, 1, 3, 2 };
							draw_triangle_list(4, v, 6, indices);
						}

						char32_t nc = utf8_decode_char(cur_char);
						float32 kern;
						if (nc)
						{
							kern = m_font->get_font_atlas()->get_kern_advance((font::codepoint_t)c, (font::codepoint_t)nc);
						}
						else
						{
							kern = 0.0f;
						}
						max_char_adv = max(max_char_adv, char_adv);
						char_adv += ch_info.advance_width + spacing.x + kern;
					}
					// Jump to next line.
					row_adv += (row_gap + spacing.y);

					char_adv = 0.0f;
				}
				res.region_width = max_char_adv;
				res.region_height = row_adv;
				res.overflow = *cur_char ? true : false;
				set_sampler(&prior_sampler);
				set_texture(prior_tex);
			}
			lucatch
			{
				set_sampler(&prior_sampler);
				set_texture(prior_tex);
				return lures;
			}
			return res;
		}

		void DrawList::draw_triangle_filled(const Float2& p1, const Float2& p2, const Float2& p3, const Color& col, bool antialiased)
		{
			m_path->begin(p1, col);
			m_path->line_to(p2);
			m_path->line_to(p3);
			m_path->end_filled_convex(antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_triangle_bordered(const Float2& p1, const Float2& p2, const Float2& p3, const Color& col, float32 line_width, bool antialiased)
		{
			m_path->begin(p1, col);
			m_path->line_to(p2);
			m_path->line_to(p3);
			m_path->end_stroke(line_width, antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_rectangle_filled(const Float2& top_left, const Float2& size, const Color& col)
		{
			m_path->begin(top_left, col);
			m_path->line_to(Float2(top_left.x + size.x, top_left.y));
			m_path->line_to(top_left + size);
			m_path->line_to(Float2(top_left.x, top_left.y + size.y));
			m_path->end_filled_convex();
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_rectangle_bordered(const Float2& top_left, const Float2& size, const Color& col, float32 line_width)
		{
			m_path->begin(top_left, col);
			m_path->line_to(Float2(top_left.x + size.x, top_left.y));
			m_path->line_to(top_left + size);
			m_path->line_to(Float2(top_left.x, top_left.y + size.y));
			m_path->end_stroke(line_width, true);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_circle_filled(const Float2& center, float32 radius, const Color& col, bool antialiased, uint32 num_segments)
		{
			float32 delta = two_pi / (float32)num_segments;
			float32 angle = delta / 2.0f;
			m_path->begin(Float2(center.x + radius * cosf(angle), center.y + radius * sinf(angle)), col);
			for (uint32 i = 0; i < num_segments - 1; ++i)
			{
				angle -= delta;
				m_path->line_to(Float2(center.x + radius * cosf(angle), center.y + radius * sinf(angle)));
			}
			m_path->end_filled_convex(true, antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_circle_bordered(const Float2& center, float32 radius, const Color& col, float32 line_width, bool antialiased, uint32 num_segments)
		{
			float32 delta = two_pi / (float32)num_segments;
			float32 angle = delta / 2.0f;
			m_path->begin(Float2(center.x + radius * cosf(angle), center.y + radius * sinf(angle)), col);
			for (uint32 i = 0; i < num_segments - 1; ++i)
			{
				angle -= delta;
				m_path->line_to(Float2(center.x + radius * cosf(angle), center.y + radius * sinf(angle)));
			}
			m_path->end_stroke(line_width, true, antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_rounded_rectangle_filled(const Float2& top_left, const Float2& size, const Color& col, float32 round_radius, bool antialiased)
		{
			m_path->begin(Float2(top_left.x + round_radius, top_left.y), col);
			m_path->line_to(Float2(top_left.x + size.x - round_radius, top_left.y));
			m_path->arc_to(Float2(top_left.x + size.x - round_radius, top_left.y + round_radius), round_radius, 270.0f, 360.0f, true, false, 8);
			m_path->line_to(Float2(top_left.x + size.x, top_left.y + size.y - round_radius));
			m_path->arc_to(Float2(top_left.x + size.x - round_radius, top_left.y + size.y - round_radius), round_radius, 0.0f, 90.0f, true, false, 8);
			m_path->line_to(Float2(top_left.x + round_radius, top_left.y + size.y));
			m_path->arc_to(Float2(top_left.x + round_radius, top_left.y + size.y - round_radius), round_radius, 90.0f, 180.0f, true, false, 8);
			m_path->line_to(Float2(top_left.x, top_left.y + round_radius));
			m_path->arc_to(Float2(top_left.x + round_radius, top_left.y + round_radius), round_radius, 180.0f, 270.0f, true, true, 8);
			m_path->end_filled_convex(true, antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_rounded_rectangle_bordered(const Float2& top_left, const Float2& size, const Color& col, float32 round_radius, float32 line_width, bool antialiased)
		{
			m_path->begin(Float2(top_left.x + round_radius, top_left.y), col);
			m_path->line_to(Float2(top_left.x + size.x - round_radius, top_left.y));
			m_path->arc_to(Float2(top_left.x + size.x - round_radius, top_left.y + round_radius), round_radius, 270.0f, 360.0f);
			m_path->line_to(Float2(top_left.x + size.x, top_left.y + size.y - round_radius));
			m_path->arc_to(Float2(top_left.x + size.x - round_radius, top_left.y + size.y - round_radius), round_radius, 0.0f, 90.0f);
			m_path->line_to(Float2(top_left.x + round_radius, top_left.y + size.y));
			m_path->arc_to(Float2(top_left.x + round_radius, top_left.y + size.y - round_radius), round_radius, 90.0f, 180.0f);
			m_path->line_to(Float2(top_left.x, top_left.y + round_radius));
			m_path->arc_to(Float2(top_left.x + round_radius, top_left.y + round_radius), round_radius, 180.0f, 270.0f, true, true);
			m_path->end_stroke(line_width, true, antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_line(const Float2& pos1, const Float2& pos2, const Color& col, float32 line_width, bool antialiased)
		{
			m_path->begin(pos1, col);
			m_path->line_to(pos2);
			m_path->end_stroke(line_width, antialiased);
			m_path->submit_triangle_list(this);
		}

		void DrawList::draw_texture(const Float2& top_left, const Float2& size, const Color& tint, gfx::IResource* tex, gfx::EResourceState state, const Float2& top_left_uv, const Float2& bottom_right_uv)
		{
			m_path->begin(top_left, tint, Float2(0.0f, 0.0f));
			m_path->set_texcoord(Float2(1.0f, 0.0f));
			m_path->line_to(Float2(top_left.x + size.x, top_left.y));
			m_path->set_texcoord(Float2(1.0f, 1.0f));
			m_path->line_to(top_left + size);
			m_path->set_texcoord(Float2(0.0f, 1.0f));
			m_path->line_to(Float2(top_left.x, top_left.y + size.y));
			m_path->end_filled_convex();
			P<IResource> pri_tex = m_texture;
			set_texture(tex);
			m_path->submit_triangle_list(this);
			set_texture(pri_tex);
		}
	}
}