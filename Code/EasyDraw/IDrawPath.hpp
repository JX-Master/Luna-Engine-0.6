// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IDrawPath.hpp
* @author JXMaster
* @date 2019/10/5
*/
#pragma once
#include <Base/IObject.hpp>
#include <Base/Math.hpp>
#include "IDrawList.hpp"

namespace luna
{
	namespace edraw
	{
		//! @interface IDrawPath
		//! A helper object used to generate triangle lists from line strip path.
		struct IDrawPath : public IObject
		{
			luiid("{60edeb75-1130-4c88-8530-b0347e5f3613}");

			//! Clears the previous path and begins a new path.
			virtual void begin(const Float2& initial_pos, const Color& initial_color = Color(1.0f, 1.0f, 1.0f, 1.0f), const Float2& initial_uv = Float2(0.0f, 0.0f)) = 0;

			//! Sets the texture coordinate for next point.
			virtual void set_texcoord(const Float2& uv) = 0;

			//! Sets the color for next point.
			virtual void set_color(const Color& c) = 0;

			//! Draws a line between this point and the last point.
			virtual void line_to(const Float2& dest) = 0;

			//! Draws an art between this point and the last point.
			//! The a_min and a_max starts from (center.x, + radius, center.y) and rotates clockwise (assumes the y axis points down), the angle is measured in degrees.
			virtual void arc_to(const Float2& center, float32 radius, float32 a_min, float32 a_max, bool skip_min = true, bool skip_max = false, uint32 num_segments = 10) = 0;

			//! Draws an Bezier curve between this point and the last point.
			virtual void bezier_to(const Float2& dest, const Float2& cp1, const Float2& cp2, uint32 num_segments = 10) = 0;

			//! Close the path and generate triangle lists as a stroke.
			virtual void end_stroke(float32 width, bool closed = false, bool antialiased = false) = 0;

			//! Close the path and generate triangle lists as a convex filled shape.
			virtual void end_filled_convex(bool clockwise = true, bool antialiased = false) = 0;

			//! Submit the generated triangle list to the specified draw list.
			virtual void submit_triangle_list(IDrawList* draw_list) = 0;
		};
	}
}