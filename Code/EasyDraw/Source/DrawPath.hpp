// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DrawPath.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include "EasyDrawHeader.hpp"
#include <Runtime/Vector.hpp>
#include "EasyDraw.hpp"
#include <Core/Interface.hpp>
namespace Luna
{
	namespace EasyDraw
	{
		class DrawPath : public IDrawPath
		{
		public:
			lucid("{6067ffa1-c980-469f-996c-fdafdc09b5f8}");
			luiimpl(DrawPath, IDrawPath, IObject);

			Vector<PrimitiveVertex> m_vertices;
			Vector<u32> m_indices;
			Vector<PrimitiveVertex> m_points;

			Float2 m_uv;
			Float4 m_color;
			bool m_recording;

			DrawPath() :
				m_recording(false) {}

			virtual void begin(const Float2& initial_pos, const Float4& initial_color = Float4(1.0f, 1.0f, 1.0f, 1.0f), const Float2& initial_uv = Float2(1.0f, 1.0f)) override;
			virtual void set_texcoord(const Float2& uv) override
			{
				m_uv = uv;
			}
			virtual void set_color(const Float4& c) override
			{
				m_color = c;
			}
			virtual void line_to(const Float2& dest) override;
			virtual void arc_to(const Float2& center, f32 radius, f32 a_min, f32 a_max, bool skip_min = true, bool skip_max = false, u32 num_segments = 10) override;
			virtual void bezier_to(const Float2& dest, const Float2& cp1, const Float2& cp2, u32 num_segments = 10) override;
			virtual void end_stroke(f32 width, bool closed = false, bool antialiased = false) override;
			virtual void end_filled_convex(bool clockwise = true, bool antialiased = false) override;
			virtual void submit_triangle_list(IDrawList* draw_list) override;
		};
	}
}