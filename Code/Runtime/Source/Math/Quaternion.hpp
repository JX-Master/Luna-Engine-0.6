// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Quaternion.hpp
* @author JXMaster
* @date 2019/1/5
 */
#pragma once

#include "Vector.hpp"

namespace Luna
{
	struct alignas(Float4) Quaternion
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
			f32 m[4];
		};

		Quaternion() = default;
		Quaternion(const Quaternion&) = default;
		Quaternion& operator=(const Quaternion&) = default;

		Quaternion(Quaternion&&) = default;
		Quaternion& operator=(Quaternion&&) = default;
		constexpr Quaternion(f32 _x, f32 _y, f32 _z, f32 _w) :
			x(_x), y(_y), z(_z), w(_w) {}
		Quaternion(const Float3& v, f32 scalar) :
			x(v.x), y(v.y), z(v.z), w(scalar) {}
		explicit Quaternion(const Float4& v) :
			x(v.x), y(v.y), z(v.z), w(v.w) {}
		explicit Quaternion(const f32* arr)
		{
#ifdef LUNA_SIMD_ENABLED
			using namespace Simd;
			vec_float_t vec = load_float4(arr);
			store_float4a(&x, vec);
#else
			x = arr[0];
			y = arr[1];
			z = arr[2];
			w = arr[3];
#endif
		}
#ifdef LUNA_SIMD_ENABLED
		Quaternion(Simd::vec_float_p1_t v) { Simd::store_float4a(this->m, v); }
		operator Simd::vec_float_t() const { return Simd::load_float4a(this->m); }
#endif

		// Comparison operators
		bool operator == (const Quaternion& q) const;
		bool operator != (const Quaternion& q) const;

		// Assignment operators
		Quaternion& operator= (const Float4& F) { x = F.x; y = F.y; z = F.z; w = F.w; return *this; }
		Quaternion& operator+= (const Quaternion& q);
		Quaternion& operator-= (const Quaternion& q);
		Quaternion& operator*= (const Quaternion& q);
		Quaternion& operator*= (f32 S);
		Quaternion& operator/= (const Quaternion& q);

		// Unary operators
		Quaternion operator+ () const { return *this; }
		Quaternion operator- () const;

		// Additional assignment.
		static Quaternion from_axis_angle(const Float3& axis, f32 angle);
		static Quaternion from_euler_angles(const Float3& euler_angles);
		static Quaternion from_roll_pitch_yaw(f32 pitch, f32 yaw, f32 roll);

		// Constants
		static constexpr Quaternion identity() { return Quaternion(0.0f, 0.0f, 0.0f, 1.0f); }
	};

	// Binary operators
	Quaternion operator+ (const Quaternion& q1, const Quaternion& q2);
	Quaternion operator- (const Quaternion& q1, const Quaternion& q2);
	Quaternion operator* (const Quaternion& q1, const Quaternion& q2);
	Quaternion operator* (const Quaternion& q, f32 s);
	Quaternion operator/ (const Quaternion& q1, const Quaternion& q2);
	Quaternion operator* (f32 s, const Quaternion& q);

	f32 length(const Quaternion& q);
	f32 length_squared(const Quaternion& q);
	Quaternion normalize(const Quaternion& q);
	Quaternion conjugate(const Quaternion& q);
	Quaternion inverse(const Quaternion& q);
	f32 dot(const Quaternion& q1, const Quaternion& q2);
	Quaternion lerp(const Quaternion& q1, const Quaternion& q2, f32 t);
	//! Spherical interpolation.
	Quaternion slerp(const Quaternion& q1, const Quaternion& q2, f32 t);
	Quaternion concatenate(const Quaternion& q1, const Quaternion& q2);


	//------------------------------------------------------------------------------------
	//
	//	Implementation part.
	//
	//------------------------------------------------------------------------------------

	/****************************************************************************
	 *
	 *	Quaternion
	 *
	 ****************************************************************************/

	 //------------------------------------------------------------------------------
	 //	 Comparison operators
	 //------------------------------------------------------------------------------

	inline bool Quaternion::operator == (const Quaternion& q) const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(this->m);
		vec_float_t q2 = load_float4a(q.m);
		return QuaternionEqual(q1, q2);
#else
		return (x == q.x) && (y == q.y) && (z == q.z) && (w == q.w);
