// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Vector.hpp
* @author JXMaster
* @date 2019/1/5
 */
#pragma once

#include "SimdVector.hpp"

#ifndef LUNA_SIMD_ENABLED
#include <math.h>
#endif

#ifdef LUNA_PLATFORM_WINDOWS
#define NOMINMAX
#endif

namespace Luna
{
	struct Float2;
	struct Float3;
	struct Float4;

	struct alignas(16) Float2
	{
		union
		{
			struct
			{
				f32 x;
				f32 y;
			};
			struct
			{
				f32 r;
				f32 g;
			};
			f32 m[2];
		};

		Float2() = default;
		Float2(const Float2&) = default;
		Float2& operator=(const Float2&) = default;
		Float2(Float2&&) = default;
		Float2& operator=(Float2&&) = default;

		constexpr explicit Float2(f32 _x) :
			x(_x), y(_x) {}
		constexpr Float2(f32 _x, f32 _y) :
			x(_x), y(_y) {}
		explicit Float2(const f32* pArray)
		{
			x = pArray[0];
			y = pArray[1];
		}
#ifdef LUNA_SIMD_ENABLEDW
		Float2(Simd::vec_float_p1_t v)
		{
			Simd::store_float2a(m, v);
		}
		operator Simd::vec_float_t() const { return Simd::load_float2a(m); }
#endif
		bool operator==(const Float2& v) const;
		bool operator!=(const Float2& v) const;
		Float2& operator+= (const Float2& v);
		Float2& operator-= (const Float2& v);
		Float2& operator*= (const Float2& v);
		Float2& operator/= (const Float2& v);
		Float2& operator+= (f32 s);
		Float2& operator-= (f32 s);
		Float2& operator*= (f32 s);
		Float2& operator/= (f32 s);

		Float2 operator+ () const { return *this; }
		Float2 operator- () const { return Float2(-x, -y); }

		static constexpr Float2 zero() { return Float2(0.0f, 0.0f); }
		static constexpr Float2 one() { return Float2(1.0f, 1.0f); }
		static constexpr Float2 unit_x() { return Float2(1.0f, 0.0f); }
		static constexpr Float2 unit_y() { return Float2(0.0f, 1.0f); }
	};

	static_assert(sizeof(Float2) == sizeof(f32) * 4, "Incorrect Float2 size.");

	Float2 operator+ (const Float2& v1, const Float2& v2);
	Float2 operator- (const Float2& v1, const Float2& v2);
	Float2 operator* (const Float2& v1, const Float2& v2);
	Float2 operator/ (const Float2& v1, const Float2& v2);
	Float2 operator+ (const Float2& v, f32 s);
	Float2 operator+ (f32 s, const Float2& v);
	Float2 operator- (const Float2& v, f32 s);
	Float2 operator- (f32 s, const Float2& v1);
	Float2 operator* (const Float2& v, f32 s);
	Float2 operator* (f32 s, const Float2& v);
	Float2 operator/ (const Float2& v, f32 s);
	Float2 operator/ (f32 s, const Float2& v);

	bool in_bounds(const Float2& vec, const Float2& bounds);
	f32 length(const Float2& vec);
	f32 length_squared(const Float2& vec);
	f32 dot(const Float2& vec1, const Float2& vec2);
	Float2 cross(const Float2& vec1, const Float2& vec2);
	Float2 normalize(const Float2& vec);
	Float2 clamp(const Float2& vec, const Float2& vec_min, const Float2& vec_max);
	f32 distance(const Float2& vec1, const Float2& vec2);
	f32 distance_squared(const Float2& vec1, const Float2& vec2);
	Float2 min(const Float2& vec1, const Float2& vec2);
	Float2 max(const Float2& vec1, const Float2& vec2);
	Float2 lerp(const Float2& vec1, const Float2& vec2, f32 t);
	Float2 smoothstep(const Float2& vec1, const Float2& vec2, f32 t);
	Float2 barycentric(const Float2& vec1, const Float2& vec2, const Float2& vec3, f32 f, f32 g);
	Float2 catmull_rom(const Float2& vec1, const Float2& vec2, const Float2& vec3, const Float2& vec4, f32 t);
	Float2 hermite(const Float2& vec1, const Float2& t1, const Float2& vec2, const Float2& t2, f32 t);
	Float2 reflect(const Float2& ivec, const Float2& nvec);
	Float2 refract(const Float2& ivec, const Float2& nvec, f32 refractionIndex);
	bool in_rect(const Float2& point, const Float2& min_point, const Float2& max_point);

	template <typename _Ty>
	struct Vec2U
	{
		union
		{
			struct
			{
				_Ty x;
				_Ty y;
			};
			struct
			{
				_Ty r;
				_Ty g;
			};
			_Ty m[2];
		};

		Vec2U() = default;
		Vec2U(const Vec2U&) = default;
		Vec2U& operator=(const Vec2U&) = default;
		Vec2U(Vec2U&&) = default;
		Vec2U& operator=(Vec2U&&) = default;

		Vec2U(const Float2& rhs) :
			x((_Ty)rhs.x), y((_Ty)rhs.y) {}
		Vec2U& operator=(const Float2& rhs)
		{
			x = (_Ty)rhs.x;
			y = (_Ty)rhs.y;
			return *this;
		}
		operator Float2() const
		{
			return Float2((f32)x, (f32)y);
		}

		constexpr explicit Vec2U(_Ty _x) :
			x(_x), y(_x) {}
		constexpr Vec2U(_Ty _x, _Ty _y) :
			x(_x), y(_y) {}
		explicit Vec2U(const _Ty* pArray)
		{
			x = pArray[0];
			y = pArray[1];
		}

