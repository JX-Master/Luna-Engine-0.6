// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Matrix.hpp
* @author JXMaster
* @date 2019/1/5
 */
#pragma once

#include "Quaternion.hpp"
#include "Transform.hpp"
#include "SimdMatrix.hpp"
#include "SimdMisc.hpp"

namespace Luna
{
	//using namespace Simd;
	struct alignas(16) Float3x3
	{
		union
		{
			struct
			{
				f32 _11, _12, _13, _padding1;
				f32 _21, _22, _23, _padding2;
				f32 _31, _32, _33, _padding3;
			};
			f32 m[3][4];
		};

		Float3x3() = default;

		Float3x3(const Float3x3&) = default;
		Float3x3& operator=(const Float3x3&) = default;

		Float3x3(Float3x3&&) = default;
		Float3x3& operator=(Float3x3&&) = default;

		constexpr Float3x3(f32 m00, f32 m01, f32 m02,
			f32 m10, f32 m11, f32 m12,
			f32 m20, f32 m21, f32 m22)
			: _11(m00), _12(m01), _13(m02),
			_21(m10), _22(m11), _23(m12),
			_31(m20), _32(m21), _33(m22) {}
		explicit Float3x3(const f32 *arr)
		{
			_11 = arr[0]; _12 = arr[1]; _13 = arr[2];
			_21 = arr[3]; _22 = arr[4]; _23 = arr[5];
			_31 = arr[6]; _32 = arr[7]; _33 = arr[8];
		}

		// Comparison operators
		bool operator == (const Float3x3& m) const;
		bool operator != (const Float3x3& m) const;

		// Assignment operators
		Float3x3(const Float3& row1, const Float3& row2, const Float3& row3);

		// Element access.
		f32* operator[](usize i) { return &(m[i][0]); }

		Float3 r1() const { return Float3(_11, _12, _13); }
		Float3 r2() const { return Float3(_21, _22, _23); }
		Float3 r3() const { return Float3(_31, _32, _33); }
		Float3 c1() const { return Float3(_11, _21, _31); }
		Float3 c2() const { return Float3(_12, _22, _32); }
		Float3 c3() const { return Float3(_13, _23, _33); }

		// Unary operators.
		Float3x3 operator+() const { return *this; }
		Float3x3 operator-() const;

		// per-component operations.
		Float3x3& operator+=(const Float3x3&);
		Float3x3& operator-=(const Float3x3&);
		Float3x3& operator*=(const Float3x3&);
		Float3x3& operator/=(const Float3x3&);
		Float3x3& operator+=(f32 s);
		Float3x3& operator-=(f32 s);
		Float3x3& operator*=(f32 s);
		Float3x3& operator/=(f32 s);

		// Constants
		static constexpr Float3x3 identity()
		{
			return Float3x3(
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f);
		}
	};

	// per-component operations.
	Float3x3 operator+(const Float3x3& m1, const Float3x3& m2);
	Float3x3 operator+(const Float3x3& m1, f32 s);
	Float3x3 operator+(f32 s, const Float3x3& m1);
	Float3x3 operator-(const Float3x3& m1, const Float3x3& m2);
	Float3x3 operator-(const Float3x3& m1, f32 s);
	Float3x3 operator-(f32 s, const Float3x3& m1);
	Float3x3 operator*(const Float3x3& m1, const Float3x3& m2);
	Float3x3 operator*(const Float3x3& m1, f32 s);
	Float3x3 operator*(f32 s, const Float3x3& m1);
	Float3x3 operator/(const Float3x3& m1, const Float3x3& m2);
	Float3x3 operator/(const Float3x3& m1, f32 s);
	Float3x3 operator/(f32 s, const Float3x3& m1);

	// matrix math.
	Float3 mul(const Float3& vec, const Float3x3& mat);
	Float3 mul(const Float3x3& mat, const Float3& vec);
	Float3x3 mul(const Float3x3& m1, const Float3x3& m2);

	static_assert(sizeof(Float3x3) == 12 * sizeof(f32), "Incorrect Float3x3 size.");

	struct alignas(16) Float4x3
	{
		union
		{
			struct
			{
				f32 _11, _12, _13, _padding1;
				f32 _21, _22, _23, _padding2;
				f32 _31, _32, _33, _padding3;
				f32 _41, _42, _43, _padding4;
			};
			f32 m[4][4];
		};
		Float4x3() = default;

		Float4x3(const Float4x3&) = default;
		Float4x3& operator=(const Float4x3&) = default;

		Float4x3(Float4x3&&) = default;
		Float4x3& operator=(Float4x3&&) = default;

		constexpr Float4x3(f32 m00, f32 m01, f32 m02,
			f32 m10, f32 m11, f32 m12,
			f32 m20, f32 m21, f32 m22,
			f32 m30, f32 m31, f32 m32)
			: _11(m00), _12(m01), _13(m02),
			_21(m10), _22(m11), _23(m12),
			_31(m20), _32(m21), _33(m22),
			_41(m30), _42(m31), _43(m32) {}
		explicit Float4x3(const f32 *arr)
		{
			_11 = arr[0]; _12 = arr[1]; _13 = arr[2];
			_21 = arr[3]; _22 = arr[4]; _23 = arr[5];
			_31 = arr[6]; _32 = arr[7]; _33 = arr[8];
			_41 = arr[9]; _42 = arr[10]; _43 = arr[11];
		}

		// Comparison operators
		bool operator == (const Float4x3& m) const;
		bool operator != (const Float4x3& m) const;

		// Assignment operators.
		Float4x3(const Float3x3& m, const Float3& row4 = Float3(0.0f, 0.0f, 0.0f));
		Float4x3(const Float3& row1, const Float3& row2, const Float3& row3, const Float3& row4);

		// Element access.
		f32* operator[](usize i) { return &(m[i][0]); }

		Float3 r1() const { return Float3(_11, _12, _13); }
		Float3 r2() const { return Float3(_21, _22, _23); }
		Float3 r3() const { return Float3(_31, _32, _33); }
		Float3 r4() const { return Float3(_41, _42, _43); }
		Float4 c1() const { return Float4(_11, _21, _31, _41); }
		Float4 c2() const { return Float4(_12, _22, _32, _42); }
		Float4 c3() const { return Float4(_13, _23, _33, _43); }

		// Unary operators.
		Float4x3 operator+() const { return *this; }
		Float4x3 operator-() const;

		// per-component operations.
		Float4x3& operator+=(const Float4x3&);
		Float4x3& operator-=(const Float4x3&);
		Float4x3& operator*=(const Float4x3&);
		Float4x3& operator/=(const Float4x3&);
		Float4x3& operator+=(f32 s);
		Float4x3& operator-=(f32 s);
		Float4x3& operator*=(f32 s);
		Float4x3& operator/=(f32 s);

		// Constants
		static constexpr Float4x3 identity()
		{
			return Float4x3(
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 0.0f);
		}
	};

	// per-component operations.
	Float4x3 operator+(const Float4x3& m1, const Float4x3& m2);
	Float4x3 operator+(const Float4x3& m1, f32 s);
	Float4x3 operator+(f32 s, const Float4x3& m1);
	Float4x3 operator-(const Float4x3& m1, const Float4x3& m2);
	Float4x3 operator-(const Float4x3& m1, f32 s);
	Float4x3 operator-(f32 s, const Float4x3& m1);
	Float4x3 operator*(const Float4x3& m1, const Float4x3& m2);
	Float4x3 operator*(const Float4x3& m1, f32 s);
	Float4x3 operator*(f32 s, const Float4x3& m1);
	Float4x3 operator/(const Float4x3& m1, const Float4x3& m2);
	Float4x3 operator/(const Float4x3& m1, f32 s);
	Float4x3 operator/(f32 s, const Float4x3& m1);

	// matrix math.
	Float3 mul(const Float4& vec, const Float4x3& mat);
	Float4 mul(const Float4x3& mat, const Float3& vec);

	static_assert(sizeof(Float4x3) == 16 * sizeof(f32), "Incorrect Float4x3 size.");

	struct alignas(16) Float3x4
	{
		union
		{
			struct
			{
				f32 _11, _12, _13, _14;
				f32 _21, _22, _23, _24;
				f32 _31, _32, _33, _34;
			};
			f32 m[3][4];
		};

		Float3x4() = default;

		Float3x4(const Float3x4&) = default;
		Float3x4& operator=(const Float3x4&) = default;

		Float3x4(Float3x4&&) = default;
		Float3x4& operator=(Float3x4&&) = default;

		constexpr Float3x4(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23)
			: _11(m00), _12(m01), _13(m02), _14(m03),
			_21(m10), _22(m11), _23(m12), _24(m13),
			_31(m20), _32(m21), _33(m22), _34(m23) {}
		explicit Float3x4(const f32 *arr)
		{
			_11 = arr[0]; _12 = arr[1]; _13 = arr[2]; _14 = arr[3];
			_21 = arr[4]; _22 = arr[5]; _23 = arr[6]; _24 = arr[7];
			_31 = arr[8]; _32 = arr[9]; _33 = arr[10]; _34 = arr[11];
		}

		// Comparison operators
		bool operator == (const Float3x4& m) const;
		bool operator != (const Float3x4& m) const;

		// Assignment operators.
		Float3x4(const Float3x3& m, const Float3& col4 = Float3(0.0f, 0.0f, 0.0f));
		Float3x4(const Float4& row1, const Float4& row2, const Float4& row3);

		// Element access.
		f32* operator[](usize i) { return &(m[i][0]); }

		Float4 r1() { return Float4(_11, _12, _13, _14); }
		Float4 r2() { return Float4(_21, _22, _23, _24); }
		Float4 r3() { return Float4(_31, _32, _33, _34); }
		Float3 c1() { return Float3(_11, _21, _31); }
		Float3 c2() { return Float3(_12, _22, _32); }
		Float3 c3() { return Float3(_13, _23, _33); }
		Float3 c4() { return Float3(_14, _24, _34); }

		// Unary operators.
		Float3x4 operator+() const { return *this; }
		Float3x4 operator-() const;

		// per-component operations.
		Float3x4& operator+=(const Float3x4&);
		Float3x4& operator-=(const Float3x4&);
		Float3x4& operator*=(const Float3x4&);
		Float3x4& operator/=(const Float3x4&);
		Float3x4& operator+=(f32 s);
		Float3x4& operator-=(f32 s);
		Float3x4& operator*=(f32 s);
		Float3x4& operator/=(f32 s);