#endif
	}

	inline bool Quaternion::operator != (const Quaternion& q) const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(this->m);
		vec_float_t q2 = load_float4a(q.m);
		return QuaternionNotEqual(q1, q2);
#else
		return (x != q.x) || (y != q.y) || (z != q.z) || (w != q.w);
#endif
	}

	//------------------------------------------------------------------------------
	// Assignment operators
	//------------------------------------------------------------------------------

	inline Quaternion& Quaternion::operator+= (const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(this->m);
		vec_float_t q2 = load_float4a(q.m);
		store_float4a(this->m, VectorAdd(q1, q2));
		return *this;
#else
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
		return *this;
#endif
	}

	inline Quaternion& Quaternion::operator-= (const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(this->m);
		vec_float_t q2 = load_float4a(q.m);
		store_float4a(this->m, VectorSubtract(q1, q2));
		return *this;
#else
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
		return *this;
#endif
	}

	inline Quaternion& Quaternion::operator*= (const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(this->m);
		vec_float_t q2 = load_float4a(q.m);
		store_float4a(this->m, QuaternionMultiply(q1, q2));
		return *this;
#else
		x = q.w * x + q.x * w + q.y * z - q.z * y;
		y = q.w * y - q.x * z + q.y * w + q.z * x;
		z = q.w * z + q.x * y - q.y * x + q.z * w;
		w = q.w * w - q.x * x - q.y * y - q.z * z;
#endif
	}

	inline Quaternion& Quaternion::operator*= (f32 S)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q = load_float4a(this->m);
		store_float4a(this->m, VectorScale(q, S));
		return *this;
#else
		x *= S;
		y *= S;
		z *= S;
		w *= S;
#endif
	}

	inline Quaternion& Quaternion::operator/= (const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(this->m);
		vec_float_t q2 = load_float4a(q.m);
		q2 = QuaternionInverse(q2);
		store_float4a(this->m, QuaternionMultiply(q1, q2));
		return *this;
#else
		Quaternion inv = inverse(q);
		*this = *this * inv;
#endif
	}

	//------------------------------------------------------------------------------
	// Unary operators
	//------------------------------------------------------------------------------

	inline Quaternion Quaternion::operator- () const
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q = load_float4a(this->m);

		Quaternion R;
		store_float4a(R.m, VectorNegate(q));
		return R;
#else
		return Quaternion(-x, -y, -z, -w);
#endif
	}

	//------------------------------------------------------------------------------
	// Binary operators
	//------------------------------------------------------------------------------

	inline Quaternion operator+ (const Quaternion& Q1, const Quaternion& Q2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(Q1.m);
		vec_float_t q2 = load_float4a(Q2.m);

		Quaternion R;
		store_float4a(R.m, VectorAdd(q1, q2));
		return R;
#else
		return Quaternion(Q1.x + Q2.x, Q1.y + Q2.y, Q1.z + Q2.z, Q1.w + Q2.w);
#endif
	}

	inline Quaternion operator- (const Quaternion& Q1, const Quaternion& Q2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(Q1.m);
		vec_float_t q2 = load_float4a(Q2.m);

		Quaternion R;
		store_float4a(R.m, VectorSubtract(q1, q2));
		return R;
#else
		return Quaternion(Q1.x - Q2.x, Q1.y - Q2.y, Q1.z - Q2.z, Q1.w - Q2.w);
#endif
	}

	inline Quaternion operator* (const Quaternion& Q1, const Quaternion& Q2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(Q1.m);
		vec_float_t q2 = load_float4a(Q2.m);

		Quaternion R;
		store_float4a(R.m, QuaternionMultiply(q1, q2));
		return R;
#else
		return Quaternion(
			Q2.w * Q1.x + Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y,
			Q2.w * Q1.y - Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x,
			Q2.w * Q1.z + Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w,
			Q2.w * Q1.w - Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z);
#endif
	}

	inline Quaternion operator* (const Quaternion& Q, f32 S)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q = load_float4a(Q.m);

		Quaternion R;
		store_float4a(R.m, VectorScale(q, S));
		return R;
#else
		return Quaternion(Q.x * S, Q.y * S, Q.z * S, Q.w * S);
