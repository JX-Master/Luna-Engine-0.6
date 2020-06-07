// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DrawList.hpp
* @author JXMaster
* @date 2019/9/24
*/
#pragma once
#include "EasyDrawHeader.hpp"
#include <Base/Vector.hpp>
#include "EasyDraw.hpp"

namespace luna
{
	namespace edraw
	{
		class DrawList : public IDrawList
		{
		public:
			lucid("{50802053-a146-4e27-bb1d-44bb51da41a7}");
			luiimpl(DrawList, IDrawList, IObject);

			struct DrawCallBuf
			{
				Vector<PrimitiveVertex> m_vertices;
				Vector<uint32> m_indices;
				P<gfx::IResource> m_texture;
				gfx::SamplerDesc sampler;
				float32 rotation;
				float32 origin_x;
				float32 origin_y;
				RectI clip_rect;

				DrawCallBuf(IAllocator* alloc) :
					m_vertices(alloc),
					m_indices(alloc) {}
			};

			Vector<DrawCallBuf> m_bufs;

			// There is a range of draw calls that can accept new elements, which starts from m_dc_barrier_index,
			// and end with m_bufs.size().
			// The draw calls after this index can accept new elements.
			uint32 m_dc_barrier_index;
			// The target draw call index for the current pipeline state.
			int32 m_target_dc_index;

			// draw context state.
			P<gfx::IResource> m_texture;
			gfx::SamplerDesc m_sampler;
			float32 m_left_offset;
			float32 m_top_offset;
			float32 m_rotation;
			RectI m_clip_rect;

			P<IDrawPath> m_path;
			P<IFontTexture> m_font;

			// If `true`, then the draw call should be re-targeted.
			bool m_state_dirty;

			// Creates one new draw call using the current pipeline state.
			void new_draw_call();
			// Selects one draw call that can be used to record the new draw elements.
			DrawCallBuf* get_target_drawcall();

			// Tests if the specified state is equal with the current set state.
			bool state_equal(uint32 index);

			DrawList(IAllocator* alloc) :
				luibind(alloc),
				m_bufs(alloc) {}

			void init();

			virtual void drawcall_barrier() override;

			virtual void reset() override;
			virtual void set_texture(gfx::IResource* tex) override
			{
				if (tex != m_texture)
				{
					m_state_dirty = true;
					m_texture = tex;
				}
			}
			virtual gfx::IResource* get_texture() override
			{
				if (m_texture)
				{
					m_texture->add_ref();
				}
				return m_texture;
			}
			virtual void set_sampler(const gfx::SamplerDesc* desc) override
			{
				gfx::SamplerDesc d;
				if (!desc)
				{
					float32 c2[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
					d = gfx::SamplerDesc(gfx::EFilter::min_mag_mip_linear, gfx::ETextureAddressMode::clamp, gfx::ETextureAddressMode::clamp,
						gfx::ETextureAddressMode::clamp, 0.0f, 1, gfx::EComparisonFunc::always, c2, 0.0f, 1.0f);
				}
				else
				{
					d = *desc;
				}
				if (d != m_sampler)
				{
					m_sampler = d;
					m_state_dirty = true;
				}
			}
			virtual void get_sampler(gfx::SamplerDesc* desc) override
			{
				*desc = m_sampler;
			}
			virtual IFontTexture* get_font() override
			{
				return m_font;
			}
			virtual void set_font(IFontTexture* font_texture) override
			{
				m_font = font_texture;
			}
			virtual void set_origin(const Float2& offset) override
			{
				if (m_left_offset != offset.x || m_top_offset != offset.y)
				{
					m_state_dirty = true;
					m_left_offset = offset.x;
					m_top_offset = offset.y;
				}
			}
			virtual void get_origin(Float2& offset) override
			{
				offset.x = m_left_offset;
				offset.y = m_top_offset;
			}
			virtual void set_rotation(float32 degrees) override
			{
				if (m_rotation != degrees)
				{
					m_state_dirty = true;
					m_rotation = degrees;
				}
			}
			virtual float32 get_rotation() override
			{
				return m_rotation;
			}
			virtual void set_clip_rect(const RectI& clip_rect) override
			{
				if (m_clip_rect != clip_rect)
				{
					m_state_dirty = true;
					m_clip_rect = clip_rect;
				}
			}
			virtual void get_clip_rect(RectI& clip_rect) override
			{
				clip_rect = m_clip_rect;
			}
			virtual void append_draw_list(IDrawList* draw_list, EDrawListAppendFlag flags = EDrawListAppendFlag::none) override;
			virtual void draw_triangle_list(uint32 num_vertices, const PrimitiveVertex* vertices,
				uint32 num_indices, const uint32* indices) override;
			virtual R<TextDrawResult> draw_text(const char* text, const Float2& top_left, const Float2& region_size,
				const Color& col = color::white, const Float2& spacing = Float2(0.0f, 0.0f)) override;
			virtual void draw_triangle_filled(const Float2& p1, const Float2& p2, const Float2& p3, const Color& col, bool antialiased = true) override;
			virtual void draw_triangle_bordered(const Float2& p1, const Float2& p2, const Float2& p3, const Color& col, float32 line_width, bool antialiased = true) override;
			virtual void draw_rectangle_filled(const Float2& top_left, const Float2& size, const Color& col) override;
			virtual void draw_rectangle_bordered(const Float2& top_left, const Float2& size, const Color& col, float32 line_width) override;
			virtual void draw_circle_filled(const Float2& center, float32 radius, const Color& col, bool antialiased, uint32 num_segments) override;
			virtual void draw_circle_bordered(const Float2& center, float32 radius, const Color& col, float32 line_width, bool antialiased, uint32 num_segments) override;
			virtual void draw_rounded_rectangle_filled(const Float2& top_left, const Float2& size, const Color& col, float32 round_radius, bool antialiased) override;
			virtual void draw_rounded_rectangle_bordered(const Float2& top_left, const Float2& size, const Color& col, float32 round_radius, float32 line_width, bool antialiased) override;
			virtual void draw_line(const Float2& pos1, const Float2& pos2, const Color& col, float32 line_width, bool antialiased) override;
			virtual void draw_texture(const Float2& top_left, const Float2& size, const Color& tint, gfx::IResource* tex, gfx::EResourceState state, const Float2& top_left_uv, const Float2& bottom_right_uv) override;

			virtual uint32 count_draw_calls() override
			{
				return (uint32)m_bufs.size();
			}

			virtual void enum_draw_call(uint32 index, DrawCall& dc) override
			{
				auto& buf = m_bufs[index];
				dc.texture = buf.m_texture;
				dc.vertices = buf.m_vertices.data();
				dc.indices = buf.m_indices.data();
				dc.num_vertices = (uint32)buf.m_vertices.size();
				dc.num_indices = (uint32)buf.m_indices.size();
				dc.sampler = buf.sampler;
				dc.rotation = buf.rotation;
				dc.origin_x = buf.origin_x;
				dc.origin_y = buf.origin_y;
				dc.clip_rect = buf.clip_rect;
			}
		};
	}
}