		static constexpr Float3x4 identity()
		{
			return Float3x4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f);
		}
	};

	// per-component operations.
	Float3x4 operator+(const Float3x4& m1, const Float3x4& m2);
	Float3x4 operator+(const Float3x4& m1, f32 s);
	Float3x4 operator+(f32 s, const Float3x4& m1);
	Float3x4 operator-(const Float3x4& m1, const Float3x4& m2);
	Float3x4 operator-(const Float3x4& m1, f32 s);
	Float3x4 operator-(f32 s, const Float3x4& m1);
	Float3x4 operator*(const Float3x4& m1, const Float3x4& m2);
	Float3x4 operator*(const Float3x4& m1, f32 s);
	Float3x4 operator*(f32 s, const Float3x4& m1);
	Float3x4 operator/(const Float3x4& m1, const Float3x4& m2);
	Float3x4 operator/(const Float3x4& m1, f32 s);
	Float3x4 operator/(f32 s, const Float3x4& m1);

	// matrix math.
	Float4 mul(const Float3& vec, const Float3x4& mat);
	Float3 mul(const Float3x4& mat, const Float4& vec);

	static_assert(sizeof(Float3x4) == 12 * sizeof(f32), "Incorrect Float3x4 size.");

	struct alignas(16) Float4x4
	{
		union
		{
			struct
			{
				f32 _11, _12, _13, _14;
				f32 _21, _22, _23, _24;
				f32 _31, _32, _33, _34;
				f32 _41, _42, _43, _44;
			};
			f32 m[4][4];
		};

		Float4x4() = default;

		Float4x4(const Float4x4&) = default;
		Float4x4& operator=(const Float4x4&) = default;

		Float4x4(Float4x4&&) = default;
		Float4x4& operator=(Float4x4&&) = default;

		constexpr Float4x4(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33)
			: _11(m00), _12(m01), _13(m02), _14(m03),
			_21(m10), _22(m11), _23(m12), _24(m13),
			_31(m20), _32(m21), _33(m22), _34(m23),
			_41(m30), _42(m31), _43(m32), _44(m33) {}
		explicit Float4x4(const f32 *arr)
		{
			_11 = arr[0]; _12 = arr[1]; _13 = arr[2]; _14 = arr[3];
			_21 = arr[4]; _22 = arr[5]; _23 = arr[6]; _24 = arr[7];
			_31 = arr[8]; _32 = arr[9]; _33 = arr[10]; _34 = arr[11];
			_41 = arr[12]; _42 = arr[13]; _43 = arr[14]; _44 = arr[15];
		}
		
#ifdef LUNA_SIMD_ENABLED
		Float4x4(Simd::mat_float_p1_t m)
		{
			Simd::store_float4x4a(&_11, m);
		}

		operator Simd::mat_float_t() const { return Simd::load_float4x4a(&_11); }
#endif
		// Comparison operators
		bool operator == (const Float4x4& m) const;
		bool operator != (const Float4x4& m) const;

		// Assignment operators
		Float4x4(const Float4x3& m, const Float4& col4 = Float4(0.0f, 0.0f, 0.0f, 1.0f));
		Float4x4(const Float3x4& m, const Float4& row4 = Float4(0.0f, 0.0f, 0.0f, 1.0f));
		Float4x4(const Float4& row1, const Float4& row2, const Float4& row3, const Float4& row4);

		// Element Access.
		f32* operator[](usize i) { return &(m[i][0]); }
		Float4 r1() const { return Float4(_11, _12, _13, _14); }
		Float4 r2() const { return Float4(_21, _22, _23, _24); }
		Float4 r3() const { return Float4(_31, _32, _33, _34); }
		Float4 r4() const { return Float4(_41, _42, _43, _44); }
		Float4 c1() const { return Float4(_11, _21, _31, _41); }
		Float4 c2() const { return Float4(_12, _22, _32, _42); }
		Float4 c3() const { return Float4(_13, _23, _33, _43); }
		Float4 c4() const { return Float4(_14, _24, _34, _44); }

		//! Returns the up direction if this matrix represents a rotation or an affine matrix.
		//! 
		//! The returned vector is scaled by the Y factor of the scale component of the affine 
		//! matrix.
		Float3 up() const { return Float3(_21, _22, _23); }
		//! Returns the down direction if this matrix represents a rotation or an affine matrix.
		//! 
		//! The returned vector is scaled by the Y factor of the scale component of the affine 
		//! matrix.
		Float3 down() const { return Float3(-_21, -_22, -_23); }
		//! Returns the right direction if this matrix represents a rotation or an affine matrix.
		//! 
		//! The returned vector is scaled by the X factor of the scale component of the affine 
		//! matrix.
		Float3 right() const { return Float3(_11, _12, _13); }
		//! Returns the left direction if this matrix represents a rotation or an affine matrix.
		//! 
		//! The returned vector is scaled by the X factor of the scale component of the affine 
		//! matrix.
		Float3 left() const { return Float3(-_11, -_12, -_13); }
		//! Returns the forward direction if this matrix represents a rotation or an affine matrix.
		//! 
		//! The returned vector is scaled by the Z factor of the scale component of the affine 
		//! matrix.
		Float3 forward() const { return Float3(_31, _32, _33); }
		//! Returns the backward direction if this matrix represents a rotation or an affine matrix.
		//! 
		//! The returned vector is scaled by the Z factor of the scale component of the affine 
		//! matrix.
		Float3 backward() const { return Float3(-_31, -_32, -_33); }
		//! Returns the translation component if this matrix represents a rotation or an affine matrix.
		Float3 translation() const { return Float3(_41, _42, _43); }

		//! Extracts the unscaled rotation matrix from this affine matrix.
		Float4x4 rotation_matrix() const;

		//! Computes the euler angles from this rotation matrix. This method cannot be used for affine matrix directly,
		//! to use this method for affine matrix, call `rotation_matrix` to extract the rotation matrix from affine matrix
		//! first.
		//!
		//! The returned euler angles represents the radians of clockwise rotation along Z(roll), X(pitch), Y(yaw) axis in 
		//! that order.
		Float3 euler_angles() const;

		//! Computes the quaternion from this rotation matrix. This method cannot be used for affine matrix directly,
		//! to use this method for affine matrix, call `rotation_matrix` to extract the rotation matrix from affine matrix
		//! first.
		Quaternion quaternion() const;

		//! Returns the scale component if this matrix represents a rotation or an affine matrix.
		Float3 scale_factor() const;

		// Unary operators
		Float4x4 operator+() const { return *this; }
		Float4x4 operator- () const;

		// per-component operations.
		Float4x4& operator+=(const Float4x4&);
		Float4x4& operator-=(const Float4x4&);
		Float4x4& operator*=(const Float4x4&);
		Float4x4& operator/=(const Float4x4&);
		Float4x4& operator+=(f32 s);
		Float4x4& operator-=(f32 s);
		Float4x4& operator*=(f32 s);
		Float4x4& operator/=(f32 s);

		// Constants
		static constexpr Float4x4 identity()
		{
			return Float4x4(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}

		static Float4x4 make_billboard(const Float3& object_pos, const Float3& camera_pos, 
			const Float3& camera_up, const Float3& camera_forward = { 0.0f, 0.0f, 1.0f});
		static Float4x4 make_constrained_billboard(const Float3& object_pos, const Float3& camera_pos,
			const Float3& rotate_axis, const Float3& camera_forward = { 0.0f, 0.0f, 1.0f}, const Float3& object_forward = { 0.0f, 0.0f, -1.0f});
		static Float4x4 make_translation(const Float3& position);
		static Float4x4 make_scale(const Float3& scales);
		static Float4x4 make_rotation_x(f32 radians);
		static Float4x4 make_rotation_y(f32 radians);
		static Float4x4 make_rotation_z(f32 radians);
		static Float4x4 make_rotation_quaternion(const Quaternion& quaternion);
		static Float4x4 make_from_axis_angle(const Float3& axis, f32 angle);
		static Float4x4 make_perspective_field_of_view(f32 fov, f32 aspect_ratio,
			f32 near_plane_distance, f32 far_plance_distance);
		static Float4x4 make_perspective(f32 width, f32 height, f32 near_plane_distance,
			f32 far_plane_distance);
		static Float4x4 make_perspective_off_center(f32 left, f32 right, f32 bottom, 
			f32 top, f32 near_plane_distance, f32 far_plance_distance);
		static Float4x4 make_orthographic(f32 width, f32 height, f32 z_near_place_distance,
			f32 z_far_plane_distance);
		static Float4x4 make_orthographic_off_center(f32 left, f32 right, f32 bottom,
			f32 top, f32 near_plane_distance, f32 far_plance_distance);
		static Float4x4 make_look_at(const Float3& camera_position, const Float3& target_position, const Float3& up_dir);
		static Float4x4 make_world(const Float3& position, const Float3& forward, const Float3& up);
		// Local to world matrix.
		static Float4x4 make_affine_position_rotation_scale(const Float3& position, const Quaternion& rotation, const Float3& scale);
		static Float4x4 make_transform3d(const Tranform3D& transform);
	};

	static_assert(sizeof(Float4x4) == 16 * sizeof(f32), "Incorrect Float4x4 size.");

	// Binary operators
	Float4x4 operator+(const Float4x4& m1, const Float4x4& m2);
	Float4x4 operator+(const Float4x4& m1, f32 s);
	Float4x4 operator+(f32 s, const Float4x4& m1);
	Float4x4 operator-(const Float4x4& m1, const Float4x4& m2);
	Float4x4 operator-(const Float4x4& m1, f32 s);
	Float4x4 operator-(f32 s, const Float4x4& m1);
	Float4x4 operator*(const Float4x4& m1, const Float4x4& m2);
	Float4x4 operator*(const Float4x4& m1, f32 s);
	Float4x4 operator*(f32 s, const Float4x4& m1);
	Float4x4 operator/(const Float4x4& m1, const Float4x4& m2);
	Float4x4 operator/(const Float4x4& m1, f32 s);
	Float4x4 operator/(f32 s, const Float4x4& m1);

	f32 determinant(const Float3x3& mat);
	f32 determinant(const Float4x4& mat);

	Float4 mul(const Float4& vec, const Float4x4& mat);
	Float4 mul(const Float4x4& mat, const Float4& vec);
	Float4x4 mul(const Float4x4& mat1, const Float4x4& mat2);

	Float3x3 transpose(const Float3x3& m);
	Float3x4 transpose(const Float4x3& m);
	Float4x3 transpose(const Float3x4& m);
	Float4x4 transpose(const Float4x4& m);

	Float3x3 inverse(const Float3x3& m, f32* out_determinant = nullptr);
	Float4x4 inverse(const Float4x4& m, f32* out_determinant = nullptr);

	//------------------------------------------------------------------------------------
	//
	//	Implementation part.
	//
	//------------------------------------------------------------------------------------

	inline bool Float3x3::operator == (const Float3x3& m) const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		vec_float_t y1 = load_float3a(&m._11);
		vec_float_t y2 = load_float3a(&m._21);
		vec_float_t y3 = load_float3a(&m._31);

		return (Vector3Equal(x1, y1) && Vector3Equal(x2, y2) && Vector3Equal(x3, y3));
#else
		return ((_11 == m._11) && (_12 == m._12) && (_13 == m._13) &&
			(_21 == m._21) && (_22 == m._22) && (_23 == m._23) &&
			(_31 == m._31) && (_32 == m._32) && (_33 == m._33));
#endif
	}
	inline bool Float3x3::operator != (const Float3x3& m) const
	{
		return !(*this == m);
	}

	inline bool Float4x3::operator == (const Float4x3& m) const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y1 = load_float3a(&m._11);
		vec_float_t y2 = load_float3a(&m._21);
		vec_float_t y3 = load_float3a(&m._31);
		vec_float_t y4 = load_float3a(&m._41);

		return (Vector3Equal(x1, y1) && Vector3Equal(x2, y2) && Vector3Equal(x3, y3)) && Vector3Equal(x4, y4);
#else
		return ((_11 == m._11) && (_12 == m._12) && (_13 == m._13) &&
			(_21 == m._21) && (_22 == m._22) && (_23 == m._23) &&
			(_31 == m._31) && (_32 == m._32) && (_33 == m._33) &&
			(_41 == m._41) && (_42 == m._42) && (_43 == m._43));
#endif
	}
	inline bool Float4x3::operator != (const Float4x3& m) const
	{
		return !(*this == m);
	}

	inline bool Float3x4::operator == (const Float3x4& m) const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y1 = load_float4a(&m._11);
		vec_float_t y2 = load_float4a(&m._21);
		vec_float_t y3 = load_float4a(&m._31);

		return (Vector4Equal(x1, y1) && Vector4Equal(x2, y2) && Vector4Equal(x3, y3));
#else
		return ((_11 == m._11) && (_12 == m._12) && (_13 == m._13) && (_14 == m._14) &&
			(_21 == m._21) && (_22 == m._22) && (_23 == m._23) && (_24 == m._24) &&
			(_31 == m._31) && (_32 == m._32) && (_33 == m._33) && (_34 == m._34));
#endif
	}
	inline bool Float3x4::operator != (const Float3x4& m) const
	{
		return !(*this == m);
	}

	inline bool Float4x4::operator == (const Float4x4& m) const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y1 = load_float4a(&m._11);
		vec_float_t y2 = load_float4a(&m._21);
		vec_float_t y3 = load_float4a(&m._31);
		vec_float_t y4 = load_float4a(&m._41);

		return (Vector4Equal(x1, y1) && Vector4Equal(x2, y2) && Vector4Equal(x3, y3) && Vector4Equal(x4, y4));
#else
		return ((_11 == m._11) && (_12 == m._12) && (_13 == m._13) && (_14 == m._14) &&
			(_21 == m._21) && (_22 == m._22) && (_23 == m._23) && (_24 == m._24) &&
			(_31 == m._31) && (_32 == m._32) && (_33 == m._33) && (_34 == m._34) &&
			(_41 == m._41) && (_42 == m._42) && (_43 == m._43) && (_44 == m._44));
#endif
	}
	inline bool Float4x4::operator != (const Float4x4& m) const
	{
		return !(*this == m);
	}

	inline Float3x3::Float3x3(const Float3& row1, const Float3& row2, const Float3& row3)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r1 = load_float3a(row1.m);
		vec_float_t r2 = load_float3a(row2.m);
		vec_float_t r3 = load_float3a(row3.m);
		store_float3a(&_11, r1);
		store_float3a(&_21, r2);
		store_float3a(&_31, r3);
#else
		_11 = row1.x;
		_12 = row1.y;
		_13 = row1.z;
		_21 = row2.x;
		_22 = row2.y;
		_23 = row2.z;
		_31 = row3.x;
		_32 = row3.y;
		_33 = row3.z;
#endif
	}

	inline Float4x3::Float4x3(const Float3& row1, const Float3& row2, const Float3& row3, const Float3& row4)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r1 = load_float3a(row1.m);
		vec_float_t r2 = load_float3a(row2.m);
		vec_float_t r3 = load_float3a(row3.m);
		vec_float_t r4 = load_float3a(row4.m);
		store_float3a(&_11, r1);
		store_float3a(&_21, r2);
		store_float3a(&_31, r3);
		store_float3a(&_41, r4);
#else
		_11 = row1.x;
		_12 = row1.y;
		_13 = row1.z;
		_21 = row2.x;
		_22 = row2.y;
		_23 = row2.z;
		_31 = row3.x;
		_32 = row3.y;
		_33 = row3.z;
		_41 = row4.x;
		_42 = row4.y;
		_43 = row4.z;
#endif
	}

	inline Float3x4::Float3x4(const Float4& row1, const Float4& row2, const Float4& row3)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r1 = load_float4a(row1.m);
		vec_float_t r2 = load_float4a(row2.m);
		vec_float_t r3 = load_float4a(row3.m);
		store_float4a(&_11, r1);
		store_float4a(&_21, r2);
		store_float4a(&_31, r3);
#else
		_11 = row1.x;
		_12 = row1.y;
		_13 = row1.z;
		_14 = row1.w;
		_21 = row2.x;
		_22 = row2.y;
		_23 = row2.z;
		_24 = row2.w;
		_31 = row3.x;
		_32 = row3.y;
		_33 = row3.z;
		_34 = row3.w;
#endif
	}

	inline Float4x4::Float4x4(const Float4& row1, const Float4& row2, const Float4& row3, const Float4& row4)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r1 = load_float4a(row1.m);
		vec_float_t r2 = load_float4a(row2.m);
		vec_float_t r3 = load_float4a(row3.m);
		vec_float_t r4 = load_float4a(row4.m);
		store_float4a(&_11, r1);
		store_float4a(&_21, r2);
		store_float4a(&_31, r3);
		store_float4a(&_41, r4);
#else
		_11 = row1.x;
		_12 = row1.y;
		_13 = row1.z;
		_14 = row1.w;
		_21 = row2.x;
		_22 = row2.y;
		_23 = row2.z;
		_24 = row2.w;
		_31 = row3.x;
		_32 = row3.y;
		_33 = row3.z;
		_34 = row3.w;
		_41 = row4.x;
		_42 = row4.y;
		_43 = row4.z;
		_44 = row4.w;
#endif
	}

	inline Float3x3 Float3x3::operator-() const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float3a(&_11);
		vec_float_t v2 = load_float3a(&_21);
		vec_float_t v3 = load_float3a(&_31);

		v1 = VectorNegate(v1);
		v2 = VectorNegate(v2);
		v3 = VectorNegate(v3);

		Float3x3 r;
		store_float3a(&(r._11), v1);
		store_float3a(&(r._21), v2);
		store_float3a(&(r._31), v3);
		return r;
#else
		Float3x3 r;
		r._11 = -_11;
		r._12 = -_12;
		r._13 = -_13;
		r._21 = -_21;
		r._22 = -_22;
		r._23 = -_23;
		r._31 = -_31;
		r._32 = -_32;
		r._33 = -_33;
		return r;
#endif
	}

	inline Float4x3 Float4x3::operator-() const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float3a(&_11);
		vec_float_t v2 = load_float3a(&_21);
		vec_float_t v3 = load_float3a(&_31);
		vec_float_t v4 = load_float3a(&_41);

		v1 = VectorNegate(v1);
		v2 = VectorNegate(v2);
		v3 = VectorNegate(v3);
		v4 = VectorNegate(v4);

		Float4x3 r;
		store_float3a(&(r._11), v1);
		store_float3a(&(r._21), v2);
		store_float3a(&(r._31), v3);
		store_float3a(&(r._41), v4);
		return r;
#else
		Float4x3 r;
		r._11 = -_11;
		r._12 = -_12;
		r._13 = -_13;
		r._21 = -_21;
		r._22 = -_22;
		r._23 = -_23;
		r._31 = -_31;
		r._32 = -_32;
		r._33 = -_33;
		r._41 = -_41;
		r._42 = -_42;
		r._43 = -_43;
		return r;
#endif
	}

	inline Float3x4 Float3x4::operator-() const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(&_11);
		vec_float_t v2 = load_float4a(&_21);
		vec_float_t v3 = load_float4a(&_31);

		v1 = VectorNegate(v1);
		v2 = VectorNegate(v2);
		v3 = VectorNegate(v3);

		Float3x4 r;
		store_float4a(&(r._11), v1);
		store_float4a(&(r._21), v2);
		store_float4a(&(r._31), v3);
		return r;