#endif
	}

	inline Quaternion operator/ (const Quaternion& Q1, const Quaternion& Q2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(Q1.m);
		vec_float_t q2 = load_float4a(Q2.m);
		q2 = QuaternionInverse(q2);

		Quaternion R;
		store_float4a(R.m, QuaternionMultiply(q1, q2));
		return R;
#else
		Quaternion inv = inverse(Q2);
		return Q1 * inv;
#endif
	}

	inline Quaternion operator* (f32 S, const Quaternion& Q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q1 = load_float4a(Q.m);

		Quaternion R;
		store_float4a(R.m, VectorScale(q1, S));
		return R;
#else
		return Quaternion(Q.x * S, Q.y * S, Q.z * S, Q.w * S);
#endif
	}

	//------------------------------------------------------------------------------
	// Quaternion operations
	//------------------------------------------------------------------------------

	inline f32 length(Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q_vec = load_float4a(q.m);
		return VectorGetX(QuaternionLength(q_vec));
#else
		return sqrtf((q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w));
#endif
	}

	inline f32 length_squared(Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t q_vec = load_float4a(q.m);
		return VectorGetX(QuaternionLengthSq(q_vec));
#else
		return (q.x * q.x) + (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
#endif
	}

	inline Quaternion normalize(const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t vecq = load_float4a(q.m);
		Quaternion result;
		store_float4a(result.m, QuaternionNormalize(vecq));
		return result;
#else
		Quaternion result;
		Float4 res(q.x, q.y, q.z, q.w);
		res = normalize(res);
		result.x = res.x;
		result.y = res.y;
		result.z = res.z;
		result.w = res.w;
		return result;
#endif
	}

	inline Quaternion conjugate(const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t vecq = load_float4a(q.m);
		Quaternion result;
		store_float4a(result.m, QuaternionConjugate(vecq));
		return result;
#else
		Quaternion result;
		result.x = -q.x;
		result.y = -q.y;
		result.z = -q.z;
		result.w = q.w;
		return result;
#endif
	}

	inline Quaternion inverse(const Quaternion& q)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t vecq = load_float4a(q.m);
		Quaternion result;
		store_float4a(result.m, QuaternionInverse(vecq));
		return result;
#else
		f32 l = length_squared(q);
		Quaternion conjugated = conjugate(q);
		Quaternion result;
		if (l <= 1.192092896e-7f)
		{
			result.x = 0.0f;
			result.y = 0.0f;
			result.z = 0.0f;
			result.w = 0.0f;
		}
		else
		{
			result.x = conjugated.x / l;
			result.y = conjugated.y / l;
			result.z = conjugated.z / l;
			result.w = conjugated.w / l;
		}
		return result;
#endif
	}

	inline f32 dot(const Quaternion& q1, const Quaternion& q2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t qua1 = load_float4a(q1.m);
		vec_float_t qua2 = load_float4a(q2.m);
		return VectorGetX(QuaternionDot(qua1, qua2));
#else
		Float4 d1(q1.x, q1.y, q1.z, q1.w);
		Float4 d2(q2.x, q2.y, q2.z, q2.w);
		return dot(d1, d2);
#endif
	}

	//------------------------------------------------------------------------------
	// Static functions
	//------------------------------------------------------------------------------

	inline Quaternion Quaternion::from_axis_angle(const Float3 &axis, f32 angle)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Quaternion result;
		vec_float_t a = load_float3(axis.m);
		store_float4a(result.m, QuaternionRotationAxis(a, angle));
		return result;
#else
		Float3 normal = normalize(axis);

		f32 sinV = sinf(0.5f * angle);
		f32 cosV = cosf(0.5f * angle);
		Quaternion result;
		result.x = normal.x * sinV;
		result.y = normal.y * sinV;
		result.z = normal.z * sinV;
		result.w = cosV;
		return result;
#endif
	}

	inline Quaternion Quaternion::from_euler_angles(const Float3& euler_angles)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd;
		vec_float_t v = load_float3a(euler_angles.m);
		Quaternion result;
		store_float4a(result.m, QuaternionRotationRollPitchYawFromVector(v));
		return result;
#else
		return Quaternion::from_roll_pitch_yaw(euler_angles.x, euler_angles.y, euler_angles.z);