		bool operator==(const Vec2U& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y);
		}
		bool operator!=(const Vec2U& rhs) const
		{
			return !(*this == rhs);
		}
	};

	using Float2U = Vec2U<f32>;
	using Int2U = Vec2U<i32>;
	using UInt2U = Vec2U<u32>;

	struct alignas(16) Float3
	{
		union
		{
			struct
			{
				f32 x;
				f32 y;
				f32 z;
			};
			struct
			{
				f32 r;
				f32 g;
				f32 b;
			};
			f32 m[3];
		};

		Float3() = default;
		Float3(const Float3&) = default;
		Float3& operator=(const Float3&) = default;
		Float3(Float3&&) = default;
		Float3& operator=(Float3&&) = default;

		constexpr explicit Float3(f32 _x) :
			x(_x), y(_x), z(_x) {}
		constexpr Float3(f32 _x, f32 _y, f32 _z) :
			x(_x), y(_y), z(_z) {}
		explicit Float3(const f32* pArray)
		{
			x = pArray[0];
			y = pArray[1];
			z = pArray[2];
		}
#ifdef LUNA_SIMD_ENABLED	
		Float3(Simd::vec_float_p1_t v)
		{
			Simd::store_float3a(m, v);
		}
		operator Simd::vec_float_t() const { return Simd::load_float3a(m); }
#endif
		bool operator==(const Float3& v) const;
		bool operator!=(const Float3& v) const;
		Float3& operator+= (const Float3& v);
		Float3& operator-= (const Float3& v);
		Float3& operator*= (const Float3& v);
		Float3& operator/= (const Float3& v);
		Float3& operator+= (f32 s);
		Float3& operator-= (f32 s);
		Float3& operator*= (f32 s);
		Float3& operator/= (f32 s);

		Float3 operator+ () const { return *this; }
		Float3 operator- () const;

		// Constants
		static constexpr Float3 zero() { return Float3(0.0f, 0.0f, 0.0f); }
		static constexpr Float3 one() { return Float3(1.0f, 1.0f, 1.0f); }
		static constexpr Float3 unit_x() { return Float3(1.0f, 0.0f, 0.0f); }
		static constexpr Float3 unit_y() { return Float3(0.0f, 1.0f, 0.0f); }
		static constexpr Float3 unit_z() { return Float3(0.0f, 0.0f, 1.0f); }
		static constexpr Float3 up() { return Float3(0.0f, 1.0f, 0.0f); }
		static constexpr Float3 down() { return Float3(0.0f, -1.0f, 0.0f); }
		static constexpr Float3 right() { return Float3(1.0f, 0.0f, 0.0f); }
		static constexpr Float3 left() { return Float3(-1.0f, 0.0f, 0.0f); }
		static constexpr Float3 forward() { return Float3(0.0f, 0.0f, -1.0f); }
		static constexpr Float3 backward() { return Float3(0.0f, 0.0f, 1.0f); }
	};

	static_assert(sizeof(Float3) == sizeof(f32) * 4, "Incorrect Float2 size.");

	Float3 operator+ (const Float3& v1, const Float3& v2);
	Float3 operator- (const Float3& v1, const Float3& v2);
	Float3 operator* (const Float3& v1, const Float3& v2);
	Float3 operator/ (const Float3& v1, const Float3& v2);
	Float3 operator+ (const Float3& v, f32 s);
	Float3 operator+ (f32 s, const Float3& v);
	Float3 operator- (const Float3& v, f32 s);
	Float3 operator- (f32 s, const Float3& v);
	Float3 operator* (const Float3& v, f32 s);
	Float3 operator* (f32 s, const Float3& v);
	Float3 operator/ (const Float3& v, f32 s);
	Float3 operator/ (f32 s, const Float3& v);

	bool in_bounds(const Float3& vec, const Float3& bounds);
	f32 length(const Float3& vec);
	f32 length_squared(const Float3& vec);
	f32 dot(const Float3& vec1, const Float3& vec2);
	Float3 cross(const Float3& vec1, const Float3& vec2);
	Float3 normalize(const Float3& vec);
	Float3 clamp(const Float3& vec, const Float3& vec_min, const Float3& vec_max);
	f32 distance(const Float3& vec1, const Float3& vec2);
	f32 distance_squared(const Float3& vec1, const Float3& vec2);
	Float3 min(const Float3& vec1, const Float3& vec2);
	Float3 max(const Float3& vec1, const Float3& vec2);
	Float3 lerp(const Float3& vec1, const Float3& vec2, f32 t);
	Float3 smoothstep(const Float3& vec1, const Float3& vec2, f32 t);
	Float3 barycentric(const Float3& vec1, const Float3& vec2, const Float3& vec3, f32 f, f32 g);
	Float3 catmull_rom(const Float3& vec1, const Float3& vec2, const Float3& vec3, const Float3& vec4, f32 t);
	Float3 hermite(const Float3& vec1, const Float3& t1, const Float3& vec2, const Float3& t2, f32 t);
	Float3 reflect(const Float3& ivec, const Float3& nvec);
	Float3 refract(const Float3& ivec, const Float3& nvec, f32 refractionIndex);

	//! Unaligned version of Float3. This is used for storing or transferring data between CPU/GPU and network only. 
	template <typename _Ty>
	struct Vec3U
	{
		union
		{
			struct
			{
				_Ty x;
				_Ty y;
				_Ty z;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
			};
			_Ty m[3];
		};

		Vec3U() = default;
		Vec3U(const Vec3U&) = default;
		Vec3U& operator=(const Vec3U&) = default;
		Vec3U(Vec3U&&) = default;
		Vec3U& operator=(Vec3U&&) = default;

		Vec3U(const Float3& rhs) :
			x((_Ty)rhs.x), y((_Ty)rhs.y), z((_Ty)rhs.z) {}
		Vec3U& operator=(const Float3& rhs)
		{
			x = (_Ty)rhs.x;
			y = (_Ty)rhs.y;
			z = (_Ty)rhs.z;
			return *this;
		}
		operator Float3() const
		{
			return Float3((f32)x, (f32)y, (f32)z);
		}

		constexpr explicit Vec3U(_Ty _x) :
			x(_x), y(_x), z(_x) {}
		constexpr Vec3U(_Ty _x, _Ty _y, _Ty _z) :
			x(_x), y(_y), z(_z) {}
		explicit Vec3U(const _Ty* pArray)
		{
			x = pArray[0];
			y = pArray[1];
			z = pArray[2];
		}

		bool operator==(const Vec3U& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
		}
		bool operator!=(const Vec3U& rhs) const
		{
			return !(*this == rhs);
		}
	};

	using Float3U = Vec3U<f32>;
	using Int3U = Vec3U<i32>;
	using UInt3U = Vec3U<u32>;

	struct alignas(16) Float4
	{
		union
		{
			struct
			{
				f32 x;
				f32 y;
				f32 z;
				f32 w;
			};
			struct
			{
				f32 r;
				f32 g;
				f32 b;
				f32 a;
			};
			f32 m[4];
		};

		Float4() = default;
		Float4(const Float4&) = default;
		Float4& operator=(const Float4&) = default;
		Float4(Float4&&) = default;
		Float4& operator=(Float4&&) = default;

		constexpr explicit Float4(f32 _x) :
			x(_x), y(_x), z(_x), w(_x) {}
		constexpr Float4(f32 _x, f32 _y, f32 _z, f32 _w) :
			x(_x), y(_y), z(_z), w(_w) {}
		explicit Float4(const f32* pArray)
		{
			x = pArray[0];
			y = pArray[1];
			z = pArray[2];
			w = pArray[3];
		}
#ifdef LUNA_SIMD_ENABLED	
		Float4(Simd::vec_float_p1_t v)
		{
			Simd::store_float4a(m, v);
		}
		operator Simd::vec_float_t() const { return Simd::load_float4a(m); }
#endif
		bool operator==(const Float4& v) const;
		bool operator!=(const Float4& v) const;
		Float4& operator+= (const Float4& v);
		Float4& operator-= (const Float4& v);
		Float4& operator*= (const Float4& v);
		Float4& operator/= (const Float4& v);
		Float4& operator+= (f32 s);
		Float4& operator-= (f32 s);
		Float4& operator*= (f32 s);
		Float4& operator/= (f32 s);

		Float4 operator+ () const { return *this; }
		Float4 operator- () const;

		static constexpr Float4 zero() { return Float4(0.0f, 0.0f, 0.0f, 0.0f); }
		static constexpr Float4 one() { return Float4(1.0f, 1.0f, 1.0f, 1.0f); }
		static constexpr Float4 unit_x() { return Float4(1.0f, 0.0f, 0.0f, 0.0f); }
		static constexpr Float4 unit_y() { return Float4(0.0f, 1.0f, 0.0f, 0.0f); }
		static constexpr Float4 unit_z() { return Float4(0.0f, 0.0f, 1.0f, 0.0f); }
		static constexpr Float4 unit_w() { return Float4(0.0f, 0.0f, 0.0f, 1.0f); }
	};

	Float4 operator+ (const Float4& v1, const Float4& v2);
	Float4 operator- (const Float4& v1, const Float4& v2);
	Float4 operator* (const Float4& v1, const Float4& v2);
	Float4 operator/ (const Float4& v1, const Float4& v2);
	Float4 operator+ (const Float4& v, f32 s);
	Float4 operator+ (f32 s, const Float4& v);
	Float4 operator- (const Float4& v, f32 s);
	Float4 operator- (f32 s, const Float4& v);
	Float4 operator* (const Float4& v, f32 s);
	Float4 operator* (f32 s, const Float4& v);
	Float4 operator/ (const Float4& v, f32 s);
	Float4 operator/ (f32 s, const Float4& v);

	bool in_bounds(const Float4& vec, const Float4& bounds);
	f32 length(const Float4& vec);
	f32 length_squared(const Float4& vec);
	f32 dot(const Float4& vec1, const Float4& vec2);
	Float4 cross(const Float4& v1, const Float4& v2, const Float4& v3);
	Float4 normalize(const Float4& vec);
	Float4 clamp(const Float4& vec, const Float4& vec_min, const Float4& vec_max);
	f32 distance(const Float4& vec1, const Float4& vec2);
	f32 distance_squared(const Float4& vec1, const Float4& vec2);
	Float4 min(const Float4& vec1, const Float4& vec2);
	Float4 max(const Float4& vec1, const Float4& vec2);
	Float4 lerp(const Float4& vec1, const Float4& vec2, f32 t);
	Float4 smoothstep(const Float4& vec1, const Float4& vec2, f32 t);
	Float4 barycentric(const Float4& vec1, const Float4& vec2, const Float4& vec3, f32 f, f32 g);
	Float4 catmull_rom(const Float4& vec1, const Float4& vec2, const Float4& vec3, const Float4& vec4, f32 t);
	Float4 hermite(const Float4& vec1, const Float4& t1, const Float4& vec2, const Float4& t2, f32 t);
	Float4 reflect(const Float4& ivec, const Float4& nvec);
	Float4 refract(const Float4& ivec, const Float4& nvec, f32 refractionIndex);

	template <typename _Ty>
	struct Vec4U
	{
		union
		{
			struct
			{
				_Ty x;
				_Ty y;
				_Ty z;
				_Ty w;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
				_Ty a;
			};
			_Ty m[4];
		};

		Vec4U() = default;
		Vec4U(const Vec4U&) = default;
		Vec4U& operator=(const Vec4U&) = default;
		Vec4U(Vec4U&&) = default;
		Vec4U& operator=(Vec4U&&) = default;

		Vec4U(const Float4& rhs) :
			x((_Ty)rhs.x), y((_Ty)rhs.y), z((_Ty)rhs.z), w((_Ty)rhs.w) {}
		Vec4U& operator=(const Float4& rhs)
		{
			x = (_Ty)rhs.x;
			y = (_Ty)rhs.y;
			z = (_Ty)rhs.z;
			w = (_Ty)rhs.w;
			return *this;
		}
		operator Float4() const
		{
			return Float4((f32)x, (f32)y, (f32)z, (f32)w);
		}

		constexpr explicit Vec4U(_Ty _x) :
			x(_x), y(_x), z(_x), w(_x) {}
		constexpr Vec4U(_Ty _x, _Ty _y, _Ty _z, _Ty _w) :
			x(_x), y(_y), z(_z), w(_w) {}
		explicit Vec4U(const _Ty* pArray)
		{
			x = pArray[0];
			y = pArray[1];
			z = pArray[2];
			w = pArray[3];
		}

		bool operator==(const Vec4U& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
		}
		bool operator!=(const Vec4U& rhs) const
		{
			return !(*this == rhs);
		}
	};

	using Float4U = Vec4U<f32>;
	using Int4U = Vec4U<i32>;
	using UInt4U = Vec4U<u32>;

	//------------------------------------------------------------------------------------
	//
	//	Implementation part.
	//
	//------------------------------------------------------------------------------------

	/****************************************************************************
	 *
	 * Float2
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparison operators
	 //------------------------------------------------------------------------------
	inline bool Float2::operator==(const Float2& vec) const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = load_float2a(vec.m);
		return Vector2Equal(v1, v2);
#else
		return (vec.x == x) && (vec.y == y);
#endif
	}

	inline bool Float2::operator!=(const Float2& vec) const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = load_float2a(vec.m);
		return Vector2NotEqual(v1, v2);
#else
		return (vec.x != x) || (vec.y != y);
#endif
	}


	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Float2& Float2::operator+= (const Float2& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = load_float2a(vec.m);
		vec_float_t X = VectorAdd(v1, v2);
		store_float2a(m, X);
		return *this;
#else
		x += vec.x;
		y += vec.y;
		return *this;
#endif
	}

	inline Float2& Float2::operator-= (const Float2& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = load_float2a(vec.m);
		vec_float_t X = VectorSubtract(v1, v2);
		store_float2a(m, X);
		return *this;
#else
		x -= vec.x;
		y -= vec.y;
		return *this;
#endif
	}

	inline Float2& Float2::operator*= (const Float2& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = load_float2a(vec.m);
		vec_float_t X = VectorMultiply(v1, v2);
		store_float2a(m, X);
		return *this;
#else
		x *= vec.x;
		y *= vec.y;
		return *this;
#endif
	}

	inline Float2& Float2::operator/=(const Float2& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = load_float2a(vec.m);
		vec_float_t X = VectorDivide(v1, v2);
		store_float2a(m, X);
		return *this;
#else
		x /= vec.x;
		y /= vec.y;
		return *this;
#endif
	}

	inline Float2 & Float2::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorAdd(v1, v2);
		store_float2a(m, v2);
		return *this;
#else
		x += s;
		y += s;
		return *this;
#endif
	}

	inline Float2 & Float2::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorSubtract(v1, v2);
		store_float2a(m, v2);
		return *this;
#else
		x -= s;
		y -= s;
		return *this;
#endif
	}

	inline Float2& Float2::operator*= (f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(m);
		vec_float_t X = VectorScale(v1, s);
		store_float2a(m, X);
		return *this;
#else
		x *= s;
		y *= s;
		return *this;
#endif
	}

	inline Float2& Float2::operator/= (f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		luassert(s != 0.0f);
		vec_float_t v1 = load_float2a(m);
		vec_float_t X = VectorScale(v1, 1.f / s);
		store_float2a(m, X);
		return *this;
#else
		x /= s;
		y /= s;
		return *this;
#endif
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Float2 operator+ (const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float2a(v1.m);
		vec_float_t vec2 = load_float2a(v2.m);
		vec_float_t X = VectorAdd(vec1, vec2);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(v1.x + v2.x, v1.y + v2.y);
#endif
	}

	inline Float2 operator- (const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float2a(v1.m);
		vec_float_t vec2 = load_float2a(v2.m);
		vec_float_t X = VectorSubtract(vec1, vec2);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(v1.x - v2.x, v1.y - v2.y);
#endif
	}

	inline Float2 operator* (const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float2a(v1.m);
		vec_float_t vec2 = load_float2a(v2.m);
		vec_float_t X = VectorMultiply(vec1, vec2);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(v1.x * v2.x, v1.y * v2.y);
#endif
	}

	inline Float2 operator/ (const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float2a(v1.m);
		vec_float_t vec2 = load_float2a(v2.m);
		vec_float_t X = VectorDivide(vec1, vec2);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(v1.x / v2.x, v1.y / v2.y);
#endif
	}

	inline Float2 operator+(const Float2 & v1, f32 s)
	{
		return Float2(v1.x + s, v1.y + s);
	}

	inline Float2 operator+(f32 s, const Float2 & v1)
	{
		return Float2(v1.x + s, v1.y + s);
	}

	inline Float2 operator-(const Float2 & v1, f32 s)
	{
		return Float2(v1.x - s, v1.y - s);
	}

	inline Float2 operator-(f32 s, const Float2 & v1)
	{
		return Float2(s - v1.x, s - v1.y);
	}

	inline Float2 operator* (const Float2& V, f32 S)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(V.m);
		vec_float_t X = VectorScale(v1, S);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(V.x * S, V.y * S);
#endif
	}

	inline Float2 operator* (f32 S, const Float2& V)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(V.m);
		vec_float_t X = VectorScale(v1, S);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(V.x * S, V.y * S);
#endif
	}

	inline Float2 operator/(const Float2 & v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(v.m);
		s = 1.0f / s;
		vec_float_t X = VectorScale(v1, s);
		Float2 R;
		store_float2a(R.m, X);
		return R;
#else
		return Float2(v.x / s, v.y / s);
#endif
	}

	inline Float2 operator/(f32 s, const Float2 & v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = vector_replicate(s);
		vec_float_t v2 = load_float2a(v.m);
		v2 = VectorDivide(v1, v2);
		Float2 R;
		store_float2a(R.m, v2);
		return R;
#else
		return Float2(s / v.x, s / v.y);
#endif
	}



	//------------------------------------------------------------------------------
	// Vector operations
	//------------------------------------------------------------------------------

	inline bool in_bounds(const Float2& vec, const Float2& bounds)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(vec.m);
		vec_float_t v2 = load_float2a(bounds.m);
		return Vector2InBounds(v1, v2);
#else
		return (vec.x <= bounds.x && vec.x >= -bounds.x) &&
			(vec.y <= bounds.y && vec.y >= -bounds.y);
#endif
	}

	inline f32 length(const Float2& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(vec.m);
		vec_float_t X = Vector2Length(v1);
		return VectorGetX(X);
#else
		return sqrtf(vec.x * vec.x + vec.y * vec.y);
#endif
	}

	inline f32 length_squared(const Float2& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(vec.m);
		vec_float_t X = Vector2LengthSq(v1);
		return VectorGetX(X);
#else
		return vec.x * vec.x + vec.y * vec.y;
#endif
	}

	inline f32 dot(const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float2a(v1.m);
		vec_float_t vec2 = load_float2a(v2.m);
		vec_float_t X = Vector2Dot(vec1, vec2);
		return VectorGetX(X);
#else
		return v1.x * v2.x + v1.y * v2.y;
#endif
	}

	inline Float2 cross(const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float2a(v1.m);
		vec_float_t vec2 = load_float2a(v2.m);
		vec_float_t R = Vector2Cross(vec1, vec2);
		Float2 result;
		store_float2a(result.m, R);
		return result;
#else
		f32 fCross = v1.x * v2.y - v1.y * v2.x;
		Float2 result;
		result.x = fCross;
		result.y = fCross;
		return result;
#endif
	}

	inline Float2 normalize(const Float2& v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(v.m);
		vec_float_t X = Vector2Normalize(v1);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		f32 fLength = length(v);
		if (fLength > 0)
		{
			fLength = 1.0f / fLength;
		}
		Float2 result;
		result.x = v.x * fLength;
		result.y = v.y * fLength;
		return result;
#endif
	}

	inline Float2 clamp(const Float2& v, const Float2& vmin, const Float2& vmax)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float2a(v.m);
		vec_float_t v2 = load_float2a(vmin.m);
		vec_float_t v3 = load_float2a(vmax.m);
		vec_float_t X = VectorClamp(v1, v2, v3);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = v.x > vmin.x ? v.x : vmin.x;
		result.x = result.x < vmax.x ? result.x : vmax.x;
		result.y = v.y > vmin.y ? v.y : vmin.y;
		result.y = result.y < vmax.y ? result.y : vmax.y;
		return result;
#endif
	}

	inline f32 distance(const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t V = VectorSubtract(x2, x1);
		vec_float_t X = Vector2Length(V);
		return VectorGetX(X);
#else
		f32 delX = v1.x - v2.x;
		f32 delY = v1.y - v2.y;
		return sqrtf(delX * delX + delY * delY);
#endif
	}

	inline f32 distance_squared(const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t V = VectorSubtract(x2, x1);
		vec_float_t X = Vector2LengthSq(V);
		return VectorGetX(X);
#else
		f32 delX = v1.x - v2.x;
		f32 delY = v1.y - v2.y;
		return delX * delX + delY * delY;
#endif
	}

	inline Float2 min(const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t X = VectorMin(x1, x2);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = v1.x < v2.x ? v1.x : v2.x;
		result.y = v1.y < v2.y ? v1.y : v2.y;
		return result;
#endif
	}

	inline Float2 max(const Float2& v1, const Float2& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t X = VectorMax(x1, x2);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = v1.x > v2.x ? v1.x : v2.x;
		result.y = v1.y > v2.y ? v1.y : v2.y;
		return result;
#endif
	}

	inline Float2 lerp(const Float2& v1, const Float2& v2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t X = VectorLerp(x1, x2, t);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = v1.x + t * (v2.x - v1.x);
		result.y = v1.y + t * (v2.y - v1.y);
		return result;
#endif
	}

	inline Float2 smoothstep(const Float2& v1, const Float2& v2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t X = VectorLerp(x1, x2, t);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		return lerp(v1, v2, t);
#endif
	}

	inline Float2 barycentric(const Float2& v1, const Float2& v2, const Float2& v3, f32 f, f32 g)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t x3 = load_float2a(v3.m);
		vec_float_t X = VectorBaryCentric(x1, x2, x3, f, g);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = v1.x + (v2.x - v1.x) * f + (v3.x - v1.x) * g;
		result.y = v1.y + (v2.y - v1.y) * f + (v3.y - v1.y) * g;
		return result;
#endif
	}

	inline Float2 catmull_rom(const Float2& v1, const Float2& v2, const Float2& v3, const Float2& v4, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(v2.m);
		vec_float_t x3 = load_float2a(v3.m);
		vec_float_t x4 = load_float2a(v4.m);
		vec_float_t X = VectorCatmullRom(x1, x2, x3, x4, t);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.x
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.x
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.x
			+ ((t * t * t) - (t * t)) * v4.x) * 0.5f;
		result.y = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.y
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.y
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.y
			+ ((t * t * t) - (t * t)) * v4.y) * 0.5f;
		return result;
#endif
	}

	inline Float2 hermite(const Float2& v1, const Float2& t1, const Float2& v2, const Float2& t2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float2a(v1.m);
		vec_float_t x2 = load_float2a(t1.m);
		vec_float_t x3 = load_float2a(v2.m);
		vec_float_t x4 = load_float2a(t2.m);
		vec_float_t X = VectorHermite(x1, x2, x3, x4, t);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		Float2 result;
		result.x = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.x +
			((t * t * t) - 2 * (t * t) + t) * t1.x +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.x +
			((t * t * t) - (t * t)) * t2.x;
		result.y = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.y +
			((t * t * t) - 2 * (t * t) + t) * t1.y +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.y +
			((t * t * t) - (t * t)) * t2.y;
		return result;
#endif
	}

	inline Float2 reflect(const Float2& ivec, const Float2& nvec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t i = load_float2a(ivec.m);
		vec_float_t n = load_float2a(nvec.m);
		vec_float_t X = Vector2Reflect(i, n);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		return ivec - (2 * dot(ivec, nvec) * nvec);
#endif
	}

	inline Float2 refract(const Float2& ivec, const Float2& nvec, f32 refractionIndex)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t i = load_float2a(ivec.m);
		vec_float_t n = load_float2a(nvec.m);
		vec_float_t X = Vector2Refract(i, n, refractionIndex);
		Float2 result;
		store_float2a(result.m, X);
		return result;
#else
		f32 din = dot(ivec, nvec);
		return refractionIndex * ivec - nvec * (refractionIndex * din +
			sqrtf(1 - refractionIndex * refractionIndex * (1 - din * din)));
#endif
	}

	inline bool in_rect(const Float2& point, const Float2& min_point, const Float2& max_point)
	{
		return point.x >= min_point.x && point.x <= max_point.x && 
			point.y >= min_point.y && point.y <= max_point.y;
	}

	/****************************************************************************
	 *
	 * Float3
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparison operators
	 //------------------------------------------------------------------------------

	inline bool Float3::operator == (const Float3& V) const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = load_float3a(V.m);
		return Vector3Equal(v1, v2);
#else
		return (x == V.x) && (y == V.y) && (z == V.z);
#endif
	}

	inline bool Float3::operator != (const Float3& V) const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = load_float3a(V.m);
		return Vector3NotEqual(v1, v2);
#else
		return (x != V.x) || (y != V.y) || (z != V.z);
#endif
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Float3& Float3::operator+= (const Float3& V)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = load_float3a(V.m);
		vec_float_t X = VectorAdd(v1, v2);
		store_float3a(m, X);
		return *this;
#else
		x += V.x;
		y += V.y;
		z += V.z;
		return *this;
#endif
	}

	inline Float3& Float3::operator-= (const Float3& V)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = load_float3a(V.m);
		vec_float_t X = VectorSubtract(v1, v2);
		store_float3a(m, X);
		return *this;
#else
		x -= V.x;
		y -= V.y;
		z -= V.z;
		return *this;
#endif
	}

	inline Float3& Float3::operator*= (const Float3& V)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = load_float3a(V.m);
		vec_float_t X = VectorMultiply(v1, v2);
		store_float3a(m, X);
		return *this;
#else
		x *= V.x;
		y *= V.y;
		z *= V.z;
		return *this;
#endif
	}

	inline Float3& Float3::operator/=(const Float3 & v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = load_float3a(v.m);
		vec_float_t X = VectorDivide(v1, v2);
		store_float3a(m, X);
		return *this;
#else
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
#endif
	}

	inline Float3& Float3::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = vector_replicate(s);
		vec_float_t X = VectorAdd(v1, v2);
		store_float3a(m, X);
		return *this;
#else
		x += s;
		y += s;
		z += s;
		return *this;
#endif
	}

	inline Float3& Float3::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t v2 = vector_replicate(s);
		vec_float_t X = VectorSubtract(v1, v2);
		store_float3a(m, X);
		return *this;
#else
		x -= s;
		y -= s;
		z -= s;
		return *this;
#endif
	}

	inline Float3& Float3::operator*= (f32 S)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t X = VectorScale(v1, S);
		store_float3a(m, X);
		return *this;
#else
		x *= S;
		y *= S;
		z *= S;
		return *this;
#endif
	}

	inline Float3& Float3::operator/= (f32 S)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		luassert(S != 0.0f);
		vec_float_t v1 = load_float3a(m);
		vec_float_t X = VectorScale(v1, 1.f / S);
		store_float3a(m, X);
		return *this;
#else
		x /= S;
		y /= S;
		z /= S;
		return *this;
#endif
	}

	//------------------------------------------------------------------------------
	// Unary operators
	//------------------------------------------------------------------------------

	inline Float3 Float3::operator- () const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(m);
		vec_float_t X = VectorNegate(v1);
		Float3 R;
		store_float3a(R.m, X);
		return R;
#else
		return Float3(-x, -y, -z);
#endif
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Float3 operator+ (const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float3a(v1.m);
		vec_float_t vec2 = load_float3a(v2.m);
		vec_float_t X = VectorAdd(vec1, vec2);
		Float3 R;
		store_float3a(R.m, X);
		return R;
#else
		return Float3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
#endif
	}

	inline Float3 operator- (const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float3a(v1.m);
		vec_float_t vec2 = load_float3a(v2.m);
		vec_float_t X = VectorSubtract(vec1, vec2);
		Float3 R;
		store_float3a(R.m, X);
		return R;
#else
		return Float3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
#endif
	}

	inline Float3 operator* (const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float3a(v1.m);
		vec_float_t vec2 = load_float3a(v2.m);
		vec_float_t X = VectorMultiply(vec1, vec2);
		Float3 R;
		store_float3a(R.m, X);
		return R;
#else
		return Float3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
#endif
	}

	inline Float3 operator/ (const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float3a(v1.m);
		vec_float_t vec2 = load_float3a(v2.m);
		vec_float_t X = VectorDivide(vec1, vec2);
		Float3 R;
		store_float3a(R.m, X);
		return R;
#else
		return Float3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
#endif
	}

	inline Float3 operator+(const Float3& v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(v.m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorAdd(v1, v2);
		Float3 R;
		store_float3a(R.m, v2);
		return R;
#else
		return Float3(v.x + s, v.y + s, v.z + s);
#endif
	}

	inline Float3 operator+(f32 s, const Float3 & v)
	{
		return v + s;
	}

	inline Float3 operator-(const Float3 & v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(v.m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorSubtract(v1, v2);
		Float3 R;
		store_float3a(R.m, v2);
		return R;
#else
		return Float3(v.x - s, v.y - s, v.z - s);
#endif
	}

	inline Float3 operator-(f32 s, const Float3 & v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v2 = load_float3a(v.m);
		vec_float_t v1 = vector_replicate(s);
		v2 = VectorSubtract(v1, v2);
		Float3 R;
		store_float3a(R.m, v2);
		return R;
#else
		return Float3(s - v.x, s - v.y, s - v.z);
#endif
	}

	inline Float3 operator* (const Float3& V, f32 S)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(V.m);
		v1 = VectorScale(v1, S);
		Float3 R;
		store_float3a(R.m, v1);
		return R;
#else
		return Float3(V.x * S, V.y * S, V.z * S);
#endif
	}

	inline Float3 operator* (f32 S, const Float3& V)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(V.m);
		v1 = VectorScale(v1, S);
		Float3 R;
		store_float3a(R.m, v1);
		return R;
#else
		return Float3(V.x * S, V.y * S, V.z * S);
#endif
	}

	inline Float3 operator/(const Float3& v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(v.m);
		s = 1.0f / s;
		v1 = VectorScale(v1, s);
		Float3 R;
		store_float3a(R.m, v1);
		return R;
#else
		return Float3(v.x / s, v.y / s, v.z / s);
#endif
	}

	inline Float3 operator/(f32 s, const Float3 & v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = vector_replicate(s);
		vec_float_t v2 = load_float3a(v.m);
		v2 = VectorDivide(v1, v2);
		Float3 R;
		store_float3a(R.m, v2);
		return R;
#else
		return Float3(s / v.x, s / v.y, s / v.z);
#endif
	}

	//------------------------------------------------------------------------------
	// Vector operations
	//------------------------------------------------------------------------------

	inline bool in_bounds(const Float3& vec, const Float3& bounds)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(vec.m);
		vec_float_t v2 = load_float3a(bounds.m);
		return Vector3InBounds(v1, v2);
#else
		return (vec.x <= bounds.x && vec.x >= -bounds.x) &&
			(vec.y <= bounds.y && vec.y >= -bounds.y) &&
			(vec.z <= bounds.z && vec.z >= -bounds.z);
#endif
	}

	inline f32 length(const Float3& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(vec.m);
		vec_float_t X = Vector3Length(v1);
		return VectorGetX(X);
#else
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
#endif
	}

	inline f32 length_squared(const Float3& vec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(vec.m);
		vec_float_t X = Vector3LengthSq(v1);
		return VectorGetX(X);
#else
		return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
#endif
	}

	inline f32 dot(const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float3a(v1.m);
		vec_float_t vec2 = load_float3a(v2.m);
		vec_float_t X = Vector3Dot(vec1, vec2);
		return VectorGetX(X);
#else
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
#endif
	}

	inline Float3 cross(const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float3a(v1.m);
		vec_float_t vec2 = load_float3a(v2.m);
		vec_float_t R = Vector3Cross(vec1, vec2);
		Float3 result;
		store_float3a(result.m, R);
		return result;
#else
		Float3 result;
		result.x = v1.y * v2.z - v1.z * v2.y;
		result.y = v1.z * v2.x - v1.x * v2.z;
		result.z = v1.x * v2.y - v1.y * v2.x;
		return result;
#endif
	}

	inline Float3 normalize(const Float3& v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(v.m);
		vec_float_t X = Vector3Normalize(v1);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		f32 fLength = length(v);
		if (fLength > 0)
		{
			fLength = 1.0f / fLength;
		}
		Float3 result;
		result.x = v.x * fLength;
		result.y = v.y * fLength;
		result.z = v.z * fLength;
		return result;
#endif
	}

	inline Float3 clamp(const Float3& v, const Float3& vmin, const Float3& vmax)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float3a(v.m);
		vec_float_t v2 = load_float3a(vmin.m);
		vec_float_t v3 = load_float3a(vmax.m);
		vec_float_t X = VectorClamp(v1, v2, v3);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = v.x > vmin.x ? v.x : vmin.x;
		result.x = result.x < vmax.x ? result.x : vmax.x;
		result.y = v.y > vmin.y ? v.y : vmin.y;
		result.y = result.y < vmax.y ? result.y : vmax.y;
		result.z = v.z > vmin.z ? v.z : vmin.z;
		result.z = result.z < vmax.z ? result.z : vmax.z;
		return result;
#endif
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline f32 distance(const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t V = VectorSubtract(x2, x1);
		vec_float_t X = Vector3Length(V);
		return VectorGetX(X);
#else
		f32 delX = v1.x - v2.x;
		f32 delY = v1.y - v2.y;
		f32 delZ = v1.z - v2.z;
		return sqrtf(delX * delX + delY * delY + delZ * delZ);
#endif
	}

	inline f32 distance_squared(const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t V = VectorSubtract(x2, x1);
		vec_float_t X = Vector3LengthSq(V);
		return VectorGetX(X);
#else
		f32 delX = v1.x - v2.x;
		f32 delY = v1.y - v2.y;
		f32 delZ = v1.z - v2.z;
		return delX * delX + delY * delY + delZ * delZ;
#endif
	}

	inline Float3 min(const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t X = VectorMin(x1, x2);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = v1.x < v2.x ? v1.x : v2.x;
		result.y = v1.y < v2.y ? v1.y : v2.y;
		result.z = v1.z < v2.z ? v1.z : v2.z;
		return result;
#endif
	}

	inline Float3 max(const Float3& v1, const Float3& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t X = VectorMax(x1, x2);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = v1.x > v2.x ? v1.x : v2.x;
		result.y = v1.y > v2.y ? v1.y : v2.y;
		result.z = v1.z > v2.z ? v1.z : v2.z;
		return result;
#endif
	}

	inline Float3 lerp(const Float3& v1, const Float3& v2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t X = VectorLerp(x1, x2, t);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = v1.x + t * (v2.x - v1.x);
		result.y = v1.y + t * (v2.y - v1.y);
		result.z = v1.z + t * (v2.z - v1.z);
		return result;
#endif
	}

	inline Float3 smoothstep(const Float3& v1, const Float3& v2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t X = VectorLerp(x1, x2, t);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		return lerp(v1, v2, t);
#endif
	}

	inline Float3 barycentric(const Float3& v1, const Float3& v2, const Float3& v3, f32 f, f32 g)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t x3 = load_float3a(v3.m);
		vec_float_t X = VectorBaryCentric(x1, x2, x3, f, g);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = v1.x + (v2.x - v1.x) * f + (v3.x - v1.x) * g;
		result.y = v1.y + (v2.y - v1.y) * f + (v3.y - v1.y) * g;
		result.z = v1.z + (v2.z - v1.z) * f + (v3.z - v1.z) * g;
		return result;
#endif
	}

	inline Float3 catmull_rom(const Float3& v1, const Float3& v2, const Float3& v3, const Float3& v4, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(v2.m);
		vec_float_t x3 = load_float3a(v3.m);
		vec_float_t x4 = load_float3a(v4.m);
		vec_float_t X = VectorCatmullRom(x1, x2, x3, x4, t);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.x
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.x
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.x
			+ ((t * t * t) - (t * t)) * v4.x) * 0.5f;
		result.y = ((-(t * t * t) + 2 * (t * t) - t) * v1.y
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.y
			+ (-3.0f * (t * t * t) + 4 * (t * t) + t) * v3.y
			+ ((t * t * t) - (t * t)) * v4.y) * 0.5f;
		result.z = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.z
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.z
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.z
			+ ((t * t * t) - (t * t)) * v4.z) * 0.5f;
		return result;
#endif
	}

	inline Float3 hermite(const Float3& v1, const Float3& t1, const Float3& v2, const Float3& t2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t x1 = load_float3a(v1.m);
		vec_float_t x2 = load_float3a(t1.m);
		vec_float_t x3 = load_float3a(v2.m);
		vec_float_t x4 = load_float3a(t2.m);
		vec_float_t X = VectorHermite(x1, x2, x3, x4, t);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		result.x = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.x +
			((t * t * t) - 2 * (t * t) + t) * t1.x +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.x +
			((t * t * t) - (t * t)) * t2.x;
		result.y = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.y +
			((t * t * t) - 2 * (t * t) + t) * t1.y +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.y +
			((t * t * t) - (t * t)) * t2.y;
		result.z = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.z +
			((t * t * t) - 2 * (t * t) + t) * t1.z +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.z +
			((t * t * t) - (t * t)) * t2.z;
		return result;
#endif
	}

	inline Float3 reflect(const Float3& ivec, const Float3& nvec)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t i = load_float3a(ivec.m);
		vec_float_t n = load_float3a(nvec.m);
		vec_float_t X = Vector3Reflect(i, n);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		Float3 result;
		f32 s = 2.0f * (ivec.x * nvec.x + ivec.y * nvec.y + ivec.z * nvec.z);
		result.x = ivec.x - s * nvec.x;
		result.y = ivec.y - s * nvec.y;
		result.z = ivec.z - s * nvec.z;
		return result;
#endif
	}

	inline Float3 refract(const Float3& ivec, const Float3& nvec, f32 refraction_index)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t i = load_float3a(ivec.m);
		vec_float_t n = load_float3a(nvec.m);
		vec_float_t X = Vector3Refract(i, n, refraction_index);
		Float3 result;
		store_float3a(result.m, X);
		return result;
#else
		f32 t = (ivec.x * nvec.x + ivec.y * nvec.y + ivec.z * nvec.z);
		f32 r = 1.0f - refraction_index * refraction_index * (1.0f - t * t);
		Float3 result;
		if (r < 0.0f) // Total internal reflection
		{
			result.x = 0.0f;
			result.y = 0.0f;
			result.z = 0.0f;
		}
		else
		{
			f32 s = refraction_index * t + sqrtf(r);
			result.x = refraction_index * ivec.x - s * nvec.x;
			result.y = refraction_index * ivec.y - s * nvec.y;
			result.z = refraction_index * ivec.z - s * nvec.z;
		}
		return result;
#endif
	}

	/****************************************************************************
	 *
	 * Float4
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 // Comparison operators
	 //------------------------------------------------------------------------------

	inline bool Float4::operator == (const Float4& V) const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = load_float4a(V.m);
		return Vector4Equal(v1, v2);
#else
		return (x == V.x) && (y == V.y) && (z == V.z) && (w == V.w);
#endif
	}

	inline bool Float4::operator != (const Float4& V) const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = load_float4a(V.m);
		return Vector4NotEqual(v1, v2);
#else
		return (x != V.x) || (y != V.y) || (z != V.z) || (w != V.w);
#endif
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Float4& Float4::operator+= (const Float4& v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = load_float4a(v.m);
		vec_float_t X = VectorAdd(v1, v2);
		store_float4a(m, X);
		return *this;
#else
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
#endif
	}

	inline Float4& Float4::operator-= (const Float4& v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = load_float4a(v.m);
		vec_float_t X = VectorSubtract(v1, v2);
		store_float4a(m, X);
		return *this;
#else
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
#endif
	}

	inline Float4& Float4::operator*= (const Float4& v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = load_float4a(v.m);
		vec_float_t X = VectorMultiply(v1, v2);
		store_float4a(m, X);
		return *this;
#else
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
		return *this;
#endif
	}

	inline Float4& Float4::operator/= (const Float4& v)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = load_float4a(v.m);
		vec_float_t X = VectorDivide(v1, v2);
		store_float4a(m, X);
		return *this;
#else
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
		return *this;
#endif
	}

	inline Float4 & Float4::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorAdd(v1, v2);
		store_float4a(m, v2);
		return *this;
#else
		x += s;
		y += s;
		z += s;
		w += s;
		return *this;
#endif
	}

	inline Float4 & Float4::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorSubtract(v1, v2);
		store_float4a(m, v2);
		return *this;
#else
		x -= s;
		y -= s;
		z -= s;
		w -= s;
		return *this;
#endif
	}

	inline Float4& Float4::operator*= (f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t X = VectorScale(v1, s);
		store_float4a(m, X);
		return *this;
#else
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
#endif
	}

	inline Float4& Float4::operator/= (f32 s)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		luassert(s != 0.0f);
		vec_float_t v1 = load_float4a(m);
		s = 1.0f / s;
		vec_float_t X = VectorScale(v1, s);
		store_float4a(m, X);
		return *this;
#else
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
#endif
	}

	//------------------------------------------------------------------------------
	// Unary operators
	//------------------------------------------------------------------------------

	inline Float4 Float4::operator- () const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = load_float4a(m);
		vec_float_t X = VectorNegate(v1);
		Float4 R;
		store_float4a(R.m, X);
		return R;
#else
		return Float4(-x, -y, -z, -w);
#endif
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Float4 operator+ (const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float4a(v1.m);
		vec_float_t vec2 = load_float4a(v2.m);
		vec_float_t X = VectorAdd(vec1, vec2);
		Float4 R;
		store_float4a(R.m, X);
		return R;
#else
		return Float4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
#endif
	}

	inline Float4 operator- (const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t vec1 = load_float4a(v1.m);
		vec_float_t vec2 = load_float4a(v2.m);
		vec_float_t X = VectorSubtract(vec1, vec2);
		Float4 R;
		store_float4a(R.m, X);
		return R;
#else
		return Float4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
#endif
	}

	inline Float4 operator* (const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t vec1 = load_float4a(v1.m);
		vec_float_t vec2 = load_float4a(v2.m);
		vec_float_t X = VectorMultiply(vec1, vec2);
		Float4 R;
		store_float4a(R.m, X);
		return R;
#else
		return Float4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
#endif
	}

	inline Float4 operator/ (const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t vec1 = load_float4a(v1.m);
		vec_float_t vec2 = load_float4a(v2.m);
		vec_float_t X = VectorDivide(vec1, vec2);
		Float4 R;
		store_float4a(R.m, X);
		return R;
#else
		return Float4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
#endif
	}

	inline Float4 operator+(const Float4& v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v1 = load_float4a(v.m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorAdd(v1, v2);
		Float4 R;
		store_float4a(R.m, v2);
		return R;
#else
		return Float4(v.x + s, v.y + s, v.z + s, v.w + s);
#endif
	}

	inline Float4 operator+(f32 s, const Float4& v)
	{
		return v + s;
	}

	inline Float4 operator-(const Float4& v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v1 = load_float4a(v.m);
		vec_float_t v2 = vector_replicate(s);
		v2 = VectorSubtract(v1, v2);
		Float4 R;
		store_float4a(R.m, v2);
		return R;
#else
		return Float4(v.x - s, v.y - s, v.z - s, v.w - s);
#endif
	}

	inline Float4 operator-(f32 s, const Float4 & v)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v2 = load_float4a(v.m);
		vec_float_t v1 = vector_replicate(s);
		v2 = VectorSubtract(v1, v2);
		Float4 R;
		store_float4a(R.m, v2);
		return R;
#else
		return Float4(s - v.x, s - v.y, s - v.z, s - v.w);
#endif
	}

	inline Float4 operator* (const Float4& V, f32 S)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v1 = load_float4a(V.m);
		vec_float_t X = VectorScale(v1, S);
		Float4 R;
		store_float4a(R.m, X);
		return R;
#else
		return Float4(V.x * S, V.y * S, V.z * S, V.w * S);
#endif
	}

	inline Float4 operator* (f32 S, const Float4& V)
	{
		return V * S;
	}


	inline Float4 operator/ (const Float4& v, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v1 = load_float4a(v.m);
		v1 = VectorScale(v1, 1.0f / s);
		Float4 R;
		store_float4a(R.m, v1);
		return R;
#else
		return Float4(v.x / s, v.y / s, v.z / s, v.w / s);
#endif
	}

	inline Float4 operator/(f32 s, const Float4& v)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v2 = load_float4a(v.m);
		vec_float_t v1 = vector_replicate(s);
		v2 = VectorDivide(v1, v2);
		Float4 R;
		store_float4a(R.m, v2);
		return R;
#else
		return Float4(s / v.x, s / v.y, s / v.z, s / v.w);
#endif
	}



	//------------------------------------------------------------------------------
	// Vector operations
	//------------------------------------------------------------------------------

	inline bool in_bounds(const Float4& vec, const Float4& bounds)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(vec.m);
		vec_float_t v2 = load_float4a(bounds.m);
		return Vector4InBounds(v1, v2);
#else
		return (vec.x <= bounds.x && vec.x >= -bounds.x) &&
			(vec.y <= bounds.y && vec.y >= -bounds.y) &&
			(vec.z <= bounds.z && vec.z >= -bounds.z) &&
			(vec.w <= bounds.w && vec.w >= -bounds.w);
#endif
	}

	inline f32 length(const Float4& vec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(vec.m);
		vec_float_t X = Vector4Length(v1);
		return VectorGetX(X);
#else
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
#endif
	}

	inline f32 length_squared(const Float4& vec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(vec.m);
		vec_float_t X = Vector4LengthSq(v1);
		return VectorGetX(X);
#else
		return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w;
#endif
	}

	inline f32 dot(const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t vec1 = load_float4a(v1.m);
		vec_float_t vec2 = load_float4a(v2.m);
		vec_float_t X = Vector4Dot(vec1, vec2);
		return VectorGetX(X);
#else
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
#endif
	}

	inline Float4 cross(const Float4& v1, const Float4& v2, const Float4& v3)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t x3 = load_float4a(v3.m);
		vec_float_t R = Vector4Cross(x1, x2, x3);
		Float4 result;
		store_float4a(result.m, R);
		return result;
#else
		Float4 result;
		result.x = ((v2.z*v3.w - v2.w*v3.z)*v1.y) - ((v2.y*v3.w - v2.w*v3.y)*v1.z) + ((v2.y*v3.z - v2.z*v3.y)*v1.w);
		result.y = ((v2.w*v3.z - v2.z*v3.w)*v1.x) - ((v2.w*v3.x - v2.x*v3.w)*v1.z) + ((v2.z*v3.x - v2.x*v3.z)*v1.w);
		result.z = ((v2.y*v3.w - v2.w*v3.y)*v1.x) - ((v2.x*v3.w - v2.w*v3.x)*v1.y) + ((v2.x*v3.y - v2.y*v3.x)*v1.w);
		result.w = ((v2.z*v3.y - v2.y*v3.z)*v1.x) - ((v2.z*v3.x - v2.x*v3.z)*v1.y) + ((v2.y*v3.x - v2.x*v3.y)*v1.z);
		return result;
#endif
	}

	inline Float4 normalize(const Float4& v)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(v.m);
		vec_float_t X = Vector4Normalize(v1);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		f32 fLength = length(v);
		if (fLength > 0)
		{
			fLength = 1.0f / fLength;
		}
		result.x = v.x * fLength;
		result.y = v.y * fLength;
		result.z = v.z * fLength;
		result.w = v.w * fLength;
		return result;
#endif
	}

	inline Float4 clamp(const Float4& v, const Float4& vmin, const Float4& vmax)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(v.m);
		vec_float_t v2 = load_float4a(vmin.m);
		vec_float_t v3 = load_float4a(vmax.m);
		vec_float_t X = VectorClamp(v1, v2, v3);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = v.x > vmin.x ? v.x : vmin.x;
		result.x = result.x < vmax.x ? result.x : vmax.x;
		result.y = v.y > vmin.y ? v.y : vmin.y;
		result.y = result.y < vmax.y ? result.y : vmax.y;
		result.z = v.z > vmin.z ? v.z : vmin.z;
		result.z = result.z < vmax.z ? result.z : vmax.z;
		result.w = v.w > vmin.w ? v.w : vmin.w;
		result.w = result.w < vmax.w ? result.w : vmax.w;
		return result;
#endif
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline f32 distance(const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t V = VectorSubtract(x2, x1);
		vec_float_t X = Vector4Length(V);
		return VectorGetX(X);
#else
		f32 delX = v1.x - v2.x;
		f32 delY = v1.y - v2.y;
		f32 delZ = v1.z - v2.z;
		f32 delW = v1.w - v2.w;
		return sqrtf(delX * delX + delY * delY + delZ * delZ + delW * delW);
#endif
	}

	inline f32 distance_squared(const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t V = VectorSubtract(x2, x1);
		vec_float_t X = Vector4LengthSq(V);
		return VectorGetX(X);
#else
		f32 delX = v1.x - v2.x;
		f32 delY = v1.y - v2.y;
		f32 delZ = v1.z - v2.z;
		f32 delW = v1.w - v2.w;
		return delX * delX + delY * delY + delZ * delZ + delW * delW;
#endif
	}

	inline Float4 min(const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t X = VectorMin(x1, x2);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = v1.x < v2.x ? v1.x : v2.x;
		result.y = v1.y < v2.y ? v1.y : v2.y;
		result.z = v1.z < v2.z ? v1.z : v2.z;
		result.w = v1.w < v2.w ? v1.w : v2.w;
		return result;
#endif
	}

	inline Float4 max(const Float4& v1, const Float4& v2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t X = VectorMax(x1, x2);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = v1.x > v2.x ? v1.x : v2.x;
		result.y = v1.y > v2.y ? v1.y : v2.y;
		result.z = v1.z > v2.z ? v1.z : v2.z;
		result.w = v1.w > v2.w ? v1.w : v2.w;
		return result;
#endif
	}

	inline Float4 lerp(const Float4& v1, const Float4& v2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t X = VectorLerp(x1, x2, t);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = v1.x + t * (v2.x - v1.x);
		result.y = v1.y + t * (v2.y - v1.y);
		result.z = v1.z + t * (v2.z - v1.z);
		result.w = v1.w + t * (v2.w - v1.w);
		return result;
#endif
	}

	inline Float4 smoothstep(const Float4& v1, const Float4& v2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t X = VectorLerp(x1, x2, t);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		return lerp(v1, v2, t);
#endif
	}

	inline Float4 barycentric(const Float4& v1, const Float4& v2, const Float4& v3, f32 f, f32 g)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t x3 = load_float4a(v3.m);
		vec_float_t X = VectorBaryCentric(x1, x2, x3, f, g);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = v1.x + (v2.x - v1.x) * f + (v3.x - v1.x) * g;
		result.y = v1.y + (v2.y - v1.y) * f + (v3.y - v1.y) * g;
		result.z = v1.z + (v2.z - v1.z) * f + (v3.z - v1.z) * g;
		result.w = v1.w + (v2.w - v1.w) * f + (v3.w - v1.w) * g;
		return result;
#endif
	}

	inline Float4 catmull_rom(const Float4& v1, const Float4& v2, const Float4& v3, const Float4& v4, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(v2.m);
		vec_float_t x3 = load_float4a(v3.m);
		vec_float_t x4 = load_float4a(v4.m);
		vec_float_t X = VectorCatmullRom(x1, x2, x3, x4, t);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.x
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.x
			+ (-3.0f * (t * t * t) + 4 * (t * t) + t) * v3.x
			+ ((t * t * t) - (t * t)) * v4.x) * 0.5f;
		result.y = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.y
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.y
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.y
			+ ((t * t * t) - (t * t)) * v4.y) * 0.5f;
		result.z = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.z
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.z
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.z
			+ ((t * t * t) - (t * t)) * v4.z) * 0.5f;
		result.w = ((-(t * t * t) + 2.0f * (t * t) - t) * v1.w
			+ (3.0f * (t * t * t) - 5.0f * (t * t) + 2.0f) * v2.w
			+ (-3.0f * (t * t * t) + 4.0f * (t * t) + t) * v3.w
			+ ((t * t * t) - (t * t)) * v4.w) * 0.5f;
		return result;
#endif
	}

	inline Float4 hermite(const Float4& v1, const Float4& t1, const Float4& v2, const Float4& t2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(v1.m);
		vec_float_t x2 = load_float4a(t1.m);
		vec_float_t x3 = load_float4a(v2.m);
		vec_float_t x4 = load_float4a(t2.m);
		vec_float_t X = VectorHermite(x1, x2, x3, x4, t);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		result.x = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.x +
			((t * t * t) - 2 * (t * t) + t) * t1.x +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.x +
			((t * t * t) - (t * t)) * t2.x;
		result.y = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.y +
			((t * t * t) - 2 * (t * t) + t) * t1.y +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.y +
			((t * t * t) - (t * t)) * t2.y;
		result.z = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.z +
			((t * t * t) - 2 * (t * t) + t) * t1.z +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.z +
			((t * t * t) - (t * t)) * t2.z;
		result.w = (2 * (t * t * t) - 3 * (t * t) + 1) * v1.w +
			((t * t * t) - 2 * (t * t) + t) * t1.w +
			(-2 * (t * t * t) + 3 * (t * t)) * v2.w +
			((t * t * t) - (t * t)) * t2.w;
		return result;
#endif
	}

	inline Float4 reflect(const Float4& ivec, const Float4& nvec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t i = load_float4a(ivec.m);
		vec_float_t n = load_float4a(nvec.m);
		vec_float_t X = Vector4Reflect(i, n);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		Float4 result;
		f32 s = 2.0f * (ivec.x * nvec.x + ivec.y * nvec.y + ivec.z * nvec.z + ivec.w * nvec.w);
		result.x = ivec.x - s * nvec.x;
		result.y = ivec.y - s * nvec.y;
		result.z = ivec.z - s * nvec.z;
		result.w = ivec.w - s * nvec.w;
		return result;
#endif
	}

	inline Float4 refract(const Float4& ivec, const Float4& nvec, f32 refractionIndex)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t i = load_float4a(ivec.m);
		vec_float_t n = load_float4a(nvec.m);
		vec_float_t X = Vector4Refract(i, n, refractionIndex);
		Float4 result;
		store_float4a(result.m, X);
		return result;
#else
		f32 t = (ivec.x * nvec.x + ivec.y * nvec.y + ivec.z * nvec.z + ivec.w * nvec.w);
		f32 r = 1.0f - refractionIndex * refractionIndex * (1.0f - t * t);
		Float4 result;
		if (r < 0.0f) // Total internal reflection
		{
			result.x = 0.0f;
			result.y = 0.0f;
			result.z = 0.0f;
			result.w = 0.0f;
		}
		else
		{
			f32 s = refractionIndex * t + sqrtf(r);
			result.x = refractionIndex * ivec.x - s * nvec.x;
			result.y = refractionIndex * ivec.y - s * nvec.y;
			result.z = refractionIndex * ivec.z - s * nvec.z;
			result.w = refractionIndex * ivec.w - s * nvec.w;
		}
		return result;
#endif
	}
}