#else
		Float3x4 r;
		r._11 = -_11;
		r._12 = -_12;
		r._13 = -_13;
		r._14 = -_14;
		r._21 = -_21;
		r._22 = -_22;
		r._23 = -_23;
		r._24 = -_24;
		r._31 = -_31;
		r._32 = -_32;
		r._33 = -_33;
		r._34 = -_34;
		return r;
#endif
	}

	namespace impl
	{
		inline void make_eular_positive(Float3& euler)
		{
			const float l = -0.0001f;
			const float h = (pi * 2.0F) - 0.0001f;

			if (euler.x < l)
				euler.x += 2.0f * pi;
			else if (euler.x > h)
				euler.x -= 2.0f * pi;

			if (euler.y < l)
				euler.y += 2.0f * pi;
			else if (euler.y > h)
				euler.y -= 2.0f * pi;

			if (euler.z < l)
				euler.z += 2.0f * pi;
			else if (euler.z > h)
				euler.z -= 2.0f * pi;
		}
	}

	inline Float4x4 Float4x4::rotation_matrix() const
	{
		Float3 scale = scale_factor();
		return Float4x4(
			r1() / scale.x,
			r2() / scale.y,
			r3() / scale.z,
			Float4(0.0f, 0.0f, 0.0f, 1.0f)
		);
	}

	inline Float3 Float4x4::euler_angles() const
	{
		// Roll, Pitch, Yaw (ZXY).
		Float3 v;
		if (_32 < 0.999f)
		{
			if (_32 > -0.999f)
			{
				v.x = -asinf(_32);
				v.z = -atan2f(-_12, _22);
				v.y = -atan2f(-_31, _33);
			}
			else
			{
				v.x = pi / 2.0f;
				v.z = atan2f(_13, _11);
				v.y = 0.0f;
			}
		}
		else
		{
			v.x = -pi / 2.0f;
			v.z = -atan2f(_13, _11);
			v.y = 0.0f;
		}
		return v;
	}

	inline Quaternion Float4x4::quaternion() const
	{
		return Quaternion::from_euler_angles(euler_angles());
		/*Quaternion r;
		f32 t;
		if (_33 < 0.0f)
		{
			if (_11 > _22)
			{
				t = 1.0f + _11 - _22 - _33;
				r = Quaternion(t, _12 + _21, _31 + _13, _23 - _32);
			}
			else
			{
				t = 1.0f - _11 + _22 - _33;
				r = Quaternion(_12 + _21, t, _23 + _32, _31 - _13);
			}
		}
		else
		{
			if (_11 < -_22)
			{
				t = 1.0f - _11 - _22 + _33;
				r = Quaternion(_31 + _13, _23 + _32, t, _12 - _21);
			}
			else
			{
				t = 1.0f + _11 + _22 + _33;
				r = Quaternion(_23 - _32, _31 - _13, _12 - _21, t);
			}
		}
		r *= 0.5f / sqrtf(t);
		return r;*/
	}

	inline Float3 Float4x4::scale_factor() const
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t v1 = vector_set(_11, _21, _31, 0.0f);
		vec_float_t v2 = vector_set(_12, _22, _32, 0.0f);
		vec_float_t v3 = vector_set(_13, _23, _33, 0.0f);
		v1 = VectorMultiply(v1, v1);
		v2 = VectorMultiply(v2, v2);
		v3 = VectorMultiply(v3, v3);
		v1 = VectorAdd(v1, VectorAdd(v2, v3));
		v1 = VectorSqrt(v1);
		Float3 ret;
		store_float3a(ret.m, v1);
		return ret;
#else
		return Float3(
			sqrtf(_11 * _11 + _12 * _12 + _13 * _13),
			sqrtf(_21 * _21 + _22 * _22 + _23 * _23),
			sqrtf(_31 * _31 + _32 * _32 + _33 * _33)
		);
#endif
	}

	inline Float4x4 Float4x4::operator-() const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t v1 = load_float4a(&_11);
		vec_float_t v2 = load_float4a(&_21);
		vec_float_t v3 = load_float4a(&_31);
		vec_float_t v4 = load_float4a(&_41);

		v1 = VectorNegate(v1);
		v2 = VectorNegate(v2);
		v3 = VectorNegate(v3);
		v4 = VectorNegate(v4);

		Float4x4 r;
		store_float4a(&(r._11), v1);
		store_float4a(&(r._21), v2);
		store_float4a(&(r._31), v3);
		store_float4a(&(r._41), v4);
		return r;
#else
		Float4x4 r;
		r._11 = -_11;
		r._12 = -_12;
		r._13 = -_13;
		r._14 = -_14;
		r._21 = -_21;
		r._22 = -_22;
		r._23 = -_23;
		r._24 = -_24;
		r._31 = -_31;
		r._32 = -_32;
		r._33 = -_33;
		r._34 = -_34;
		r._41 = -_41;
		r._42 = -_42;
		r._43 = -_43;
		r._44 = -_44;
		return r;
#endif
	}

	inline Float3x3& Float3x3::operator+=(const Float3x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		
		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 += rhs._11;
		_12 += rhs._12;
		_13 += rhs._13;
		_21 += rhs._21;
		_22 += rhs._22;
		_23 += rhs._23;
		_31 += rhs._31;
		_32 += rhs._32;
		_33 += rhs._33;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator+=(const Float4x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));
		vec_float_t y4 = load_float3a(&(rhs._41));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);
		x4 = VectorAdd(x4, y4);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 += rhs._11;
		_12 += rhs._12;
		_13 += rhs._13;
		_21 += rhs._21;
		_22 += rhs._22;
		_23 += rhs._23;
		_31 += rhs._31;
		_32 += rhs._32;
		_33 += rhs._33;
		_41 += rhs._41;
		_42 += rhs._42;
		_43 += rhs._43;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator+=(const Float3x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 += rhs._11;
		_12 += rhs._12;
		_13 += rhs._13;
		_14 += rhs._14;
		_21 += rhs._21;
		_22 += rhs._22;
		_23 += rhs._23;
		_24 += rhs._24;
		_31 += rhs._31;
		_32 += rhs._32;
		_33 += rhs._33;
		_34 += rhs._34;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator+=(const Float4x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));
		vec_float_t y4 = load_float4a(&(rhs._41));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);
		x4 = VectorAdd(x4, y4);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 += rhs._11;
		_12 += rhs._12;
		_13 += rhs._13;
		_14 += rhs._14;
		_21 += rhs._21;
		_22 += rhs._22;
		_23 += rhs._23;
		_24 += rhs._24;
		_31 += rhs._31;
		_32 += rhs._32;
		_33 += rhs._33;
		_34 += rhs._34;
		_41 += rhs._41;
		_42 += rhs._42;
		_43 += rhs._43;
		_44 += rhs._44;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator-=(const Float3x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 -= rhs._11;
		_12 -= rhs._12;
		_13 -= rhs._13;
		_21 -= rhs._21;
		_22 -= rhs._22;
		_23 -= rhs._23;
		_31 -= rhs._31;
		_32 -= rhs._32;
		_33 -= rhs._33;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator-=(const Float4x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));
		vec_float_t y4 = load_float3a(&(rhs._41));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);
		x4 = VectorSubtract(x4, y4);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 -= rhs._11;
		_12 -= rhs._12;
		_13 -= rhs._13;
		_21 -= rhs._21;
		_22 -= rhs._22;
		_23 -= rhs._23;
		_31 -= rhs._31;
		_32 -= rhs._32;
		_33 -= rhs._33;
		_41 -= rhs._41;
		_42 -= rhs._42;
		_43 -= rhs._43;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator-=(const Float3x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 -= rhs._11;
		_12 -= rhs._12;
		_13 -= rhs._13;
		_14 -= rhs._14;
		_21 -= rhs._21;
		_22 -= rhs._22;
		_23 -= rhs._23;
		_24 -= rhs._24;
		_31 -= rhs._31;
		_32 -= rhs._32;
		_33 -= rhs._33;
		_34 -= rhs._34;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator-=(const Float4x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));
		vec_float_t y4 = load_float4a(&(rhs._41));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);
		x4 = VectorSubtract(x4, y4);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 -= rhs._11;
		_12 -= rhs._12;
		_13 -= rhs._13;
		_14 -= rhs._14;
		_21 -= rhs._21;
		_22 -= rhs._22;
		_23 -= rhs._23;
		_24 -= rhs._24;
		_31 -= rhs._31;
		_32 -= rhs._32;
		_33 -= rhs._33;
		_34 -= rhs._34;
		_41 -= rhs._41;
		_42 -= rhs._42;
		_43 -= rhs._43;
		_44 -= rhs._44;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator*=(const Float3x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 *= rhs._11;
		_12 *= rhs._12;
		_13 *= rhs._13;
		_21 *= rhs._21;
		_22 *= rhs._22;
		_23 *= rhs._23;
		_31 *= rhs._31;
		_32 *= rhs._32;
		_33 *= rhs._33;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator*=(const Float4x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));
		vec_float_t y4 = load_float3a(&(rhs._41));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);
		x4 = VectorMultiply(x4, y4);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 *= rhs._11;
		_12 *= rhs._12;
		_13 *= rhs._13;
		_21 *= rhs._21;
		_22 *= rhs._22;
		_23 *= rhs._23;
		_31 *= rhs._31;
		_32 *= rhs._32;
		_33 *= rhs._33;
		_41 *= rhs._41;
		_42 *= rhs._42;
		_43 *= rhs._43;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator*=(const Float3x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 *= rhs._11;
		_12 *= rhs._12;
		_13 *= rhs._13;
		_14 *= rhs._14;
		_21 *= rhs._21;
		_22 *= rhs._22;
		_23 *= rhs._23;
		_24 *= rhs._24;
		_31 *= rhs._31;
		_32 *= rhs._32;
		_33 *= rhs._33;
		_34 *= rhs._34;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator*=(const Float4x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));
		vec_float_t y4 = load_float4a(&(rhs._41));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);
		x4 = VectorMultiply(x4, y4);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 *= rhs._11;
		_12 *= rhs._12;
		_13 *= rhs._13;
		_14 *= rhs._14;
		_21 *= rhs._21;
		_22 *= rhs._22;
		_23 *= rhs._23;
		_24 *= rhs._24;
		_31 *= rhs._31;
		_32 *= rhs._32;
		_33 *= rhs._33;
		_34 *= rhs._34;
		_41 *= rhs._41;
		_42 *= rhs._42;
		_43 *= rhs._43;
		_44 *= rhs._44;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator/=(const Float3x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 /= rhs._11;
		_12 /= rhs._12;
		_13 /= rhs._13;
		_21 /= rhs._21;
		_22 /= rhs._22;
		_23 /= rhs._23;
		_31 /= rhs._31;
		_32 /= rhs._32;
		_33 /= rhs._33;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator/=(const Float4x3& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y1 = load_float3a(&(rhs._11));
		vec_float_t y2 = load_float3a(&(rhs._21));
		vec_float_t y3 = load_float3a(&(rhs._31));
		vec_float_t y4 = load_float3a(&(rhs._41));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);
		x4 = VectorDivide(x4, y4);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 /= rhs._11;
		_12 /= rhs._12;
		_13 /= rhs._13;
		_21 /= rhs._21;
		_22 /= rhs._22;
		_23 /= rhs._23;
		_31 /= rhs._31;
		_32 /= rhs._32;
		_33 /= rhs._33;
		_41 /= rhs._41;
		_42 /= rhs._42;
		_43 /= rhs._43;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator/=(const Float3x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 /= rhs._11;
		_12 /= rhs._12;
		_13 /= rhs._13;
		_14 /= rhs._14;
		_21 /= rhs._21;
		_22 /= rhs._22;
		_23 /= rhs._23;
		_24 /= rhs._24;
		_31 /= rhs._31;
		_32 /= rhs._32;
		_33 /= rhs._33;
		_34 /= rhs._34;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator/=(const Float4x4& rhs)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y1 = load_float4a(&(rhs._11));
		vec_float_t y2 = load_float4a(&(rhs._21));
		vec_float_t y3 = load_float4a(&(rhs._31));
		vec_float_t y4 = load_float4a(&(rhs._41));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);
		x4 = VectorDivide(x4, y4);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 /= rhs._11;
		_12 /= rhs._12;
		_13 /= rhs._13;
		_14 /= rhs._14;
		_21 /= rhs._21;
		_22 /= rhs._22;
		_23 /= rhs._23;
		_24 /= rhs._24;
		_31 /= rhs._31;
		_32 /= rhs._32;
		_33 /= rhs._33;
		_34 /= rhs._34;
		_41 /= rhs._41;
		_42 /= rhs._42;
		_43 /= rhs._43;
		_44 /= rhs._44;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 += s;
		_12 += s;
		_13 += s;
		_21 += s;
		_22 += s;
		_23 += s;
		_31 += s;
		_32 += s;
		_33 += s;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);
		x4 = VectorAdd(x4, y);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 += s;
		_12 += s;
		_13 += s;
		_21 += s;
		_22 += s;
		_23 += s;
		_31 += s;
		_32 += s;
		_33 += s;
		_41 += s;
		_42 += s;
		_43 += s;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 += s;
		_12 += s;
		_13 += s;
		_14 += s;
		_21 += s;
		_22 += s;
		_23 += s;
		_24 += s;
		_31 += s;
		_32 += s;
		_33 += s;
		_34 += s;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator+=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);
		x4 = VectorAdd(x4, y);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 += s;
		_12 += s;
		_13 += s;
		_14 += s;
		_21 += s;
		_22 += s;
		_23 += s;
		_24 += s;
		_31 += s;
		_32 += s;
		_33 += s;
		_34 += s;
		_41 += s;
		_42 += s;
		_43 += s;
		_44 += s;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 -= s;
		_12 -= s;
		_13 -= s;
		_21 -= s;
		_22 -= s;
		_23 -= s;
		_31 -= s;
		_32 -= s;
		_33 -= s;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);
		x4 = VectorSubtract(x4, y);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 -= s;
		_12 -= s;
		_13 -= s;
		_21 -= s;
		_22 -= s;
		_23 -= s;
		_31 -= s;
		_32 -= s;
		_33 -= s;
		_41 -= s;
		_42 -= s;
		_43 -= s;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 -= s;
		_12 -= s;
		_13 -= s;
		_14 -= s;
		_21 -= s;
		_22 -= s;
		_23 -= s;
		_24 -= s;
		_31 -= s;
		_32 -= s;
		_33 -= s;
		_34 -= s;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator-=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);
		x4 = VectorSubtract(x4, y);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 -= s;
		_12 -= s;
		_13 -= s;
		_14 -= s;
		_21 -= s;
		_22 -= s;
		_23 -= s;
		_24 -= s;
		_31 -= s;
		_32 -= s;
		_33 -= s;
		_34 -= s;
		_41 -= s;
		_42 -= s;
		_43 -= s;
		_44 -= s;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator*=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		_11 *= s;
		_12 *= s;
		_13 *= s;
		_21 *= s;
		_22 *= s;
		_23 *= s;
		_31 *= s;
		_32 *= s;
		_33 *= s;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator*=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 *= s;
		_12 *= s;
		_13 *= s;
		_21 *= s;
		_22 *= s;
		_23 *= s;
		_31 *= s;
		_32 *= s;
		_33 *= s;
		_41 *= s;
		_42 *= s;
		_43 *= s;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator*=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 *= s;
		_12 *= s;
		_13 *= s;
		_14 *= s;
		_21 *= s;
		_22 *= s;
		_23 *= s;
		_24 *= s;
		_31 *= s;
		_32 *= s;
		_33 *= s;
		_34 *= s;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator*=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 *= s;
		_12 *= s;
		_13 *= s;
		_14 *= s;
		_21 *= s;
		_22 *= s;
		_23 *= s;
		_24 *= s;
		_31 *= s;
		_32 *= s;
		_33 *= s;
		_34 *= s;
		_41 *= s;
		_42 *= s;
		_43 *= s;
		_44 *= s;
		return *this;
