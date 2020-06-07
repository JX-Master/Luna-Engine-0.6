// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file DrawPath.cpp
* @author JXMaster
* @date 2019/10/5
*/
#include "DrawPath.hpp"

namespace luna
{
	namespace edraw
	{
		void DrawPath::begin(const Float2& initial_pos, const Color& initial_color, const Float2& initial_uv)
		{
			m_uv = initial_uv;
			m_color = initial_color;
			m_vertices.clear();
			m_indices.clear();
			m_points.clear();
			PrimitiveVertex v;
			v.pos = initial_pos;
			v.color = initial_color;
			v.uv = initial_uv;
			m_points.push_back(v);
			m_recording = true;
		}
		void DrawPath::line_to(const Float2& dest)
		{
			if (m_recording)
			{
				Float2U& pre = m_points.back().pos;
				if (pre.x != dest.x || pre.y != dest.y)
				{
					PrimitiveVertex v;
					v.pos = dest;
					v.color = m_color;
					v.uv = m_uv;
					m_points.push_back(v);
				}
			}
		}
		void DrawPath::arc_to(const Float2& center, float32 radius, float32 a_min, float32 a_max, bool skip_min, bool skip_max, uint32 num_segments)
		{
			size_t points_to_draw = num_segments + 1;
			if (skip_min)
			{
				--points_to_draw;
			}
			if (skip_max)
			{
				--points_to_draw;
			}
			m_points.reserve(m_points.size() + points_to_draw);
			for (uint32 i = 0; i <= num_segments; i++)
			{
				if ((!i && skip_min) || (i == num_segments && skip_max))
				{
					continue;
				}
				const float32 a = deg_to_rad(a_min + ((float32)i / (float32)num_segments) * (a_max - a_min));
				line_to(Float2(center.x + cosf(a) * radius, center.y + sinf(a) * radius));
			}
		}
		void DrawPath::bezier_to(const Float2& dest, const Float2& cp1, const Float2& cp2, uint32 num_segments)
		{
			float32 t_step = 1.0f / (float32)num_segments;
			Float2 src = m_points.back().pos;
			for (uint32 i_step = 1; i_step <= num_segments; i_step++)
			{
				float32 t = t_step * i_step;
				float32 u = 1.0f - t;
				float32 w1 = u * u * u;
				float32 w2 = 3 * u * u * t;
				float32 w3 = 3 * u * t * t;
				float32 w4 = t * t * t;
				line_to(src * w1 + cp1 * w2 + cp2 * w3 + dest * w4);
			}
		}
		void DrawPath::end_stroke(float32 width, bool closed, bool antialiased)
		{
			if (!m_recording)
			{
				return;
			}
			// generate vertices.
			if (m_points.size() < 2)
			{
				return;
			}
			float32 half_width = width / 2.0f;
			bool thick_line = width >= 1.0f ? true : false;
			if (antialiased)
			{
				if (thick_line)
				{
					m_vertices.reserve(m_points.size() * 4);
					if (closed)
					{
						m_indices.reserve(m_points.size() * 18);
					}
					else
					{
						m_indices.reserve(m_points.size() * 18 - 18);
					}
				}
				else
				{
					m_vertices.reserve(m_points.size() * 3);
					if (closed)
					{
						m_indices.reserve(m_points.size() * 12);
					}
					else
					{
						m_indices.reserve(m_points.size() * 12 - 12);
					}
				}
			}
			else
			{
				m_vertices.reserve(m_points.size() * 2);
				if (closed)
				{
					m_indices.reserve(m_points.size() * 6);
				}
				else
				{
					m_indices.reserve(m_points.size() * 6 - 6);
				}
			}
			for (size_t i = 0; i < m_points.size(); ++i)
			{
				PrimitiveVertex v;
				v.color = m_points[i].color;
				v.uv = m_points[i].uv;
				Float2 dir;
				float32 mulp = 1.0f;
				if (i == 0)
				{
					if (closed)
					{
						Float2 dir_1 = normalize(m_points[i].pos - m_points.back().pos);
						Float2 dir_2 = normalize(m_points[i + 1].pos - m_points[i].pos);
						dir = normalize(dir_1 + dir_2);
						mulp = 1.0f / sqrtf((dot(dir_1, dir_2) + 1) / 2.0f);
					}
					else
					{
						dir = normalize(m_points[i + 1].pos - m_points[i].pos);
					}
				}
				else if (i == m_points.size() - 1)
				{
					if (closed)
					{
						Float2 dir_1 = normalize(m_points[i].pos - m_points[i - 1].pos);
						Float2 dir_2 = normalize(m_points.front().pos - m_points[i].pos);
						dir = normalize(dir_1 + dir_2);
						mulp = 1.0f / sqrtf((dot(dir_1, dir_2) + 1) / 2.0f);
					}
					else
					{
						dir = normalize(m_points[i].pos - m_points[i - 1].pos);
					}
				}
				else
				{
					Float2 dir_1 = normalize(m_points[i].pos - m_points[i - 1].pos);
					Float2 dir_2 = normalize(m_points[i + 1].pos - m_points[i].pos);
					dir = normalize(dir_1 + dir_2);
					mulp = 1.0f / sqrtf((dot(dir_1, dir_2) + 1) / 2.0f);
				}
				if (antialiased)
				{
					if (thick_line)
					{
						v.pos = m_points[i].pos + Float2(-(dir.y), dir.x) * (half_width - 0.5f) * mulp;
						m_vertices.push_back(v);
						v.pos = m_points[i].pos + Float2(dir.y, -(dir.x)) * (half_width - 0.5f) * mulp;
						m_vertices.push_back(v);
						v.color.a = 0.0f;
						v.pos = m_points[i].pos + Float2(-(dir.y), dir.x) * (half_width + 0.5f) * mulp;
						m_vertices.push_back(v);
						v.pos = m_points[i].pos + Float2(dir.y, -(dir.x)) * (half_width + 0.5f) * mulp;
						m_vertices.push_back(v);
					}
					else
					{
						v.pos = m_points[i].pos;
						m_vertices.push_back(v);
						v.color.a = 0.0f;
						v.pos = m_points[i].pos + Float2(-(dir.y), dir.x) * (half_width + 0.5f) * mulp;
						m_vertices.push_back(v);
						v.pos = m_points[i].pos + Float2(dir.y, -(dir.x)) * (half_width + 0.5f) * mulp;
						m_vertices.push_back(v);
					}
				}
				else
				{
					v.pos = m_points[i].pos + Float2(-(dir.y), dir.x) * half_width * mulp;
					m_vertices.push_back(v);
					v.pos = m_points[i].pos + Float2(dir.y, -(dir.x)) * half_width * mulp;
					m_vertices.push_back(v);
				}
			}
			// generate indices.
			if (antialiased)
			{
				if (thick_line)
				{
					for (uint32 i = 0; i < (uint32)m_points.size() - 1; ++i)
					{
						uint32 indices[18] = {
							i * 4,
							i * 4 + 1,
							i * 4 + 4,
							i * 4 + 1,
							i * 4 + 5,
							i * 4 + 4,

							i * 4 + 2,
							i * 4,
							i * 4 + 6,
							i * 4,
							i * 4 + 4,
							i * 4 + 6,

							i * 4 + 1,
							i * 4 + 3,
							i * 4 + 5,
							i * 4 + 3,
							i * 4 + 7,
							i * 4 + 5
						};

						m_indices.insert_n(m_indices.end(), indices, 18);
					}
					if (closed)
					{
						uint32 i = (uint32)m_points.size() - 1;
						uint32 indices[18] = {
							i * 4,
							i * 4 + 1,
							0,
							i * 4 + 1,
							1,
							0,

							i * 4 + 2,
							i * 4,
							2,
							i * 4,
							0,
							2,

							i * 4 + 1,
							i * 4 + 3,
							1,
							i * 4 + 3,
							3,
							1
						};
						m_indices.insert_n(m_indices.end(), indices, 18);
					}
				}
				else
				{
					for (uint32 i = 0; i < (uint32)m_points.size() - 1; ++i)
					{
						uint32 indices[12] = {
							i * 3 + 1,
							i * 3,
							i * 3 + 4,
							i * 3,
							i * 3 + 3,
							i * 3 + 4,

							i * 3,
							i * 3 + 2,
							i * 3 + 3,
							i * 3 + 2,
							i * 3 + 5,
							i * 3 + 3
						};
						m_indices.insert_n(m_indices.end(), indices, 12);
					}
					if (closed)
					{
						uint32 i = (uint32)m_points.size() - 1;
						uint32 indices[12] = {
							i * 3 + 1,
							i * 3,
							1,
							i * 3,
							0,
							1,

							i * 3,
							i * 3 + 2,
							0,
							i * 3 + 2,
							2,
							0
						};
						m_indices.insert_n(m_indices.end(), indices, 12);
					}
				}
			}
			else
			{
				for (uint32 i = 0; i < (uint32)m_points.size() - 1; ++i)
				{
					uint32 indices[6] =
					{
						i * 2,
						i * 2 + 1,
						i * 2 + 2,
						i * 2 + 1,
						i * 2 + 3,
						i * 2 + 2,
					};
					m_indices.insert_n(m_indices.end(), indices, 6);
				}
				if (closed)
				{
					uint32 i = (uint32)m_points.size() - 1;
					uint32 indices[6] =
					{
						i * 2,
						i * 2 + 1,
						0,
						i * 2 + 1,
						1,
						0
					};
					m_indices.insert_n(m_indices.end(), indices, 6);
				}
			}
			m_recording = false;
			m_points.clear();
		}
		void DrawPath::end_filled_convex(bool clockwise, bool antialiasing)
		{
			if (!m_recording)
			{
				return;
			}
			// generate vertices.
			if (m_points.size() < 3)
			{
				return;
			}
			if (antialiasing)
			{
				m_vertices.reserve(m_points.size() * 2);
				m_indices.reserve(m_points.size() * 9 - 6);
				// If we choose antialiased shape, we need to calculate normals as well.
				for (size_t i = 0; i < m_points.size(); ++i)
				{
					PrimitiveVertex v;
					v.color = m_points[i].color;
					v.uv = m_points[i].uv;
					Float2 dir;
					float32 mulp = 1.0f;
					if (i == 0)
					{
						Float2 dir_1 = normalize(m_points[i].pos - m_points.back().pos);
						Float2 dir_2 = normalize(m_points[i + 1].pos - m_points[i].pos);
						dir = normalize(dir_1 + dir_2);
						mulp = 1.0f / sqrtf((dot(dir_1, dir_2) + 1) / 2.0f);
					}
					else if (i == m_points.size() - 1)
					{
						Float2 dir_1 = normalize(m_points[i].pos - m_points[i - 1].pos);
						Float2 dir_2 = normalize(m_points.front().pos - m_points[i].pos);
						dir = normalize(dir_1 + dir_2);
						mulp = 1.0f / sqrtf((dot(dir_1, dir_2) + 1) / 2.0f);
					}
					else
					{
						Float2 dir_1 = normalize(m_points[i].pos - m_points[i - 1].pos);
						Float2 dir_2 = normalize(m_points[i + 1].pos - m_points[i].pos);
						dir = normalize(dir_1 + dir_2);
						mulp = 1.0f / sqrtf((dot(dir_1, dir_2) + 1) / 2.0f);
					}
					v.pos = m_points[i].pos;
					m_vertices.push_back(v);
					v.color.a = 0.0f;
					if (clockwise)
					{
						v.pos = m_points[i].pos + Float2(-(dir.y), dir.x) * 0.5f * mulp;
					}
					else
					{
						v.pos = m_points[i].pos + Float2(dir.y, -(dir.x)) * 0.5f * mulp;
					}
					m_vertices.push_back(v);
				}
				// generate internal indices.
				for (uint32 i = 0; i < (uint32)m_points.size() - 2; ++i)
				{
					if (clockwise)
					{
						uint32 indices[3] = {
							0,
							(i + 2) * 2,
							(i + 1) * 2
						};
						m_indices.insert_n(m_indices.end(), indices, 3);
					}
					else
					{
						uint32 indices[3] = {
							0,
							(i + 1) * 2,
							(i + 2) * 2
						};
						m_indices.insert_n(m_indices.end(), indices, 3);
					}
				}
				// generate outlines.
				for (uint32 i = 0; i < (uint32)m_points.size() - 1; ++i)
				{
					uint32 indices[6] = {
						i * 2,
						i * 2 + 3,
						i * 2 + 1,
						i * 2,
						i * 2 + 2,
						i * 2 + 3,
					};
					m_indices.insert_n(m_indices.end(), indices, 6);
				}
				uint32 i = (uint32)m_points.size() - 1;
				uint32 indices[6] = {
					i * 2,
					1,
					i * 2 + 1,
					i * 2,
					0,
					1,
				};
				m_indices.insert_n(m_indices.end(), indices, 6);
			}
			else
			{
				m_vertices = move(m_points);
				m_indices.reserve(m_vertices.size() * 3 - 6);
				for (uint32 i = 0; i < (uint32)m_vertices.size() - 2; ++i)
				{
					if (clockwise)
					{
						uint32 indices[3] = {
							0,
							i + 2,
							i + 1
						};
						m_indices.insert_n(m_indices.end(), indices, 3);
					}
					else
					{
						uint32 indices[3] = {
							0,
							i + 1,
							i + 2
						};
						m_indices.insert_n(m_indices.end(), indices, 3);
					}
				}
			}
			m_recording = false;
		}
		void DrawPath::submit_triangle_list(IDrawList* draw_list)
		{
			luassert_usr(!m_recording);
			draw_list->draw_triangle_list((uint32)m_vertices.size(), m_vertices.data(), (uint32)m_indices.size(), m_indices.data());
		}
	}
}