#endif
	}

	inline Quaternion Quaternion::from_roll_pitch_yaw(f32 pitch, f32 yaw, f32 roll)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		Quaternion result;
		store_float4a(result.m, QuaternionRotationRollPitchYaw(pitch, yaw, roll));
		return result;
#else
		Float3 sins(sinf(pitch / 2.0f), sinf(yaw / 2.0f), sinf(roll / 2.0f));
		Float3 coss(cosf(pitch / 2.0f), cosf(yaw / 2.0f), cosf(roll / 2.0f));

		Float4 P0(sins.x, coss.x, coss.x, coss.x);
		Float4 Y0(coss.y, sins.y, coss.y, coss.y);
		Float4 R0(coss.z, coss.z, sins.z, coss.z);
		Float4 P1(coss.x, sins.x, sins.x, sins.x);
		Float4 Y1(sins.y, coss.y, sins.y, sins.y);
		Float4 R1(sins.z, sins.z, coss.z, sins.z);

		Float4 Q1(P1.x, -P1.y, -P1.z, P1.w);
		Float4 Q0 = P0 * Y0;

		Q1 *= Y1;
		Q0 *= R0;
		Float4 Q = (Q1 * R1) + Q0;

		Quaternion result;
		result.x = Q.x;
		result.y = Q.y;
		result.z = Q.z;
		result.w = Q.w;
		return result;
#endif
	}

	inline Quaternion lerp(const Quaternion& q1, const Quaternion& q2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t Q0 = load_float4a(q1.m);
		vec_float_t Q1 = load_float4a(q2.m);

		vec_float_t dot = Vector4Dot(Q0, Q1);

		vec_float_t R;
		if (Vector4GreaterOrEqual(dot, vector_zero()))
		{
			R = VectorLerp(Q0, Q1, t);
		}
		else
		{
			vec_float_t tv = vector_replicate(t);
			vec_float_t t1v = vector_replicate(1.f - t);
			vec_float_t X0 = VectorMultiply(Q0, t1v);
			vec_float_t X1 = VectorMultiply(Q1, tv);
			R = VectorSubtract(X0, X1);
		}
		Quaternion result;
		store_float4a(result.m, QuaternionNormalize(R));
		return result;
#else
		Quaternion result;
		f32 dot = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		if (dot >= 0)
		{
			result.x = q1.x + t * (q2.x - q1.x);
			result.y = q1.y + t * (q2.y - q1.y);
			result.z = q1.z + t * (q2.z - q1.z);
			result.w = q1.w + t * (q2.w - q1.w);
		}
		else
		{
			result.x = q1.x * (1.0f - t) - q2.x * t;
			result.y = q1.y * (1.0f - t) - q2.y * t;
			result.z = q1.z * (1.0f - t) - q2.z * t;
			result.w = q1.w * (1.0f - t) - q2.w * t;
		}
		return result;
#endif
	}

	inline Quaternion slerp(const Quaternion& q1, const Quaternion& q2, f32 t)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t Q0 = load_float4a(q1.m);
		vec_float_t Q1 = load_float4a(q2.m);
		Quaternion result;
		store_float4a(result.m, QuaternionSlerp(Q0, Q1, t));
		return result;
#else
		constexpr Float4 OneMinusEpsilon(1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f);

		f32 CosOmega = dot(q1, q2);
		CosOmega = CosOmega < 0 ? -CosOmega : CosOmega;
		f32 SinOmega = sqrtf(1.0f - CosOmega * CosOmega);
		f32 Omega = atan2f(SinOmega, CosOmega);
		// Result = Q0 * sin((1.0 - t) * Omega) / sin(Omega) + Q1 * sin(t * Omega) / sin(Omega)
		Quaternion result;
		result = q1 * (sinf((1.0f - t) * Omega) / sinf(Omega)) + q2 * (sinf(t * Omega) / sinf(Omega));
		return result;
#endif
	}

	inline Quaternion concatenate(const Quaternion& q1, const Quaternion& q2)
	{
#ifdef LUNA_SIMD_ENABLED 
		using namespace Simd; 
		vec_float_t Q0 = load_float4a(q1.m);
		vec_float_t Q1 = load_float4a(q2.m);
		Quaternion result;
		store_float4a(result.m, QuaternionMultiply(Q1, Q0));
		return result;
#else
		return q1 * q2;
#endif
	}
}