#endif
	}

	inline Float3x3& Float3x3::operator/=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);

		f32 rs = 1.0f / s;

		x1 = VectorScale(x1, rs);
		x2 = VectorScale(x2, rs);
		x3 = VectorScale(x3, rs);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		return *this;
#else
		s = 1.0f / s;
		_11 *= s;
		_12 *= s;
		_13 *= s;
		_21 *= s;
		_22 *= s;
		_23 *= s;
		_31 *= s;
		_32 *= s;
		_33 *= s;
		return *this;
#endif
	}

	inline Float4x3& Float4x3::operator/=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&_11);
		vec_float_t x2 = load_float3a(&_21);
		vec_float_t x3 = load_float3a(&_31);
		vec_float_t x4 = load_float3a(&_41);

		s = 1.0f / s;
		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		store_float3a(&_11, x1);
		store_float3a(&_21, x2);
		store_float3a(&_31, x3);
		store_float3a(&_41, x4);
		return *this;
#else
		_11 /= s;
		_12 /= s;
		_13 /= s;
		_21 /= s;
		_22 /= s;
		_23 /= s;
		_31 /= s;
		_32 /= s;
		_33 /= s;
		_41 /= s;
		_42 /= s;
		_43 /= s;
		return *this;
#endif
	}

	inline Float3x4& Float3x4::operator/=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);

		s = 1.0f / s;
		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		return *this;
#else
		_11 /= s;
		_12 /= s;
		_13 /= s;
		_14 /= s;
		_21 /= s;
		_22 /= s;
		_23 /= s;
		_24 /= s;
		_31 /= s;
		_32 /= s;
		_33 /= s;
		_34 /= s;
		return *this;
#endif
	}

	inline Float4x4& Float4x4::operator/=(f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&_11);
		vec_float_t x2 = load_float4a(&_21);
		vec_float_t x3 = load_float4a(&_31);
		vec_float_t x4 = load_float4a(&_41);

		s = 1.0f / s;
		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		store_float4a(&_11, x1);
		store_float4a(&_21, x2);
		store_float4a(&_31, x3);
		store_float4a(&_41, x4);
		return *this;
#else
		_11 /= s;
		_12 /= s;
		_13 /= s;
		_14 /= s;
		_21 /= s;
		_22 /= s;
		_23 /= s;
		_24 /= s;
		_31 /= s;
		_32 /= s;
		_33 /= s;
		_34 /= s;
		_41 /= s;
		_42 /= s;
		_43 /= s;
		_44 /= s;
		return *this;
#endif
	}


	inline Float3x3 operator+(const Float3x3& m1, const Float3x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);

		Float3x3 result;
		store_float3a(&(result._11), x1);
		store_float3a(&(result._21), x2);
		store_float3a(&(result._31), x3);
		return result;
#else
		Float3x3 result;
		result._11 = m1._11 + m2._11;
		result._12 = m1._12 + m2._12;
		result._13 = m1._13 + m2._13;
		result._21 = m1._21 + m2._21;
		result._22 = m1._22 + m2._22;
		result._23 = m1._23 + m2._23;
		result._31 = m1._31 + m2._31;
		result._32 = m1._32 + m2._32;
		result._33 = m1._33 + m2._33;
		return result;
#endif
	}

	inline Float4x3 operator+(const Float4x3& m1, const Float4x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));
		vec_float_t y4 = load_float3a(&(m2._41));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);
		x4 = VectorAdd(x4, y4);

		Float4x3 result;
		store_float3a(&(result._11), x1);
		store_float3a(&(result._21), x2);
		store_float3a(&(result._31), x3);
		store_float3a(&(result._41), x4);
		return result;
#else
		Float4x3 result;
		result._11 = m1._11 + m2._11;
		result._12 = m1._12 + m2._12;
		result._13 = m1._13 + m2._13;
		result._21 = m1._21 + m2._21;
		result._22 = m1._22 + m2._22;
		result._23 = m1._23 + m2._23;
		result._31 = m1._31 + m2._31;
		result._32 = m1._32 + m2._32;
		result._33 = m1._33 + m2._33;
		result._41 = m1._41 + m2._41;
		result._42 = m1._42 + m2._42;
		result._43 = m1._43 + m2._43;
		return result;
#endif
	}

	inline Float3x4 operator+(const Float3x4& m1, const Float3x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);

		Float3x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		return result;
#else
		Float3x4 result;
		result._11 = m1._11 + m2._11;
		result._12 = m1._12 + m2._12;
		result._13 = m1._13 + m2._13;
		result._14 = m1._14 + m2._14;
		result._21 = m1._21 + m2._21;
		result._22 = m1._22 + m2._22;
		result._23 = m1._23 + m2._23;
		result._24 = m1._24 + m2._24;
		result._31 = m1._31 + m2._31;
		result._32 = m1._32 + m2._32;
		result._33 = m1._33 + m2._33;
		result._34 = m1._34 + m2._34;
		return result;
#endif
	}

	inline Float4x4 operator+(const Float4x4& m1, const Float4x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));
		vec_float_t y4 = load_float4a(&(m2._41));

		x1 = VectorAdd(x1, y1);
		x2 = VectorAdd(x2, y2);
		x3 = VectorAdd(x3, y3);
		x4 = VectorAdd(x4, y4);

		Float4x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		store_float4a(&(result._41), x4);
		return result;
#else
		Float4x4 result;
		result._11 = m1._11 + m2._11;
		result._12 = m1._12 + m2._12;
		result._13 = m1._13 + m2._13;
		result._14 = m1._14 + m2._14;
		result._21 = m1._21 + m2._21;
		result._22 = m1._22 + m2._22;
		result._23 = m1._23 + m2._23;
		result._24 = m1._24 + m2._24;
		result._31 = m1._31 + m2._31;
		result._32 = m1._32 + m2._32;
		result._33 = m1._33 + m2._33;
		result._34 = m1._34 + m2._34;
		result._41 = m1._41 + m2._41;
		result._42 = m1._42 + m2._42;
		result._43 = m1._43 + m2._43;
		result._44 = m1._44 + m2._44;
		return result;
#endif
	}

	inline Float3x3 operator+(const Float3x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = m1._11 + s;
		result._12 = m1._12 + s;
		result._13 = m1._13 + s;
		result._21 = m1._21 + s;
		result._22 = m1._22 + s;
		result._23 = m1._23 + s;
		result._31 = m1._31 + s;
		result._32 = m1._32 + s;
		result._33 = m1._33 + s;
		return result;
#endif
	}

	inline Float4x3 operator+(const Float4x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);
		x4 = VectorAdd(x4, y);

		Float4x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		store_float3a(&(r._41), x4);
		return r;
#else
		Float4x3 result;
		result._11 = m1._11 + s;
		result._12 = m1._12 + s;
		result._13 = m1._13 + s;
		result._21 = m1._21 + s;
		result._22 = m1._22 + s;
		result._23 = m1._23 + s;
		result._31 = m1._31 + s;
		result._32 = m1._32 + s;
		result._33 = m1._33 + s;
		result._41 = m1._41 + s;
		result._42 = m1._42 + s;
		result._43 = m1._43 + s;
		return result;
#endif
	}

	inline Float3x4 operator+(const Float3x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);

		Float3x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		return r;
#else
		Float3x4 result;
		result._11 = m1._11 + s;
		result._12 = m1._12 + s;
		result._13 = m1._13 + s;
		result._14 = m1._14 + s;
		result._21 = m1._21 + s;
		result._22 = m1._22 + s;
		result._23 = m1._23 + s;
		result._24 = m1._24 + s;
		result._31 = m1._31 + s;
		result._32 = m1._32 + s;
		result._33 = m1._33 + s;
		result._34 = m1._34 + s;
		return result;
#endif
	}

	inline Float4x4 operator+(const Float4x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorAdd(x1, y);
		x2 = VectorAdd(x2, y);
		x3 = VectorAdd(x3, y);
		x4 = VectorAdd(x4, y);

		Float4x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		store_float4a(&(r._41), x4);
		return r;
#else
		Float4x4 result;
		result._11 = m1._11 + s;
		result._12 = m1._12 + s;
		result._13 = m1._13 + s;
		result._14 = m1._14 + s;
		result._21 = m1._21 + s;
		result._22 = m1._22 + s;
		result._23 = m1._23 + s;
		result._24 = m1._24 + s;
		result._31 = m1._31 + s;
		result._32 = m1._32 + s;
		result._33 = m1._33 + s;
		result._34 = m1._34 + s;
		result._41 = m1._41 + s;
		result._42 = m1._42 + s;
		result._43 = m1._43 + s;
		result._44 = m1._44 + s;
		return result;
#endif
	}

	inline Float3x3 operator+(f32 s, const Float3x3& m1)
	{
		return m1 + s;
	}

	inline Float4x3 operator+(f32 s, const Float4x3& m1)
	{
		return m1 + s;
	}

	inline Float3x4 operator+(f32 s, const Float3x4& m1)
	{
		return m1 + s;
	}

	inline Float4x4 operator+(f32 s, const Float4x4& m1)
	{
		return m1 + s;
	}

	inline Float3x3 operator-(const Float3x3& m1, const Float3x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);

		Float3x3 result;
		store_float3a(&(result._11), x1);
		store_float3a(&(result._21), x2);
		store_float3a(&(result._31), x3);
		return result;
#else
		Float3x3 result;
		result._11 = m1._11 - m2._11;
		result._12 = m1._12 - m2._12;
		result._13 = m1._13 - m2._13;
		result._21 = m1._21 - m2._21;
		result._22 = m1._22 - m2._22;
		result._23 = m1._23 - m2._23;
		result._31 = m1._31 - m2._31;
		result._32 = m1._32 - m2._32;
		result._33 = m1._33 - m2._33;
		return result;
#endif
	}

	inline Float4x3 operator-(const Float4x3& m1, const Float4x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));
		vec_float_t y4 = load_float3a(&(m2._41));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);
		x4 = VectorSubtract(x4, y4);

		Float4x3 result;
		store_float3a(&(result._11), x1);
		store_float3a(&(result._21), x2);
		store_float3a(&(result._31), x3);
		store_float3a(&(result._41), x4);
		return result;
#else
		Float4x3 result;
		result._11 = m1._11 - m2._11;
		result._12 = m1._12 - m2._12;
		result._13 = m1._13 - m2._13;
		result._21 = m1._21 - m2._21;
		result._22 = m1._22 - m2._22;
		result._23 = m1._23 - m2._23;
		result._31 = m1._31 - m2._31;
		result._32 = m1._32 - m2._32;
		result._33 = m1._33 - m2._33;
		result._41 = m1._41 - m2._41;
		result._42 = m1._42 - m2._42;
		result._43 = m1._43 - m2._43;
		return result;
#endif
	}

	inline Float3x4 operator-(const Float3x4& m1, const Float3x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);

		Float3x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		return result;
#else
		Float3x4 result;
		result._11 = m1._11 - m2._11;
		result._12 = m1._12 - m2._12;
		result._13 = m1._13 - m2._13;
		result._14 = m1._14 - m2._14;
		result._21 = m1._21 - m2._21;
		result._22 = m1._22 - m2._22;
		result._23 = m1._23 - m2._23;
		result._24 = m1._24 - m2._24;
		result._31 = m1._31 - m2._31;
		result._32 = m1._32 - m2._32;
		result._33 = m1._33 - m2._33;
		result._34 = m1._34 - m2._34;
		return result;
#endif
	}

	inline Float4x4 operator-(const Float4x4& m1, const Float4x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));
		vec_float_t y4 = load_float4a(&(m2._41));

		x1 = VectorSubtract(x1, y1);
		x2 = VectorSubtract(x2, y2);
		x3 = VectorSubtract(x3, y3);
		x4 = VectorSubtract(x4, y4);

		Float4x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		store_float4a(&(result._41), x4);
		return result;
#else
		Float4x4 result;
		result._11 = m1._11 - m2._11;
		result._12 = m1._12 - m2._12;
		result._13 = m1._13 - m2._13;
		result._14 = m1._14 - m2._14;
		result._21 = m1._21 - m2._21;
		result._22 = m1._22 - m2._22;
		result._23 = m1._23 - m2._23;
		result._24 = m1._24 - m2._24;
		result._31 = m1._31 - m2._31;
		result._32 = m1._32 - m2._32;
		result._33 = m1._33 - m2._33;
		result._34 = m1._34 - m2._34;
		result._41 = m1._41 - m2._41;
		result._42 = m1._42 - m2._42;
		result._43 = m1._43 - m2._43;
		result._44 = m1._44 - m2._44;
		return result;
#endif
	}

	inline Float3x3 operator-(const Float3x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = m1._11 - s;
		result._12 = m1._12 - s;
		result._13 = m1._13 - s;
		result._21 = m1._21 - s;
		result._22 = m1._22 - s;
		result._23 = m1._23 - s;
		result._31 = m1._31 - s;
		result._32 = m1._32 - s;
		result._33 = m1._33 - s;
		return result;
#endif
	}

	inline Float4x3 operator-(const Float4x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);
		x4 = VectorSubtract(x4, y);

		Float4x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		store_float3a(&(r._41), x4);
		return r;
#else
		Float4x3 result;
		result._11 = m1._11 - s;
		result._12 = m1._12 - s;
		result._13 = m1._13 - s;
		result._21 = m1._21 - s;
		result._22 = m1._22 - s;
		result._23 = m1._23 - s;
		result._31 = m1._31 - s;
		result._32 = m1._32 - s;
		result._33 = m1._33 - s;
		result._41 = m1._41 - s;
		result._42 = m1._42 - s;
		result._43 = m1._43 - s;
		return result;
#endif
	}

	inline Float3x4 operator-(const Float3x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);

		Float3x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		return r;
#else
		Float3x4 result;
		result._11 = m1._11 - s;
		result._12 = m1._12 - s;
		result._13 = m1._13 - s;
		result._14 = m1._14 - s;
		result._21 = m1._21 - s;
		result._22 = m1._22 - s;
		result._23 = m1._23 - s;
		result._24 = m1._24 - s;
		result._31 = m1._31 - s;
		result._32 = m1._32 - s;
		result._33 = m1._33 - s;
		result._34 = m1._34 - s;
		return result;
