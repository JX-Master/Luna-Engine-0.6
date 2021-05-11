// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file MathBase.hpp
* @author JXMaster
* @date 2018/10/26
* @brief Base math operation defines.
 */
#pragma once
#include "../../Base.hpp"

namespace Luna
{
	constexpr f32 pi = 3.141592654f;
	constexpr f32 two_pi = 6.283185307f;
	constexpr f32 one_div_pi = 0.318309886f;
	constexpr f32 one_div_two_pi = 0.159154943f;
	constexpr f32 pi_div_two = 1.570796327f;
	constexpr f32 pi_div_four = 0.785398163f;

	//! Convert degree angle to radius angle.
	inline constexpr f32 deg_to_rad(f32 degrees)
	{
		return degrees * (pi / 180.0f);
	}
	//! Convert radius angle to degree angle.
	inline constexpr f32 rad_to_deg(f32 radians)
	{
		return radians * (180.0f / pi);
	}

	//! Check whether the provided number is power of two.
	template <typename _Ty>
	constexpr bool is_pow_of_two(_Ty n)
	{
		return (((n)&((n)-1)) == 0 && (n) != 0);
	}

	//! Rect is used to specify a region in the bitmap, texture or window.
	template <typename _Ty>
	struct Rect
	{
		_Ty left;
		_Ty top;
		_Ty right;
		_Ty bottom;

		Rect() = default;
		Rect(_Ty _left, _Ty _top, _Ty _right, _Ty _bottom) :
			left(_left),
			top(_top),
			right(_right),
			bottom(_bottom) {}

		bool operator==(const Rect& rhs)
		{
			return ((left == rhs.left) &&
				(right == rhs.right) &&
				(top == rhs.top) &&
				(bottom == rhs.bottom));
		}
		bool operator!= (const Rect& rhs)
		{
			return !(*this == rhs);
		}
	};

	using RectI = Rect<i32>;
	using RectU = Rect<u32>;
	using RectF = Rect<f32>;

	template <typename _Ty>
	struct Box
	{
		_Ty left;
		_Ty top;
		_Ty front;
		_Ty right;
		_Ty bottom;
		_Ty back;

		Box() = default;
		Box(_Ty _left, _Ty _top, _Ty _front, _Ty _right, _Ty _bottom, _Ty _back) :
			left(_left),
			top(_top),
			front(_front),
			right(_right),
			bottom(_bottom),
			back(_back) {}
	};

	using BoxU = Box<u32>;

	inline f32 lerp(f32 f1, f32 f2, f32 t)
	{
		return f1 + t * (f2 - f1);
	}

	inline f32 smoothstep(f32 f1, f32 f2, f32 t)
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		return lerp(f1, f2, t);
	}

	inline f32 clamp(f32 v, f32 min_v, f32 max_v)
	{
		v = v > min_v ? v : min_v;
		v = v < max_v ? v : max_v;
		return v;
	}
}