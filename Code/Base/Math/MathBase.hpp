// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MathBase.hpp
* @author JXMaster
* @date 2018/10/26
* @brief Base math operation defines.
 */
#pragma once
#include "../BaseDefines.hpp"

namespace luna
{
	constexpr float32 pi = 3.141592654f;
	constexpr float32 two_pi = 6.283185307f;
	constexpr float32 one_div_pi = 0.318309886f;
	constexpr float32 one_div_two_pi = 0.159154943f;
	constexpr float32 pi_div_two = 1.570796327f;
	constexpr float32 pi_div_four = 0.785398163f;

	//! Convert degree angle to radius angle.
	inline constexpr float32 deg_to_rad(float32 degrees)
	{
		return degrees * (pi / 180.0f);
	}
	//! Convert radius angle to degree angle.
	inline constexpr float32 rad_to_deg(float32 radians)
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

	using RectI = Rect<int32>;
	using RectU = Rect<uint32>;
	using RectF = Rect<float32>;

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

	using BoxU = Box<uint32>;

	inline float32 lerp(float32 f1, float32 f2, float32 t)
	{
		return f1 + t * (f2 - f1);
	}

	inline float32 smoothstep(float32 f1, float32 f2, float32 t)
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		return lerp(f1, f2, t);
	}

	inline float32 clamp(float32 v, float32 min_v, float32 max_v)
	{
		v = v > min_v ? v : min_v;
		v = v < max_v ? v : max_v;
		return v;
	}
}