#endif
	}

	inline Float4x4 operator-(const Float4x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(x1, y);
		x2 = VectorSubtract(x2, y);
		x3 = VectorSubtract(x3, y);
		x4 = VectorSubtract(x4, y);

		Float4x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		store_float4a(&(r._41), x4);
		return r;
#else
		Float4x4 result;
		result._11 = m1._11 - s;
		result._12 = m1._12 - s;
		result._13 = m1._13 - s;
		result._14 = m1._14 - s;
		result._21 = m1._21 - s;
		result._22 = m1._22 - s;
		result._23 = m1._23 - s;
		result._24 = m1._24 - s;
		result._31 = m1._31 - s;
		result._32 = m1._32 - s;
		result._33 = m1._33 - s;
		result._34 = m1._34 - s;
		result._41 = m1._41 - s;
		result._42 = m1._42 - s;
		result._43 = m1._43 - s;
		result._44 = m1._44 - s;
		return result;
#endif
	}

	inline Float3x3 operator-(f32 s, const Float3x3& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(y, x1);
		x2 = VectorSubtract(y, x2);
		x3 = VectorSubtract(y, x3);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = s - m1._11;
		result._12 = s - m1._12;
		result._13 = s - m1._13;
		result._21 = s - m1._21;
		result._22 = s - m1._22;
		result._23 = s - m1._23;
		result._31 = s - m1._31;
		result._32 = s - m1._32;
		result._33 = s - m1._33;
		return result;
#endif
	}

	inline Float4x3 operator-(f32 s, const Float4x3& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(y, x1);
		x2 = VectorSubtract(y, x2);
		x3 = VectorSubtract(y, x3);
		x4 = VectorSubtract(y, x4);

		Float4x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		store_float3a(&(r._41), x4);
		return r;
#else
		Float4x3 result;
		result._11 = s - m1._11;
		result._12 = s - m1._12;
		result._13 = s - m1._13;
		result._21 = s - m1._21;
		result._22 = s - m1._22;
		result._23 = s - m1._23;
		result._31 = s - m1._31;
		result._32 = s - m1._32;
		result._33 = s - m1._33;
		result._41 = s - m1._41;
		result._42 = s - m1._42;
		result._43 = s - m1._43;
		return result;
#endif
	}

	inline Float3x4 operator-(f32 s, const Float3x4& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(y, x1);
		x2 = VectorSubtract(y, x2);
		x3 = VectorSubtract(y, x3);

		Float3x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		return r;
#else
		Float3x4 result;
		result._11 = s - m1._11;
		result._12 = s - m1._12;
		result._13 = s - m1._13;
		result._14 = s - m1._14;
		result._21 = s - m1._21;
		result._22 = s - m1._22;
		result._23 = s - m1._23;
		result._24 = s - m1._24;
		result._31 = s - m1._31;
		result._32 = s - m1._32;
		result._33 = s - m1._33;
		result._34 = s - m1._34;
		return result;
#endif
	}

	inline Float4x4 operator-(f32 s, const Float4x4& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorSubtract(y, x1);
		x2 = VectorSubtract(y, x2);
		x3 = VectorSubtract(y, x3);
		x4 = VectorSubtract(y, x4);

		Float4x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		store_float4a(&(r._41), x4);
		return r;
#else
		Float4x4 result;
		result._11 = s - m1._11;
		result._12 = s - m1._12;
		result._13 = s - m1._13;
		result._14 = s - m1._14;
		result._21 = s - m1._21;
		result._22 = s - m1._22;
		result._23 = s - m1._23;
		result._24 = s - m1._24;
		result._31 = s - m1._31;
		result._32 = s - m1._32;
		result._33 = s - m1._33;
		result._34 = s - m1._34;
		result._41 = s - m1._41;
		result._42 = s - m1._42;
		result._43 = s - m1._43;
		result._44 = s - m1._44;
		return result;
#endif
	}

	inline Float3x3 operator*(const Float3x3& m1, const Float3x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = m1._11 * m2._11;
		result._12 = m1._12 * m2._12;
		result._13 = m1._13 * m2._13;
		result._21 = m1._21 * m2._21;
		result._22 = m1._22 * m2._22;
		result._23 = m1._23 * m2._23;
		result._31 = m1._31 * m2._31;
		result._32 = m1._32 * m2._32;
		result._33 = m1._33 * m2._33;
		return result;
#endif
	}

	inline Float4x3 operator*(const Float4x3& m1, const Float4x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));
		vec_float_t y4 = load_float3a(&(m2._41));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);
		x4 = VectorMultiply(x4, y4);

		Float4x3 result;
		store_float3a(&(result._11), x1);
		store_float3a(&(result._21), x2);
		store_float3a(&(result._31), x3);
		store_float3a(&(result._41), x4);
		return result;
#else
		Float4x3 result;
		result._11 = m1._11 * m2._11;
		result._12 = m1._12 * m2._12;
		result._13 = m1._13 * m2._13;
		result._21 = m1._21 * m2._21;
		result._22 = m1._22 * m2._22;
		result._23 = m1._23 * m2._23;
		result._31 = m1._31 * m2._31;
		result._32 = m1._32 * m2._32;
		result._33 = m1._33 * m2._33;
		result._41 = m1._41 * m2._41;
		result._42 = m1._42 * m2._42;
		result._43 = m1._43 * m2._43;
		return result;
#endif
	}

	inline Float3x4 operator*(const Float3x4& m1, const Float3x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);

		Float3x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		return result;
#else
		Float3x4 result;
		result._11 = m1._11 * m2._11;
		result._12 = m1._12 * m2._12;
		result._13 = m1._13 * m2._13;
		result._14 = m1._14 * m2._14;
		result._21 = m1._21 * m2._21;
		result._22 = m1._22 * m2._22;
		result._23 = m1._23 * m2._23;
		result._24 = m1._24 * m2._24;
		result._31 = m1._31 * m2._31;
		result._32 = m1._32 * m2._32;
		result._33 = m1._33 * m2._33;
		result._34 = m1._34 * m2._34;
		return result;
#endif
	}

	inline Float4x4 operator*(const Float4x4& m1, const Float4x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));
		vec_float_t y4 = load_float4a(&(m2._41));

		x1 = VectorMultiply(x1, y1);
		x2 = VectorMultiply(x2, y2);
		x3 = VectorMultiply(x3, y3);
		x4 = VectorMultiply(x4, y4);

		Float4x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		store_float4a(&(result._41), x4);
		return result;
#else
		Float4x4 result;
		result._11 = m1._11 * m2._11;
		result._12 = m1._12 * m2._12;
		result._13 = m1._13 * m2._13;
		result._14 = m1._14 * m2._14;
		result._21 = m1._21 * m2._21;
		result._22 = m1._22 * m2._22;
		result._23 = m1._23 * m2._23;
		result._24 = m1._24 * m2._24;
		result._31 = m1._31 * m2._31;
		result._32 = m1._32 * m2._32;
		result._33 = m1._33 * m2._33;
		result._34 = m1._34 * m2._34;
		result._41 = m1._41 * m2._41;
		result._42 = m1._42 * m2._42;
		result._43 = m1._43 * m2._43;
		result._44 = m1._44 * m2._44;
		return result;
#endif
	}

	inline Float3x3 operator*(const Float3x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = m1._11 * s;
		result._12 = m1._12 * s;
		result._13 = m1._13 * s;
		result._21 = m1._21 * s;
		result._22 = m1._22 * s;
		result._23 = m1._23 * s;
		result._31 = m1._31 * s;
		result._32 = m1._32 * s;
		result._33 = m1._33 * s;
		return result;
#endif
	}

	inline Float4x3 operator*(const Float4x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		Float4x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		store_float3a(&(r._41), x4);
		return r;
#else
		Float4x3 result;
		result._11 = m1._11 * s;
		result._12 = m1._12 * s;
		result._13 = m1._13 * s;
		result._21 = m1._21 * s;
		result._22 = m1._22 * s;
		result._23 = m1._23 * s;
		result._31 = m1._31 * s;
		result._32 = m1._32 * s;
		result._33 = m1._33 * s;
		result._41 = m1._41 * s;
		result._42 = m1._42 * s;
		result._43 = m1._43 * s;
		return result;
#endif
	}

	inline Float3x4 operator*(const Float3x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);

		Float3x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		return r;
#else
		Float3x4 result;
		result._11 = m1._11 * s;
		result._12 = m1._12 * s;
		result._13 = m1._13 * s;
		result._14 = m1._14 * s;
		result._21 = m1._21 * s;
		result._22 = m1._22 * s;
		result._23 = m1._23 * s;
		result._24 = m1._24 * s;
		result._31 = m1._31 * s;
		result._32 = m1._32 * s;
		result._33 = m1._33 * s;
		result._34 = m1._34 * s;
		return result;
#endif
	}

	inline Float4x4 operator*(const Float4x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		Float4x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		store_float4a(&(r._41), x4);
		return r;
#else
		Float4x4 result;
		result._11 = m1._11 * s;
		result._12 = m1._12 * s;
		result._13 = m1._13 * s;
		result._14 = m1._14 * s;
		result._21 = m1._21 * s;
		result._22 = m1._22 * s;
		result._23 = m1._23 * s;
		result._24 = m1._24 * s;
		result._31 = m1._31 * s;
		result._32 = m1._32 * s;
		result._33 = m1._33 * s;
		result._34 = m1._34 * s;
		result._41 = m1._41 * s;
		result._42 = m1._42 * s;
		result._43 = m1._43 * s;
		result._44 = m1._44 * s;
		return result;
#endif
	}

	inline Float3x3 operator*(f32 s, const Float3x3& m1)
	{
		return m1 * s;
	}

	inline Float4x3 operator*(f32 s, const Float4x3& m1)
	{
		return m1 * s;
	}

	inline Float3x4 operator*(f32 s, const Float3x4& m1)
	{
		return m1 * s;
	}

	inline Float4x4 operator*(f32 s, const Float4x4& m1)
	{
		return m1 * s;
	}

	inline Float3x3 operator/(const Float3x3& m1, const Float3x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else

		Float3x3 result;
		result._11 = m1._11 / m2._11;
		result._12 = m1._12 / m2._12;
		result._13 = m1._13 / m2._13;
		result._21 = m1._21 / m2._21;
		result._22 = m1._22 / m2._22;
		result._23 = m1._23 / m2._23;
		result._31 = m1._31 / m2._31;
		result._32 = m1._32 / m2._32;
		result._33 = m1._33 / m2._33;
		return result;
#endif
	}

	inline Float4x3 operator/(const Float4x3& m1, const Float4x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y1 = load_float3a(&(m2._11));
		vec_float_t y2 = load_float3a(&(m2._21));
		vec_float_t y3 = load_float3a(&(m2._31));
		vec_float_t y4 = load_float3a(&(m2._41));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);
		x4 = VectorDivide(x4, y4);

		Float4x3 result;
		store_float3a(&(result._11), x1);
		store_float3a(&(result._21), x2);
		store_float3a(&(result._31), x3);
		store_float3a(&(result._41), x4);
		return result;
#else
		Float4x3 result;
		result._11 = m1._11 / m2._11;
		result._12 = m1._12 / m2._12;
		result._13 = m1._13 / m2._13;
		result._21 = m1._21 / m2._21;
		result._22 = m1._22 / m2._22;
		result._23 = m1._23 / m2._23;
		result._31 = m1._31 / m2._31;
		result._32 = m1._32 / m2._32;
		result._33 = m1._33 / m2._33;
		result._41 = m1._41 / m2._41;
		result._42 = m1._42 / m2._42;
		result._43 = m1._43 / m2._43;
		return result;
#endif
	}

	inline Float3x4 operator/(const Float3x4& m1, const Float3x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);

		Float3x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		return result;
#else
		Float3x4 result;
		result._11 = m1._11 / m2._11;
		result._12 = m1._12 / m2._12;
		result._13 = m1._13 / m2._13;
		result._14 = m1._14 / m2._14;
		result._21 = m1._21 / m2._21;
		result._22 = m1._22 / m2._22;
		result._23 = m1._23 / m2._23;
		result._24 = m1._24 / m2._24;
		result._31 = m1._31 / m2._31;
		result._32 = m1._32 / m2._32;
		result._33 = m1._33 / m2._33;
		result._34 = m1._34 / m2._34;
		return result;
#endif
	}

	inline Float4x4 operator/(const Float4x4& m1, const Float4x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y1 = load_float4a(&(m2._11));
		vec_float_t y2 = load_float4a(&(m2._21));
		vec_float_t y3 = load_float4a(&(m2._31));
		vec_float_t y4 = load_float4a(&(m2._41));

		x1 = VectorDivide(x1, y1);
		x2 = VectorDivide(x2, y2);
		x3 = VectorDivide(x3, y3);
		x4 = VectorDivide(x4, y4);

		Float4x4 result;
		store_float4a(&(result._11), x1);
		store_float4a(&(result._21), x2);
		store_float4a(&(result._31), x3);
		store_float4a(&(result._41), x4);
		return result;
#else
		Float4x4 result;
		result._11 = m1._11 / m2._11;
		result._12 = m1._12 / m2._12;
		result._13 = m1._13 / m2._13;
		result._14 = m1._14 / m2._14;
		result._21 = m1._21 / m2._21;
		result._22 = m1._22 / m2._22;
		result._23 = m1._23 / m2._23;
		result._24 = m1._24 / m2._24;
		result._31 = m1._31 / m2._31;
		result._32 = m1._32 / m2._32;
		result._33 = m1._33 / m2._33;
		result._34 = m1._34 / m2._34;
		result._41 = m1._41 / m2._41;
		result._42 = m1._42 / m2._42;
		result._43 = m1._43 / m2._43;
		result._44 = m1._44 / m2._44;
		return result;
#endif
	}

	inline Float3x3 operator/(const Float3x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		f32 rs = 1.0f / s;

		x1 = VectorScale(x1, rs);
		x2 = VectorScale(x2, rs);
		x3 = VectorScale(x3, rs);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = m1._11 / s;
		result._12 = m1._12 / s;
		result._13 = m1._13 / s;
		result._21 = m1._21 / s;
		result._22 = m1._22 / s;
		result._23 = m1._23 / s;
		result._31 = m1._31 / s;
		result._32 = m1._32 / s;
		result._33 = m1._33 / s;
		return result;
#endif
	}

	inline Float4x3 operator/(const Float4x3& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		s = 1.0f / s;

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		Float4x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		store_float3a(&(r._41), x4);
		return r;
#else
		Float4x3 result;
		result._11 = m1._11 / s;
		result._12 = m1._12 / s;
		result._13 = m1._13 / s;
		result._21 = m1._21 / s;
		result._22 = m1._22 / s;
		result._23 = m1._23 / s;
		result._31 = m1._31 / s;
		result._32 = m1._32 / s;
		result._33 = m1._33 / s;
		result._41 = m1._41 / s;
		result._42 = m1._42 / s;
		result._43 = m1._43 / s;
		return result;
#endif
	}

	inline Float3x4 operator/(const Float3x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		s = 1.0f / s;

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);

		Float3x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		return r;
#else
		Float3x4 result;
		result._11 = m1._11 / s;
		result._12 = m1._12 / s;
		result._13 = m1._13 / s;
		result._14 = m1._14 / s;
		result._21 = m1._21 / s;
		result._22 = m1._22 / s;
		result._23 = m1._23 / s;
		result._24 = m1._24 / s;
		result._31 = m1._31 / s;
		result._32 = m1._32 / s;
		result._33 = m1._33 / s;
		result._34 = m1._34 / s;
		return result;
#endif
	}

	inline Float4x4 operator/(const Float4x4& m1, f32 s)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		s = 1.0f / s;

		x1 = VectorScale(x1, s);
		x2 = VectorScale(x2, s);
		x3 = VectorScale(x3, s);
		x4 = VectorScale(x4, s);

		Float4x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		store_float4a(&(r._41), x4);
		return r;
#else
		Float4x4 result;
		result._11 = m1._11 / s;
		result._12 = m1._12 / s;
		result._13 = m1._13 / s;
		result._14 = m1._14 / s;
		result._21 = m1._21 / s;
		result._22 = m1._22 / s;
		result._23 = m1._23 / s;
		result._24 = m1._24 / s;
		result._31 = m1._31 / s;
		result._32 = m1._32 / s;
		result._33 = m1._33 / s;
		result._34 = m1._34 / s;
		result._41 = m1._41 / s;
		result._42 = m1._42 / s;
		result._43 = m1._43 / s;
		result._44 = m1._44 / s;
		return result;
#endif
	}

	inline Float3x3 operator/(f32 s, const Float3x3& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));

		vec_float_t sv = vector_replicate(s);

		x1 = VectorDivide(sv, x1);
		x2 = VectorDivide(sv, x2);
		x3 = VectorDivide(sv, x3);

		Float3x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		return r;
#else
		Float3x3 result;
		result._11 = s / m1._11;
		result._12 = s / m1._12;
		result._13 = s / m1._13;
		result._21 = s / m1._21;
		result._22 = s / m1._22;
		result._23 = s / m1._23;
		result._31 = s / m1._31;
		result._32 = s / m1._32;
		result._33 = s / m1._33;
		return result;
#endif
	}

	inline Float4x3 operator/(f32 s, const Float4x3& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float3a(&(m1._11));
		vec_float_t x2 = load_float3a(&(m1._21));
		vec_float_t x3 = load_float3a(&(m1._31));
		vec_float_t x4 = load_float3a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorDivide(y, x1);
		x2 = VectorDivide(y, x2);
		x3 = VectorDivide(y, x3);
		x4 = VectorDivide(y, x4);

		Float4x3 r;
		store_float3a(&(r._11), x1);
		store_float3a(&(r._21), x2);
		store_float3a(&(r._31), x3);
		store_float3a(&(r._41), x4);
		return r;
#else
		Float4x3 result;
		result._11 = s / m1._11;
		result._12 = s / m1._12;
		result._13 = s / m1._13;
		result._21 = s / m1._21;
		result._22 = s / m1._22;
		result._23 = s / m1._23;
		result._31 = s / m1._31;
		result._32 = s / m1._32;
		result._33 = s / m1._33;
		result._41 = s / m1._41;
		result._42 = s / m1._42;
		result._43 = s / m1._43;
		return result;
#endif
	}

	inline Float3x4 operator/(f32 s, const Float3x4& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));

		vec_float_t y = vector_replicate(s);

		x1 = VectorDivide(y, x1);
		x2 = VectorDivide(y, x2);
		x3 = VectorDivide(y, x3);

		Float3x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		return r;
#else
		Float3x4 result;
		result._11 = s / m1._11;
		result._12 = s / m1._12;
		result._13 = s / m1._13;
		result._14 = s / m1._14;
		result._21 = s / m1._21;
		result._22 = s / m1._22;
		result._23 = s / m1._23;
		result._24 = s / m1._24;
		result._31 = s / m1._31;
		result._32 = s / m1._32;
		result._33 = s / m1._33;
		result._34 = s / m1._34;
		return result;
#endif
	}

	inline Float4x4 operator/(f32 s, const Float4x4& m1)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t x1 = load_float4a(&(m1._11));
		vec_float_t x2 = load_float4a(&(m1._21));
		vec_float_t x3 = load_float4a(&(m1._31));
		vec_float_t x4 = load_float4a(&(m1._41));

		vec_float_t y = vector_replicate(s);

		x1 = VectorDivide(y, x1);
		x2 = VectorDivide(y, x2);
		x3 = VectorDivide(y, x3);
		x4 = VectorDivide(y, x4);

		Float4x4 r;
		store_float4a(&(r._11), x1);
		store_float4a(&(r._21), x2);
		store_float4a(&(r._31), x3);
		store_float4a(&(r._41), x4);
		return r;
#else
		Float4x4 result;
		result._11 = s / m1._11;
		result._12 = s / m1._12;
		result._13 = s / m1._13;
		result._14 = s / m1._14;
		result._21 = s / m1._21;
		result._22 = s / m1._22;
		result._23 = s / m1._23;
		result._24 = s / m1._24;
		result._31 = s / m1._31;
		result._32 = s / m1._32;
		result._33 = s / m1._33;
		result._34 = s / m1._34;
		result._41 = s / m1._41;
		result._42 = s / m1._42;
		result._43 = s / m1._43;
		result._44 = s / m1._44;
		return result;
#endif
	}

	inline Float3 mul(const Float3& vec, const Float3x3& mat)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t s = vector_replicate(vec.x);
		vec_float_t m = load_float3a(&(mat._11));
		vec_float_t r = VectorMultiply(s, m);

		s = vector_replicate(vec.y);
		m = load_float3a(&(mat._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(vec.z);
		m = load_float3a(&(mat._31));
		r = VectorMultiplyAdd(s, m, r);

		Float3 result;
		store_float3a(result.m, r);
		return result;
#else
		return Float3(
			vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31,
			vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32,
			vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33);
#endif
	}

	inline Float3 mul(const Float3x3& mat, const Float3& vec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Float3 t;
		Float3 result;
		vec_float_t s = load_float3a(vec.m);

		vec_float_t m = load_float3a(&(mat._11));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.x = t.x + t.y + t.z;

		m = load_float3a(&(mat._21));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.y = t.x + t.y + t.z;

		m = load_float3a(&(mat._31));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.z = t.x + t.y + t.z;

		return result;
#else
		return Float3(
			vec.x * mat._11 + vec.y * mat._12 + vec.z * mat._13,
			vec.x * mat._21 + vec.y * mat._22 + vec.z * mat._23,
			vec.x * mat._31 + vec.y * mat._32 + vec.z * mat._33);
#endif
	}

	inline Float3x3 mul(const Float3x3& m1, const Float3x3& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Float3x3 result;

		vec_float_t s = vector_replicate(m1._11);
		vec_float_t m = load_float3a(&(m2._11));
		vec_float_t r = VectorMultiply(s, m);

		s = vector_replicate(m1._12);
		m = load_float3a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._13);
		m = load_float3a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		store_float3a(&(result._11), r);

		s = vector_replicate(m1._21);
		m = load_float3a(&(m2._11));
		r = VectorMultiply(s, m);

		s = vector_replicate(m1._22);
		m = load_float3a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._23);
		m = load_float3a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		store_float3a(&(result._21), r);

		s = vector_replicate(m1._31);
		m = load_float3a(&(m2._11));
		r = VectorMultiply(s, m);

		s = vector_replicate(m1._32);
		m = load_float3a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._33);
		m = load_float3a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		store_float3a(&(result._31), r);

		return result;
#else
		return Float3x3(
			m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31,
			m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32,
			m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33,

			m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31,
			m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32,
			m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33, 

			m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31,
			m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32,
			m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33);
#endif
	}

	inline Float4x3::Float4x3(const Float3x3& m, const Float3& row4)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r = load_float3a(&(m._11));
		store_float3a(&(_11), r);
		r = load_float3a(&(m._21));
		store_float3a(&(_21), r);
		r = load_float3a(&(m._31));
		store_float3a(&(_31), r);
		r = load_float3a(row4.m);
		store_float3a(&(_41), r);
#else
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
		_31 = m._31;
		_32 = m._32;
		_33 = m._33;
		_41 = row4.x;
		_42 = row4.y;
		_43 = row4.z;
#endif
	}

	inline Float3 mul(const Float4& vec, const Float4x3& mat)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t s = vector_replicate(vec.x);
		vec_float_t m = load_float3a(&(mat._11));
		vec_float_t r = VectorMultiply(s, m);

		s = vector_replicate(vec.y);
		m = load_float3a(&(mat._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(vec.z);
		m = load_float3a(&(mat._31));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(vec.w);
		m = load_float3a(&(mat._41));
		r = VectorMultiplyAdd(s, m, r);

		Float3 result;
		store_float3a(result.m, r);
		return result;
#else
		return Float3(
			vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31 + vec.w * mat._41,
			vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32 + vec.w * mat._42,
			vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33 + vec.w * mat._43);
#endif
	}

	inline Float4 mul(const Float4x3& mat, const Float3& vec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Float3 t;
		Float4 result;
		vec_float_t s = load_float3a(vec.m);

		vec_float_t m = load_float3a(&(mat._11));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.x = t.x + t.y + t.z;

		m = load_float3a(&(mat._21));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.y = t.x + t.y + t.z;

		m = load_float3a(&(mat._31));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.z = t.x + t.y + t.z;

		m = load_float3a(&(mat._41));
		m = VectorMultiply(m, s);
		store_float3a(t.m, m);
		result.w = t.x + t.y + t.z;

		return result;
#else
		return Float4(
			vec.x * mat._11 + vec.y * mat._12 + vec.z * mat._13,
			vec.x * mat._21 + vec.y * mat._22 + vec.z * mat._23,
			vec.x * mat._31 + vec.y * mat._32 + vec.z * mat._33,
			vec.x * mat._41 + vec.y * mat._42 + vec.z * mat._43);
#endif
	}

	inline Float3x4::Float3x4(const Float3x3& m, const Float3& col4)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r = load_float3a(&(m._11));
		store_float3a(&(_11), r);
		r = load_float3a(&(m._21));
		store_float3a(&(_21), r);
		r = load_float3a(&(m._31));
		store_float3a(&(_31), r);
		_14 = col4.x;
		_24 = col4.y;
		_34 = col4.z;
#else
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_14 = col4.x;
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
		_24 = col4.y;
		_31 = m._31;
		_32 = m._32;
		_33 = m._33;
		_34 = col4.z;
#endif
	}

	inline Float4 mul(const Float3& vec, const Float3x4& mat)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t s = vector_replicate(vec.x);
		vec_float_t m = load_float4a(&(mat._11));
		vec_float_t r = VectorMultiply(s, m);

		s = vector_replicate(vec.y);
		m = load_float4a(&(mat._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(vec.z);
		m = load_float4a(&(mat._31));
		r = VectorMultiplyAdd(s, m, r);

		Float4 result;
		store_float4a(result.m, r);
		return result;
#else
		return Float4(
			vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31,
			vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32,
			vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33,
			vec.x * mat._14 + vec.y * mat._24 + vec.z * mat._34);
#endif
	}

	inline Float3 mul(const Float3x4& mat, const Float4& vec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Float4 t;
		Float3 result;
		vec_float_t s = load_float4a(vec.m);

		vec_float_t m = load_float4a(&(mat._11));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.x = t.x + t.y + t.z + t.w;

		m = load_float4a(&(mat._21));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.y = t.x + t.y + t.z + t.w;

		m = load_float4a(&(mat._31));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.z = t.x + t.y + t.z + t.w;

		return result;
#else
		return Float3(
			vec.x * mat._11 + vec.y * mat._12 + vec.z * mat._13 + vec.w * mat._14,
			vec.x * mat._21 + vec.y * mat._22 + vec.z * mat._23 + vec.w * mat._24,
			vec.x * mat._31 + vec.y * mat._32 + vec.z * mat._33 + vec.w * mat._34);
#endif
	}

	inline Float4x4::Float4x4(const Float4x3& m, const Float4& col4)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r = load_float3a(&(m._11));
		store_float3a(&(_11), r);
		r = load_float3a(&(m._21));
		store_float3a(&(_21), r);
		r = load_float3a(&(m._31));
		store_float3a(&(_31), r);
		r = load_float3a(&(m._41));
		store_float3a(&(_41), r);
		_14 = col4.x;
		_24 = col4.y;
		_34 = col4.z;
		_44 = col4.w;
#else
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_14 = col4.x;
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
		_24 = col4.y;
		_31 = m._31;
		_32 = m._32;
		_33 = m._33;
		_34 = col4.z;
		_41 = m._41;
		_42 = m._42;
		_43 = m._43;
		_44 = col4.w;
#endif
	}

	inline Float4x4::Float4x4(const Float3x4& m, const Float4& row4)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t r = load_float4a(&(m._11));
		store_float4a(&(_11), r);
		r = load_float4a(&(m._21));
		store_float4a(&(_21), r);
		r = load_float4a(&(m._31));
		store_float4a(&(_31), r);
		r = load_float4a(row4.m);
		store_float4a(&(_41), r);
#else
		_11 = m._11;
		_12 = m._12;
		_13 = m._13;
		_14 = m._14;
		_21 = m._21;
		_22 = m._22;
		_23 = m._23;
		_24 = m._24;
		_31 = m._31;
		_32 = m._32;
		_33 = m._33;
		_34 = m._34;
		_41 = row4.x;
		_42 = row4.y;
		_43 = row4.z;
		_44 = row4.w;
#endif
	}

	inline Float4 mul(const Float4& vec, const Float4x4& mat)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t s = vector_replicate(vec.x);
		vec_float_t m = load_float4a(&(mat._11));
		vec_float_t r = VectorMultiply(s, m);

		s = vector_replicate(vec.y);
		m = load_float4a(&(mat._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(vec.z);
		m = load_float4a(&(mat._31));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(vec.w);
		m = load_float4a(&(mat._41));
		r = VectorMultiplyAdd(s, m, r);

		Float4 result;
		store_float4a(result.m, r);
		return result;
#else
		return Float4(
			vec.x * mat._11 + vec.y * mat._21 + vec.z * mat._31 + vec.w * mat._41,
			vec.x * mat._12 + vec.y * mat._22 + vec.z * mat._32 + vec.w * mat._42,
			vec.x * mat._13 + vec.y * mat._23 + vec.z * mat._33 + vec.w * mat._43,
			vec.x * mat._14 + vec.y * mat._24 + vec.z * mat._34 + vec.w * mat._44);
#endif
	}

	inline Float4 mul(const Float4x4& mat, const Float4& vec)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Float4 t;
		Float4 result;
		vec_float_t s = load_float4a(vec.m);

		vec_float_t m = load_float4a(&(mat._11));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.x = t.x + t.y + t.z + t.w;

		m = load_float4a(&(mat._21));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.y = t.x + t.y + t.z + t.w;

		m = load_float4a(&(mat._31));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.z = t.x + t.y + t.z + t.w;

		m = load_float4a(&(mat._41));
		m = VectorMultiply(m, s);
		store_float4a(t.m, m);
		result.w = t.x + t.y + t.z + t.w;

		return result;
#else
		return Float4(
			vec.x * mat._11 + vec.y * mat._12 + vec.z * mat._13 + vec.w * mat._14,
			vec.x * mat._21 + vec.y * mat._22 + vec.z * mat._23 + vec.w * mat._24,
			vec.x * mat._31 + vec.y * mat._32 + vec.z * mat._33 + vec.w * mat._34,
			vec.x * mat._41 + vec.y * mat._42 + vec.z * mat._43 + vec.w * mat._44);
#endif
	}

	inline Float4x4 mul(const Float4x4& m1, const Float4x4& m2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Float4x4 result;

		vec_float_t s = vector_replicate(m1._11);
		vec_float_t m = load_float4a(&(m2._11));
		vec_float_t r = VectorMultiply(s, m);

		s = vector_replicate(m1._12);
		m = load_float4a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._13);
		m = load_float4a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._14);
		m = load_float4a(&(m2._41));
		r = VectorMultiplyAdd(s, m, r);

		store_float4a(&(result._11), r);

		s = vector_replicate(m1._21);
		m = load_float4a(&(m2._11));
		r = VectorMultiply(s, m);

		s = vector_replicate(m1._22);
		m = load_float4a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._23);
		m = load_float4a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._24);
		m = load_float4a(&(m2._41));
		r = VectorMultiplyAdd(s, m, r);

		store_float4a(&(result._21), r);

		s = vector_replicate(m1._31);
		m = load_float4a(&(m2._11));
		r = VectorMultiply(s, m);

		s = vector_replicate(m1._32);
		m = load_float4a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._33);
		m = load_float4a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._34);
		m = load_float4a(&(m2._41));
		r = VectorMultiplyAdd(s, m, r);

		store_float4a(&(result._31), r);

		s = vector_replicate(m1._41);
		m = load_float4a(&(m2._11));
		r = VectorMultiply(s, m);

		s = vector_replicate(m1._42);
		m = load_float4a(&(m2._21));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._43);
		m = load_float4a(&(m2._31));
		r = VectorMultiplyAdd(s, m, r);

		s = vector_replicate(m1._44);
		m = load_float4a(&(m2._41));
		r = VectorMultiplyAdd(s, m, r);

		store_float4a(&(result._41), r);

		return result;
#else
		return Float4x4(
			m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31 + m1._14 * m2._41,
			m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32 + m1._14 * m2._42,
			m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33 + m1._14 * m2._43,
			m1._11 * m2._14 + m1._12 * m2._24 + m1._13 * m2._34 + m1._14 * m2._44,

			m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31 + m1._24 * m2._41,
			m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32 + m1._24 * m2._42,
			m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33 + m1._24 * m2._43,
			m1._21 * m2._14 + m1._22 * m2._24 + m1._23 * m2._34 + m1._24 * m2._44,

			m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31 + m1._34 * m2._41,
			m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32 + m1._34 * m2._42,
			m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33 + m1._34 * m2._43,
			m1._31 * m2._14 + m1._32 * m2._24 + m1._33 * m2._34 + m1._34 * m2._44,
			
			m1._41 * m2._11 + m1._42 * m2._21 + m1._43 * m2._31 + m1._44 * m2._41,
			m1._41 * m2._12 + m1._42 * m2._22 + m1._43 * m2._32 + m1._44 * m2._42,
			m1._41 * m2._13 + m1._42 * m2._23 + m1._43 * m2._33 + m1._44 * m2._43,
			m1._41 * m2._14 + m1._42 * m2._24 + m1._43 * m2._34 + m1._44 * m2._44);
#endif
	}

	inline f32 determinant(const Float3x3& mat)
	{
		return
			mat._11 * mat._22 * mat._33 +
			mat._12 * mat._23 * mat._31 +
			mat._13 * mat._21 * mat._32 -
			mat._13 * mat._22 * mat._31 -
			mat._12 * mat._21 * mat._33 -
			mat._11 * mat._23 * mat._32;
	}

	inline f32 determinant(const Float4x4& mat)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		mat_float_t m = load_float4x4a(&(mat._11));
		return VectorGetX(MatrixDeterminant(m));
#else
		return
			mat._11 *
			(mat._22 * mat._33 * mat._44 +
				mat._23 * mat._34 * mat._42 +
				mat._24 * mat._32 * mat._43 -
				mat._24 * mat._33 * mat._42 -
				mat._23 * mat._32 * mat._44 -
				mat._22 * mat._34 * mat._43)
			- mat._12 *
			(mat._21 * mat._33 * mat._44 +
				mat._23 * mat._34 * mat._41 +
				mat._24 * mat._31 * mat._43 -
				mat._24 * mat._33 * mat._41 -
				mat._23 * mat._31 * mat._44 -
				mat._21 * mat._34 * mat._43)
			+ mat._13 *
			(mat._21 * mat._32 * mat._44 +
				mat._22 * mat._34 * mat._41 +
				mat._24 * mat._31 * mat._42 -
				mat._24 * mat._32 * mat._41 -
				mat._22 * mat._31 * mat._44 -
				mat._21 * mat._34 * mat._42)
			- mat._14 *
			(mat._21 * mat._32 * mat._43 +
				mat._22 * mat._33 * mat._41 +
				mat._23 * mat._31 * mat._42 -
				mat._23 * mat._32 * mat._41 -
				mat._22 * mat._31 * mat._43 -
				mat._21 * mat._33 * mat._42);
#endif
	}

	inline Float3x3 transpose(const Float3x3& m)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		mat_float_t M = load_float3x4a(&(m._11));
		Float3x3 result;
		store_float3x4a(&(result._11), MatrixTranspose(M));
		return result;
#else
		return Float3x3(
			m._11, m._21, m._31,
			m._12, m._22, m._32,
			m._13, m._23, m._33
		);
#endif
	}
	inline Float3x4 transpose(const Float4x3& m)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		mat_float_t M = load_float4x4a(&(m._11));
		Float3x4 result;
		M = MatrixTranspose(M);
		store_float4a(&(result._11), M.r[0]);
		store_float4a(&(result._21), M.r[1]);
		store_float4a(&(result._31), M.r[2]);
		return result;
#else
		return Float3x4(
			m._11, m._21, m._31, m._41,
			m._12, m._22, m._32, m._42,
			m._13, m._23, m._33, m._43
		);
#endif
	}
	inline Float4x3 transpose(const Float3x4& m)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		mat_float_t M;
		M.r[0] = load_float4a(&(m._11));
		M.r[1] = load_float4a(&(m._21));
		M.r[2] = load_float4a(&(m._31));
		Float4x3 result;
		M = MatrixTranspose(M);
		store_float4x4a(&(result._11), M);
		return result;
#else
		return Float4x3(
			m._11, m._21, m._31,
			m._12, m._22, m._32,
			m._13, m._23, m._33,
			m._14, m._24, m._34
		);
#endif
	}
	inline Float4x4 transpose(const Float4x4& m)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		mat_float_t M = load_float4x4a(&(m._11));
		Float4x4 result;
		store_float4x4a(&(result._11), MatrixTranspose(M));
		return result;
#else
		return Float4x4(
			m._11, m._21, m._31, m._41,
			m._12, m._22, m._32, m._42,
			m._13, m._23, m._33, m._43,
			m._14, m._24, m._34, m._44
		);
#endif
	}

	inline Float3x3 inverse(const Float3x3& m, f32* out_determinant)
	{
		f32 det = determinant(m);
		if (out_determinant)
		{
			*out_determinant = det;
		}
		if (det > -FLT_EPSILON && det < FLT_EPSILON)
		{
			det = FLT_EPSILON;
		}

		f32 det_inv = 1.0f / det;
		Float3x3 r;
		r._11 = det_inv * (m._22 * m._33 - m._32 * m._23);
		r._21 = -det_inv * (m._21 * m._33 - m._31 * m._23);
		r._31 = det_inv * (m._21 * m._32 - m._31 * m._22);

		r._12 = -det_inv * (m._12 * m._33 - m._32 * m._13);
		r._22 = det_inv * (m._11 * m._33 - m._31 * m._13);
		r._32 = -det_inv * (m._11 * m._32 - m._31 * m._12);

		r._13 = det_inv * (m._12 * m._23 - m._22 * m._13);
		r._23 = -det_inv * (m._11 * m._23 - m._21 * m._13);
		r._33 = det_inv * (m._11 * m._22 - m._21 * m._12);
		return r;
	}

	inline Float4x4 inverse(const Float4x4& m, f32* out_determinant)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		mat_float_t M = load_float4x4a(&(m._11));
		vec_float_t det;
		Float4x4 result;
		store_float4x4a(&(result._11), MatrixInverse(&det, M));
		if (out_determinant)
		{
			store_float(out_determinant, det);
		}
		return result;
#else
		f32 det = determinant(m);
		if (out_determinant)
		{
			*out_determinant = det;
		}
		if (det > -f32_epsilon && det < f32_epsilon)
		{
			det = f32_epsilon;
		}

		f32 det_inv = 1.0f / det;
		Float4x4 r;

		r._11 = (m._22 * m._33 * m._44 + m._23 * m._34 * m._42 + m._24 * m._32 * m._43 - m._24 * m._33 * m._42 - m._23 * m._32 * m._44 - m._22 * m._34 * m._43) / det;
		r._21 = (m._21 * m._33 * m._44 + m._23 * m._34 * m._41 + m._24 * m._31 * m._43 - m._24 * m._33 * m._41 - m._23 * m._31 * m._44 - m._21 * m._34 * m._43) / -det;
		r._31 = (m._21 * m._32 * m._44 + m._22 * m._34 * m._41 + m._24 * m._31 * m._42 - m._24 * m._32 * m._41 - m._22 * m._31 * m._44 - m._21 * m._34 * m._42) / det;
		r._41 = (m._21 * m._32 * m._43 + m._22 * m._33 * m._41 + m._23 * m._31 * m._42 - m._23 * m._32 * m._41 - m._22 * m._31 * m._43 - m._21 * m._33 * m._42) / -det;

		r._12 = (m._12 * m._33 * m._44 + m._13 * m._34 * m._42 + m._14 * m._32 * m._43 - m._14 * m._33 * m._42 - m._13 * m._32 * m._44 - m._12 * m._34 * m._43) / -det;
		r._22 = (m._11 * m._33 * m._44 + m._13 * m._34 * m._41 + m._14 * m._31 * m._43 - m._14 * m._33 * m._41 - m._13 * m._31 * m._44 - m._11 * m._34 * m._43) / det;
		r._32 = (m._11 * m._32 * m._44 + m._12 * m._34 * m._41 + m._14 * m._31 * m._42 - m._14 * m._32 * m._41 - m._12 * m._31 * m._44 - m._11 * m._34 * m._42) / -det;
		r._42 = (m._11 * m._32 * m._43 + m._12 * m._33 * m._41 + m._13 * m._31 * m._42 - m._13 * m._32 * m._41 - m._12 * m._31 * m._43 - m._11 * m._33 * m._42) / det;

		r._13 = (m._12 * m._23 * m._44 + m._13 * m._24 * m._42 + m._14 * m._22 * m._43 - m._14 * m._23 * m._42 - m._13 * m._22 * m._44 - m._12 * m._24 * m._43) / det;
		r._23 = (m._11 * m._23 * m._44 + m._13 * m._24 * m._41 + m._14 * m._21 * m._43 - m._14 * m._23 * m._41 - m._13 * m._21 * m._44 - m._11 * m._24 * m._43) / -det;
		r._33 = (m._11 * m._22 * m._44 + m._12 * m._24 * m._41 + m._14 * m._21 * m._42 - m._14 * m._22 * m._41 - m._12 * m._21 * m._44 - m._11 * m._24 * m._42) / det;
		r._43 = (m._11 * m._22 * m._43 + m._12 * m._23 * m._41 + m._13 * m._21 * m._42 - m._13 * m._22 * m._41 - m._12 * m._21 * m._43 - m._11 * m._23 * m._42) / -det;

		r._14 = (m._12 * m._23 * m._34 + m._13 * m._24 * m._32 + m._14 * m._22 * m._33 - m._14 * m._23 * m._32 - m._13 * m._22 * m._34 - m._12 * m._24 * m._33) / -det;
		r._24 = (m._11 * m._23 * m._34 + m._13 * m._24 * m._31 + m._14 * m._21 * m._33 - m._14 * m._23 * m._31 - m._13 * m._21 * m._34 - m._11 * m._24 * m._33) / det;
		r._34 = (m._11 * m._22 * m._34 + m._12 * m._24 * m._31 + m._14 * m._21 * m._32 - m._14 * m._22 * m._31 - m._12 * m._21 * m._34 - m._11 * m._24 * m._32) / -det;
		r._44 = (m._11 * m._22 * m._33 + m._12 * m._23 * m._31 + m._13 * m._21 * m._32 - m._13 * m._22 * m._31 - m._12 * m._21 * m._33 - m._11 * m._23 * m._32) / det;
 		return r;
#endif
	}

	inline Float4x4 Float4x4::make_billboard(const Float3& object_pos, const Float3& camera_pos,
		const Float3& camera_up, const Float3& camera_forward)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t O = load_float3a(object_pos.m);
		vec_float_t C = load_float3a(camera_pos.m);
		vec_float_t Z = VectorSubtract(O, C);

		vec_float_t N = Vector3LengthSq(Z);
		if (Vector3Less(N, g_Epsilon))
		{
			vec_float_t F = load_float3a(camera_forward.m);
			Z = VectorNegate(F);
		}
		else
		{
			Z = Vector3Normalize(Z);
		}

		vec_float_t up = load_float3a(camera_up.m);
		vec_float_t X = Vector3Cross(up, Z);
		X = Vector3Normalize(X);

		vec_float_t Y = Vector3Cross(Z, X);

		mat_float_t M;
		M.r[0] = X;
		M.r[1] = Y;
		M.r[2] = Z;
		M.r[3] = VectorSetW(O, 1.f);

		Float4x4 R;
		store_float4x4a(R.m[0], M);
		return R;
#else
		Float3 Z = object_pos - camera_pos;

		f32 N = length_squared(Z);
		if (N < 1.192092896e-7f)
		{
			Z = -camera_forward;
		}
		else
		{
			Z = normalize(Z);
		}

		Float3 X = cross(camera_up, Z);
		X = normalize(X);

		Float3 Y = cross(Z, X);

		Float4x4 M(
			X.x, X.y, X.z, 0.0f,
			Y.x, Y.y, Y.z, 0.0f,
			Z.x, Z.y, Z.z, 0.0f,
			object_pos.x, object_pos.y, object_pos.z, 1.0f
		);
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_constrained_billboard(const Float3& object_pos, const Float3& camera_pos,
		const Float3& rotate_axis, const Float3& camera_forward, const Float3& object_forward)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		constexpr Float4 s_minAngle = { 0.99825467075f, 0.99825467075f, 0.99825467075f, 0.99825467075f };// 1.0 - ConvertToRadians( 0.1f );

		vec_float_t O = load_float3a(object_pos.m);
		vec_float_t C = load_float3a(camera_pos.m);
		vec_float_t faceDir = VectorSubtract(O, C);

		vec_float_t N = Vector3LengthSq(faceDir);
		if (Vector3Less(N, g_Epsilon))
		{
			vec_float_t F = load_float3a(camera_forward.m);
			faceDir = VectorNegate(F);
		}
		else
		{
			faceDir = Vector3Normalize(faceDir);
		}

		vec_float_t Y = load_float3a(rotate_axis.m);
		vec_float_t X, Z;

		vec_float_t dot = VectorAbs(Vector3Dot(Y, faceDir));
		if (Vector3Greater(dot, s_minAngle))
		{
			Z = load_float3a(object_forward.m);
			dot = VectorAbs(Vector3Dot(Y, Z));
			if (Vector3Greater(dot, s_minAngle))
			{
				dot = VectorAbs(Vector3Dot(Y, g_NegIdentityR2));
				Z = (Vector3Greater(dot, s_minAngle)) ? g_IdentityR0 : g_NegIdentityR2;
			}

			X = Vector3Cross(Y, Z);
			X = Vector3Normalize(X);

			Z = Vector3Cross(X, Y);
			Z = Vector3Normalize(Z);
		}
		else
		{
			X = Vector3Cross(Y, faceDir);
			X = Vector3Normalize(X);

			Z = Vector3Cross(X, Y);
			Z = Vector3Normalize(Z);
		}

		mat_float_t M;
		M.r[0] = X;
		M.r[1] = Y;
		M.r[2] = Z;
		M.r[3] = VectorSetW(O, 1.f);

		Float4x4 R;
		store_float4x4a(R.m[0], M);
		return R;
#else
		static const f32 s_minAngle{ 0.99825467075f }; // 1.0 - ConvertToRadians( 0.1f );

		//vec_float_t O = LoadFloat3(object.m);
		//vec_float_t C = LoadFloat3(cameraPosition.m);
		Float3 faceDir = object_pos - camera_pos;

		f32 N = length_squared(faceDir);
		if (N < 1.192092896e-7f)
		{
			faceDir = -camera_forward;
		}
		else
		{
			faceDir = normalize(faceDir);
		}

		Float3 Y = rotate_axis;
		Float3 X, Z;

		f32 d = fabsf(dot(Y, faceDir));
		if (d > s_minAngle)
		{
			Z = object_forward;
			d = fabsf(dot(Y, Z));
			if (d > s_minAngle)
			{
				d = fabsf(dot(Y, Float3(0.0f, 0.0f, -1.0f)));
				Z = (d > s_minAngle) ? Float3(1.0f, 0.0f, 0.0f) : Float3(0.0f, 0.0f, -1.0f);
			}

			X = cross(Y, Z);
			X = normalize(X);

			Z = cross(X, Y);
			Z = normalize(Z);
		}
		else
		{
			X = cross(Y, faceDir);
			X = normalize(X);

			Z = cross(X, Y);
			Z = normalize(Z);
		}

		Float4x4 M(
			X.x, X.y, X.z, 0.0f,
			Y.x, Y.y, Y.z, 0.0f,
			Z.x, Z.y, Z.z, 0.0f,
			object_pos.x, object_pos.y, object_pos.z, 1.0f
		);
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_translation(const Float3& position)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixTranslation(position.x, position.y, position.z));
		return R;
#else
		Float4x4 M;
		M.m[0][0] = 1.0f;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = 1.0f;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = 1.0f;
		M.m[2][3] = 0.0f;

		M.m[3][0] = position.x;
		M.m[3][1] = position.y;
		M.m[3][2] = position.z;
		M.m[3][3] = 1.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_scale(const Float3& scales)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixScaling(scales.x, scales.y, scales.z));
		return R;
#else
		Float4x4 M;
		M.m[0][0] = scales.x;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = scales.y;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = scales.z;
		M.m[2][3] = 0.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = 0.0f;
		M.m[3][3] = 1.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_rotation_x(f32 radians)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixRotationX(radians));
		return R;
#else
		f32    fSinAngle = sinf(radians);
		f32    fCosAngle = cosf(radians);

		Float4x4 M;
		M.m[0][0] = 1.0f;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = fCosAngle;
		M.m[1][2] = fSinAngle;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = -fSinAngle;
		M.m[2][2] = fCosAngle;
		M.m[2][3] = 0.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = 0.0f;
		M.m[3][3] = 1.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_rotation_y(f32 radians)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixRotationY(radians));
		return R;
#else
		f32    fSinAngle = sinf(radians);
		f32    fCosAngle = cosf(radians);

		Float4x4 M;
		M.m[0][0] = fCosAngle;
		M.m[0][1] = 0.0f;
		M.m[0][2] = -fSinAngle;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = 1.0f;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = fSinAngle;
		M.m[2][1] = 0.0f;
		M.m[2][2] = fCosAngle;
		M.m[2][3] = 0.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = 0.0f;
		M.m[3][3] = 1.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_rotation_z(f32 radians)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixRotationZ(radians));
		return R;
#else
		f32    fSinAngle = sinf(radians);
		f32    fCosAngle = cosf(radians);

		Float4x4 M;
		M.m[0][0] = fCosAngle;
		M.m[0][1] = fSinAngle;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = -fSinAngle;
		M.m[1][1] = fCosAngle;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = 1.0f;
		M.m[2][3] = 0.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = 0.0f;
		M.m[3][3] = 1.0f;
		return M;
#endif
	}

	inline Float4x4 Float4x4::make_rotation_quaternion(const Quaternion& quaternion)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixRotationQuaternion(quaternion));
		return R;
#else
		lupanic("Not implemented");
#endif
	}

	inline Float4x4 Float4x4::make_from_axis_angle(const Float3& axis, f32 angle)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		vec_float_t a = load_float3(axis.m);
		store_float4x4a(R.m[0], MatrixRotationAxis(a, angle));
		return R;
#else
		Float3 norm = normalize(axis);

		f32    fSinAngle = sinf(angle);
		f32    fCosAngle = cosf(angle);

		Float4 A(fSinAngle, fCosAngle, 1.0f - fCosAngle, 0.0f);

		Float4 C2(A.z, A.z, A.z, A.z);
		Float4 C1(A.y, A.y, A.y, A.y);
		Float4 C0(A.x, A.x, A.x, A.x);

		Float4 N0(norm.y, norm.z, norm.x, 0.0f);
		Float4 N1(norm.z, norm.x, norm.y, 0.0f);

		Float4 V0 = C2 * N0;
		V0 = V0 * N1;

		Float4 R0 = C2 * Float4(norm.x, norm.y, norm.z, 0.0f);
		R0 = (R0 * Float4(norm.x, norm.y, norm.z, 0.0f)) + C1;

		Float4 R1 = (C0 * Float4(norm.x, norm.y, norm.z, 0.0f)) + V0;
		Float4 R2 = C0 - (Float4(norm.x, norm.y, norm.z, 0.0f) * V0);

		V0 = Float4(R0.x, R0.y, R0.z, A.w);
		Float4 V1(R1.z, R2.y, R2.z, R1.x);
		Float4 V2(R1.y, R2.x, R1.y, R2.x);

		Float4x4 M(
			V0.x, V1.x, V1.y, V0.w,
			V1.z, V0.y, V1.w, V0.w,
			V2.x, V2.y, V0.z, V0.w,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_perspective_field_of_view(f32 fov, f32 aspect_ratio,
		f32 near_plane_distance, f32 far_plance_distance)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixPerspectiveFovLH(fov, aspect_ratio, near_plane_distance, far_plance_distance));
		return R;
#else
		f32    SinFov = sinf(0.5f * fov);
		f32    CosFov = cosf(0.5f * fov);

		f32 Height = CosFov / SinFov;
		f32 Width = Height / aspect_ratio;
		f32 fRange = far_plance_distance / (near_plane_distance - far_plance_distance);

		Float4x4 M;
		M.m[0][0] = Width;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = Height;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = fRange;
		M.m[2][3] = -1.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = fRange * near_plane_distance;
		M.m[3][3] = 0.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_perspective(f32 width, f32 height, f32 near_plane_distance,
		f32 far_plane_distance)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixPerspectiveLH(width, height, near_plane_distance, far_plane_distance));
		return R;
#else
		f32 TwoNearZ = near_plane_distance + near_plane_distance;
		f32 fRange = far_plane_distance / (near_plane_distance - far_plane_distance);

		Float4x4 M;
		M.m[0][0] = TwoNearZ / width;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = TwoNearZ / height;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = fRange;
		M.m[2][3] = -1.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = fRange * near_plane_distance;
		M.m[3][3] = 0.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_perspective_off_center(f32 left, f32 right, f32 bottom,
		f32 top, f32 near_plane_distance, f32 far_plance_distance)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixPerspectiveOffCenterLH(left, right, bottom, top, near_plane_distance, far_plance_distance));
		return R;
#else
		f32 TwoNearZ = near_plane_distance + near_plane_distance;
		f32 ReciprocalWidth = 1.0f / (right - left);
		f32 ReciprocalHeight = 1.0f / (top - bottom);
		f32 fRange = far_plance_distance / (near_plane_distance - far_plance_distance);

		Float4x4 M;
		M.m[0][0] = TwoNearZ * ReciprocalWidth;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = TwoNearZ * ReciprocalHeight;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = (left + right) * ReciprocalWidth;
		M.m[2][1] = (top + bottom) * ReciprocalHeight;
		M.m[2][2] = fRange;
		M.m[2][3] = -1.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = fRange * near_plane_distance;
		M.m[3][3] = 0.0f;
		return M;
#endif
	}
	inline Float4x4 Float4x4::make_orthographic(f32 width, f32 height, f32 z_near_place_distance,
		f32 z_far_plane_distance)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixOrthographicLH(width, height, z_near_place_distance, z_far_plane_distance));
		return R;
#else
		f32 fRange = 1.0f / (z_near_place_distance - z_far_plane_distance);

		Float4x4 M;
		M.m[0][0] = 2.0f / width;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = 2.0f / height;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = fRange;
		M.m[2][3] = 0.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = fRange * z_near_place_distance;
		M.m[3][3] = 1.0f;
		return M;

#endif
	}
	inline Float4x4 Float4x4::make_orthographic_off_center(f32 left, f32 right, f32 bottom,
		f32 top, f32 near_plane_distance, f32 far_plance_distance)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		store_float4x4a(R.m[0], MatrixOrthographicOffCenterLH(left, right, bottom, top, near_plane_distance, far_plance_distance));
		return R;
#else
		f32 ReciprocalWidth = 1.0f / (right - left);
		f32 ReciprocalHeight = 1.0f / (top - bottom);
		f32 fRange = 1.0f / (near_plane_distance - far_plance_distance);

		Float4x4 M;
		M.m[0][0] = ReciprocalWidth + ReciprocalWidth;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = ReciprocalHeight + ReciprocalHeight;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = fRange;
		M.m[2][3] = 0.0f;

		M.m[3][0] = -(left + right) * ReciprocalWidth;
		M.m[3][1] = -(top + bottom) * ReciprocalHeight;
		M.m[3][2] = fRange * near_plane_distance;
		M.m[3][3] = 1.0f;

		return M;
#endif
	}
	inline Float4x4 Float4x4::make_look_at(const Float3& camera_position, const Float3& target_position, const Float3& up_dir)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		Float4x4 R;
		vec_float_t eyev = load_float3a(camera_position.m);
		vec_float_t targetv = load_float3a(target_position.m);
		vec_float_t upv = load_float3a(up_dir.m);
		store_float4x4(R.m[0], MatrixLookAtLH(eyev, targetv, upv));
		return R;
#else
		Float3 NegEyeDirection = camera_position - target_position;

		Float3 R2 = normalize(NegEyeDirection);

		Float3 R0 = normalize(cross(up_dir, R2));

		Float3 R1 = cross(R2, R0);

		Float3 NegEyePosition = -camera_position;

		f32 D0 = dot(R0, NegEyePosition);
		f32 D1 = dot(R1, NegEyePosition);
		f32 D2 = dot(R2, NegEyePosition);

		Float4x4 M(
			R0.x, R0.y, R0.z, D0,
			R1.x, R1.y, R1.z, D1,
			R2.x, R2.y, R2.z, D2,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		return transpose(M);
#endif
	}
	inline Float4x4 Float4x4::make_world(const Float3& position, const Float3& forward, const Float3& up)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t zaxis = Vector3Normalize(VectorNegate(load_float3a(forward.m)));
		vec_float_t yaxis = load_float3a(up.m);
		vec_float_t xaxis = Vector3Normalize(Vector3Cross(yaxis, zaxis));
		yaxis = Vector3Cross(zaxis, xaxis);

		Float4x4 R;
		store_float3a(reinterpret_cast<f32*>(&R._11), xaxis);
		store_float3a(reinterpret_cast<f32*>(&R._21), yaxis);
		store_float3a(reinterpret_cast<f32*>(&R._31), zaxis);
		R._14 = R._24 = R._34 = 0.f;
		R._41 = position.x; R._42 = position.y; R._43 = position.z;
		R._44 = 1.f;
		return R;
#else
		Float3 zaxis = normalize(-forward);
		Float3 yaxis = up;
		Float3 xaxis = normalize(cross(yaxis, zaxis));
		yaxis = cross(zaxis, xaxis);

		Float4x4 R (
			xaxis.x, xaxis.y, xaxis.z, 0.0f,
			yaxis.x, yaxis.y, yaxis.z, 0.0f,
			zaxis.x, zaxis.y, zaxis.z, 0.0f,
			position.x, position.y, position.z, 1.0f
		);
		return R;
#endif
	}

	inline Float4x4 Float4x4::make_affine_position_rotation_scale(const Float3& position, const Quaternion& rotation, const Float3& scale)
	{
#ifdef LUNA_SIMD_ENABLED
		using namespace Simd;
		vec_float_t s = load_float3a(scale.m);
		vec_float_t p = load_float3a(position.m);
		vec_float_t q = load_float4a(rotation.m);
		vec_float_t zero = vector_set(0.0f, 0.0f, 0.0f, 1.0f);
		mat_float_t r = MatrixAffineTransformation(s, zero, q, p);
		return Float4x4(r);
#else
		lupanic("Not Implemented");
#endif
	}

	inline Float4x4 Float4x4::make_transform3d(const Tranform3D& transform)
	{
		return make_affine_position_rotation_scale(transform.position, transform.rotation, transform.scale);
	}
}