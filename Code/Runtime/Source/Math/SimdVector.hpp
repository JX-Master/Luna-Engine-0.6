// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file SimdVector.hpp
* @author JxMaster
* @date 2019/1/4
* @brief Multi-platform SIMD Library modified from Microsoft's DirectX Math.
* See: https://github.com/Microsoft/Directath
 */
#pragma once

#include "SimdConvert.hpp"

#if defined(LUNA_SIMD_ENABLED)

#include <math.h>

namespace Luna
{
	namespace Simd
	{

#if defined(LUNA_PLATFORM_SSE2)

#define LUNA_SIMD_3UNPACK3INTO4(l1,l2,l3) \
    vec_float_t V3 = _mm_shuffle_ps(l2,l3,_MM_SHUFFLE(0,0,3,2));\
    vec_float_t V2 = _mm_shuffle_ps(l2,l1,_MM_SHUFFLE(3,3,1,0));\
    V2 = LUNA_SIMD_PERMUTE_PS(V2,_MM_SHUFFLE(1,1,0,2));\
    vec_float_t V4 = _mm_castsi128_ps( _mm_srli_si128(_mm_castps_si128(L3),32/8) );

#define LUNA_SIMD_3PACK4INTO3(v2x) \
    v2x = _mm_shuffle_ps(V2,V3,_MM_SHUFFLE(1,0,2,1));\
    V2 = _mm_shuffle_ps(V2,V1,_MM_SHUFFLE(2,2,0,0));\
    V1 = _mm_shuffle_ps(V1,V2,_MM_SHUFFLE(0,2,1,0));\
    V3 = _mm_shuffle_ps(V3,V4,_MM_SHUFFLE(0,0,2,2));\
    V3 = _mm_shuffle_ps(V3,V4,_MM_SHUFFLE(2,1,2,0));\

#endif

		/****************************************************************************
		 *
		 * General Vector
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Assignment operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------
		 // Return a vector with all elements equaling zero
		inline vec_float_t LUNA_SIMD_CALLCONV vector_zero()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_f32(0);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_setzero_ps();
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with four floating point values
		inline vec_float_t LUNA_SIMD_CALLCONV vector_set
		(
			f32 x,
			f32 y,
			f32 z,
			f32 w
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t V0 = vcreate_f32(
				static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&x))
				| (static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&y)) << 32));
			float32x2_t V1 = vcreate_f32(
				static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&z))
				| (static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&w)) << 32));
			return vcombine_f32(V0, V1);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_set_ps(w, z, y, x);
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with four integer values
		inline vec_float_t LUNA_SIMD_CALLCONV vector_set_int
		(
			u32 x,
			u32 y,
			u32 z,
			u32 w
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t V0 = vcreate_u32(static_cast<uint64_t>(x) | (static_cast<uint64_t>(y) << 32));
			uint32x2_t V1 = vcreate_u32(static_cast<uint64_t>(z) | (static_cast<uint64_t>(w) << 32));
			return vcombine_u32(V0, V1);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_set_epi32(static_cast<int>(w), static_cast<int>(z), static_cast<int>(y), static_cast<int>(x));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with a replicated floating point value
		inline vec_float_t LUNA_SIMD_CALLCONV vector_replicate
		(
			f32 Value
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_f32(Value);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_set_ps1(Value);
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with a replicated floating point value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV vector_replicate_ptr
		(
			const f32* pValue
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_dup_f32(pValue);
#elif defined(LUNA_PLATFORM_AVX)
			return _mm_broadcast_ss(pValue);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_load_ps1(pValue);
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with a replicated integer value
		inline vec_float_t LUNA_SIMD_CALLCONV vector_replicate_int
		(
			u32 Value
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_u32(Value);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_set1_epi32(static_cast<int>(Value));
			return _mm_castsi128_ps(vTemp);
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with a replicated integer value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV vector_replicate_int_ptr
		(
			const u32 *pValue
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_dup_u32(pValue);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_load_ps1(reinterpret_cast<const f32 *>(pValue));
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with all bits set (true mask)
		inline vec_float_t LUNA_SIMD_CALLCONV vector_true_int()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_s32(-1);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_set1_epi32(-1);
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------
		// Initialize a vector with all bits clear (false mask)
		inline vec_float_t LUNA_SIMD_CALLCONV vector_false_int()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_u32(0);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_setzero_ps();
#endif
		}

		//------------------------------------------------------------------------------
		// Replicate the x component of the vector
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_x
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_lane_f32(vget_low_f32(V), 0);
#elif defined(LUNA_PLATFORM_AVX2)
			return _mm_broadcastss_ps(V);
#elif defined(LUNA_PLATFORM_SSE2)
			return LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
#endif
		}

		//------------------------------------------------------------------------------
		// Replicate the y component of the vector
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_y
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_lane_f32(vget_low_f32(V), 1);
#elif defined(LUNA_PLATFORM_SSE2)
			return LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
#endif
		}

		//------------------------------------------------------------------------------
		// Replicate the z component of the vector
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_z
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_lane_f32(vget_high_f32(V), 0);
#elif defined(LUNA_PLATFORM_SSE2)
			return LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
#endif
		}

		//------------------------------------------------------------------------------
		// Replicate the w component of the vector
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_w
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_lane_f32(vget_high_f32(V), 1);
#elif defined(LUNA_PLATFORM_SSE2)
			return LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
#endif
		}

		//------------------------------------------------------------------------------
		// Return a vector of 1.0f,1.0f,1.0f,1.0f
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_one()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_f32(1.0f);
#elif defined(LUNA_PLATFORM_SSE2)
			return g_One;
#endif
		}

		//------------------------------------------------------------------------------
		// Return a vector of INF,INF,INF,INF
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_infinity()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_u32(0x7F800000);
#elif defined(LUNA_PLATFORM_SSE2)
			return g_Infinity;
#endif
		}

		//------------------------------------------------------------------------------
		// Return a vector of Q_NAN,Q_NAN,Q_NAN,Q_NAN
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_qnan()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_u32(0x7FC00000);
#elif defined(LUNA_PLATFORM_SSE2)
			return g_QNaN;
#endif
		}

		//------------------------------------------------------------------------------
		// Return a vector of 1.192092896e-7f,1.192092896e-7f,1.192092896e-7f,1.192092896e-7f
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_epsilon()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_u32(0x34000000);
#elif defined(LUNA_PLATFORM_SSE2)
			return g_Epsilon;
#endif
		}

		//------------------------------------------------------------------------------
		// Return a vector of -0.0f (0x80000000),-0.0f,-0.0f,-0.0f
		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_sign_mask()
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vdupq_n_u32(0x80000000U);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_set1_epi32(static_cast<int>(0x80000000));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------
		// Return a floating point value via an index. This is not a recommended
		// function to use due to performance loss.
		inline f32 LUNA_SIMD_CALLCONV VectorGetByIndex(vec_float_p1_t V, usize i)
		{
			luassert(i < 4);
			vec_f32_t U;
			U.v = V;
			return U.f[i];
		}

		//------------------------------------------------------------------------------
		// Return the X component in an FPU register. 
		inline f32 LUNA_SIMD_CALLCONV VectorGetX(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_f32(V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cvtss_f32(V);
#endif
		}

		// Return the Y component in an FPU register. 
		inline f32 LUNA_SIMD_CALLCONV VectorGetY(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_f32(V, 1);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			return _mm_cvtss_f32(vTemp);
#endif
		}

		// Return the Z component in an FPU register. 
		inline f32 LUNA_SIMD_CALLCONV VectorGetZ(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_f32(V, 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			return _mm_cvtss_f32(vTemp);
#endif
		}

		// Return the W component in an FPU register. 
		inline f32 LUNA_SIMD_CALLCONV VectorGetW(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_f32(V, 3);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
			return _mm_cvtss_f32(vTemp);
#endif
		}

		//------------------------------------------------------------------------------

		// Store a component indexed by i into a 32 bit f32 location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetByIndexPtr(f32 *f, vec_float_p1_t V, usize i)
		{
			luassert(f != nullptr);
			luassert(i < 4);
			vec_f32_t U;
			U.v = V;
			*f = U.f[i];
		}

		//------------------------------------------------------------------------------

		// Store the X component into a 32 bit f32 location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetXPtr(f32 *x, vec_float_p1_t V)
		{
			luassert(x != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_f32(x, V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ss(x, V);
#endif
		}

		// Store the Y component into a 32 bit f32 location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetYPtr(f32 *y, vec_float_p1_t V)
		{
			luassert(y != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_f32(y, V, 1);
#elif defined(LUNA_PLATFORM_SSE4)
			*(reinterpret_cast<int*>(y)) = _mm_extract_ps(V, 1);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			_mm_store_ss(y, vResult);
#endif
		}

		// Store the Z component into a 32 bit f32 location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetZPtr(f32 *z, vec_float_p1_t V)
		{
			luassert(z != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_f32(z, V, 2);
#elif defined(LUNA_PLATFORM_SSE4)
			*(reinterpret_cast<int*>(z)) = _mm_extract_ps(V, 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_store_ss(z, vResult);
#endif
		}

		// Store the W component into a 32 bit f32 location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetWPtr(f32 *w, vec_float_p1_t V)
		{
			luassert(w != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_f32(w, V, 3);
#elif defined(LUNA_PLATFORM_SSE4)
			*(reinterpret_cast<int*>(w)) = _mm_extract_ps(V, 3);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
			_mm_store_ss(w, vResult);
#endif
		}

		//------------------------------------------------------------------------------

		// Return an integer value via an index. This is not a recommended
		// function to use due to performance loss.
		inline u32 LUNA_SIMD_CALLCONV VectorGetIntByIndex(vec_float_p1_t V, usize i)
		{
			luassert(i < 4);
			vec_u32_t U;
			U.v = V;
			return U.u[i];
		}

		//------------------------------------------------------------------------------

		// Return the X component in an integer register. 
		inline u32 LUNA_SIMD_CALLCONV VectorGetIntX(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_u32(V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			return static_cast<u32>(_mm_cvtsi128_si32(_mm_castps_si128(V)));
#endif
		}

		// Return the Y component in an integer register. 
		inline u32 LUNA_SIMD_CALLCONV VectorGetIntY(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_u32(V, 1);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i V1 = _mm_castps_si128(V);
			return static_cast<u32>(_mm_extract_epi32(V1, 1));
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vResulti = _mm_shuffle_epi32(_mm_castps_si128(V), _MM_SHUFFLE(1, 1, 1, 1));
			return static_cast<u32>(_mm_cvtsi128_si32(vResulti));
#endif
		}

		// Return the Z component in an integer register. 
		inline u32 LUNA_SIMD_CALLCONV VectorGetIntZ(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_u32(V, 2);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i V1 = _mm_castps_si128(V);
			return static_cast<u32>(_mm_extract_epi32(V1, 2));
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vResulti = _mm_shuffle_epi32(_mm_castps_si128(V), _MM_SHUFFLE(2, 2, 2, 2));
			return static_cast<u32>(_mm_cvtsi128_si32(vResulti));
#endif
		}

		// Return the W component in an integer register. 
		inline u32 LUNA_SIMD_CALLCONV VectorGetIntW(vec_float_p1_t V)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vgetq_lane_u32(V, 3);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i V1 = _mm_castps_si128(V);
			return static_cast<u32>(_mm_extract_epi32(V1, 3));
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vResulti = _mm_shuffle_epi32(_mm_castps_si128(V), _MM_SHUFFLE(3, 3, 3, 3));
			return static_cast<u32>(_mm_cvtsi128_si32(vResulti));
#endif
		}

		//------------------------------------------------------------------------------

		// Store a component indexed by i into a 32 bit integer location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetIntByIndexPtr(u32 *x, vec_float_p1_t V, usize i)
		{
			luassert(x != nullptr);
			luassert(i < 4);
			vec_u32_t U;
			U.v = V;
			*x = U.u[i];
		}

		//------------------------------------------------------------------------------

		// Store the X component into a 32 bit integer location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetIntXPtr(u32 *x, vec_float_p1_t V)
		{
			luassert(x != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_u32(x, *reinterpret_cast<const uint32x4_t*>(&V), 0);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ss(reinterpret_cast<f32 *>(x), V);
#endif
		}

		// Store the Y component into a 32 bit integer location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetIntYPtr(u32 *y, vec_float_p1_t V)
		{
			luassert(y != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_u32(y, *reinterpret_cast<const uint32x4_t*>(&V), 1);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i V1 = _mm_castps_si128(V);
			*y = static_cast<u32>(_mm_extract_epi32(V1, 1));
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			_mm_store_ss(reinterpret_cast<f32 *>(y), vResult);
#endif
		}

		// Store the Z component into a 32 bit integer locaCantion in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetIntZPtr(u32 *z, vec_float_p1_t V)
		{
			luassert(z != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_u32(z, *reinterpret_cast<const uint32x4_t*>(&V), 2);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i V1 = _mm_castps_si128(V);
			*z = static_cast<u32>(_mm_extract_epi32(V1, 2));
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_store_ss(reinterpret_cast<f32 *>(z), vResult);
#endif
		}

		// Store the W component into a 32 bit integer location in memory.

		inline void LUNA_SIMD_CALLCONV VectorGetIntWPtr(u32 *w, vec_float_p1_t V)
		{
			luassert(w != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_u32(w, *reinterpret_cast<const uint32x4_t*>(&V), 3);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i V1 = _mm_castps_si128(V);
			*w = static_cast<u32>(_mm_extract_epi32(V1, 3));
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
			_mm_store_ss(reinterpret_cast<f32 *>(w), vResult);
#endif
		}

		//------------------------------------------------------------------------------

		// Set a single indexed floating point component
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetByIndex(vec_float_p1_t V, f32 f, usize i)
		{
			luassert(i < 4);
			vec_f32_t U;
			U.v = V;
			U.f[i] = f;
			return U.v;
		}

		//------------------------------------------------------------------------------

		// Sets the X component of a vector to a passed floating point value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetX(vec_float_p1_t V, f32 x)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_f32(x, V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_set_ss(x);
			vResult = _mm_move_ss(V, vResult);
			return vResult;
#endif
		}

		// Sets the Y component of a vector to a passed floating point value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetY(vec_float_p1_t V, f32 y)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_f32(y, V, 1);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vResult = _mm_set_ss(y);
			vResult = _mm_insert_ps(V, vResult, 0x10);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap y and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 2, 0, 1));
			// Convert input to vector
			vec_float_t vTemp = _mm_set_ss(y);
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap y and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 2, 0, 1));
			return vResult;
#endif
		}
		// Sets the Z component of a vector to a passed floating point value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetZ(vec_float_p1_t V, f32 z)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_f32(z, V, 2);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vResult = _mm_set_ss(z);
			vResult = _mm_insert_ps(V, vResult, 0x20);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap z and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 0, 1, 2));
			// Convert input to vector
			vec_float_t vTemp = _mm_set_ss(z);
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap z and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 0, 1, 2));
			return vResult;
#endif
		}

		// Sets the W component of a vector to a passed floating point value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetW(vec_float_p1_t V, f32 w)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_f32(w, V, 3);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vResult = _mm_set_ss(w);
			vResult = _mm_insert_ps(V, vResult, 0x30);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap w and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 2, 1, 3));
			// Convert input to vector
			vec_float_t vTemp = _mm_set_ss(w);
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap w and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(0, 2, 1, 3));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		// Sets a component of a vector to a floating point value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetByIndexPtr(vec_float_p1_t V, const f32 *f, usize i)
		{
			luassert(f != nullptr);
			luassert(i < 4);
			vec_f32_t U;
			U.v = V;
			U.f[i] = *f;
			return U.v;
		}

		//------------------------------------------------------------------------------

		// Sets the X component of a vector to a floating point value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetXPtr(vec_float_p1_t V, const f32 *x)
		{
			luassert(x != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_f32(x, V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_load_ss(x);
			vResult = _mm_move_ss(V, vResult);
			return vResult;
#endif
		}

		// Sets the Y component of a vector to a floating point value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetYPtr(vec_float_p1_t V, const f32 *y)
		{
			luassert(y != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_f32(y, V, 1);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap y and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 2, 0, 1));
			// Convert input to vector
			vec_float_t vTemp = _mm_load_ss(y);
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap y and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 2, 0, 1));
			return vResult;
#endif
		}

		// Sets the Z component of a vector to a floating point value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetZPtr(vec_float_p1_t V, const f32 *z)
		{
			luassert(z != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_f32(z, V, 2);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap z and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 0, 1, 2));
			// Convert input to vector
			vec_float_t vTemp = _mm_load_ss(z);
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap z and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 0, 1, 2));
			return vResult;
#endif
		}

		// Sets the W component of a vector to a floating point value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetWPtr(vec_float_p1_t V, const f32 *w)
		{
			luassert(w != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_f32(w, V, 3);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap w and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 2, 1, 3));
			// Convert input to vector
			vec_float_t vTemp = _mm_load_ss(w);
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap w and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(0, 2, 1, 3));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		// Sets a component of a vector to an integer passed by value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntByIndex(vec_float_p1_t V, u32 x, usize i)
		{
			luassert(i < 4);
			vec_u32_t tmp;
			tmp.v = V;
			tmp.u[i] = x;
			return tmp;
		}

		//------------------------------------------------------------------------------

		// Sets the X component of a vector to an integer passed by value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntX(vec_float_p1_t V, u32 x)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_u32(x, V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cvtsi32_si128(static_cast<int>(x));
			vec_float_t vResult = _mm_move_ss(V, _mm_castsi128_ps(vTemp));
			return vResult;
#endif
		}

		// Sets the Y component of a vector to an integer passed by value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntY(vec_float_p1_t V, u32 y)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_u32(y, V, 1);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i vResult = _mm_castps_si128(V);
			vResult = _mm_insert_epi32(vResult, static_cast<int>(y), 1);
			return _mm_castsi128_ps(vResult);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap y and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 2, 0, 1));
			// Convert input to vector
			__m128i vTemp = _mm_cvtsi32_si128(static_cast<int>(y));
			// Replace the x component
			vResult = _mm_move_ss(vResult, _mm_castsi128_ps(vTemp));
			// Swap y and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 2, 0, 1));
			return vResult;
#endif
		}

		// Sets the Z component of a vector to an integer passed by value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntZ(vec_float_p1_t V, u32 z)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_u32(z, V, 2);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i vResult = _mm_castps_si128(V);
			vResult = _mm_insert_epi32(vResult, static_cast<int>(z), 2);
			return _mm_castsi128_ps(vResult);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap z and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 0, 1, 2));
			// Convert input to vector
			__m128i vTemp = _mm_cvtsi32_si128(static_cast<int>(z));
			// Replace the x component
			vResult = _mm_move_ss(vResult, _mm_castsi128_ps(vTemp));
			// Swap z and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 0, 1, 2));
			return vResult;
#endif
		}

		// Sets the W component of a vector to an integer passed by value
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntW(vec_float_p1_t V, u32 w)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsetq_lane_u32(w, V, 3);
#elif defined(LUNA_PLATFORM_SSE4)
			__m128i vResult = _mm_castps_si128(V);
			vResult = _mm_insert_epi32(vResult, static_cast<int>(w), 3);
			return _mm_castsi128_ps(vResult);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap w and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 2, 1, 3));
			// Convert input to vector
			__m128i vTemp = _mm_cvtsi32_si128(static_cast<int>(w));
			// Replace the x component
			vResult = _mm_move_ss(vResult, _mm_castsi128_ps(vTemp));
			// Swap w and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(0, 2, 1, 3));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		// Sets a component of a vector to an integer value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntByIndexPtr(vec_float_p1_t V, const u32 *x, usize i)
		{
			luassert(x != nullptr);
			luassert(i < 4);
			vec_u32_t tmp;
			tmp.v = V;
			tmp.u[i] = *x;
			return tmp;
		}

		//------------------------------------------------------------------------------

		// Sets the X component of a vector to an integer value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntXPtr(vec_float_p1_t V, const u32 *x)
		{
			luassert(x != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_u32(x, *reinterpret_cast<const uint32x4_t *>(&V), 0);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_load_ss(reinterpret_cast<const f32 *>(x));
			vec_float_t vResult = _mm_move_ss(V, vTemp);
			return vResult;
#endif
		}

		// Sets the Y component of a vector to an integer value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntYPtr(vec_float_p1_t V, const u32 *y)
		{
			luassert(y != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_u32(y, *reinterpret_cast<const uint32x4_t *>(&V), 1);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap y and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 2, 0, 1));
			// Convert input to vector
			vec_float_t vTemp = _mm_load_ss(reinterpret_cast<const f32 *>(y));
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap y and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 2, 0, 1));
			return vResult;
#endif
		}

		// Sets the Z component of a vector to an integer value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntZPtr(vec_float_p1_t V, const u32 *z)
		{
			luassert(z != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_u32(z, *reinterpret_cast<const uint32x4_t *>(&V), 2);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap z and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 0, 1, 2));
			// Convert input to vector
			vec_float_t vTemp = _mm_load_ss(reinterpret_cast<const f32 *>(z));
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap z and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 0, 1, 2));
			return vResult;
#endif
		}

		// Sets the W component of a vector to an integer value passed by pointer

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSetIntWPtr(vec_float_p1_t V, const u32 *w)
		{
			luassert(w != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_lane_u32(w, *reinterpret_cast<const uint32x4_t *>(&V), 3);
#elif defined(LUNA_PLATFORM_SSE2)
			// Swap w and x
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 2, 1, 3));
			// Convert input to vector
			vec_float_t vTemp = _mm_load_ss(reinterpret_cast<const f32 *>(w));
			// Replace the x component
			vResult = _mm_move_ss(vResult, vTemp);
			// Swap w and x again
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(0, 2, 1, 3));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSwizzle
		(
			vec_float_p1_t V,
			u32 E0,
			u32 E1,
			u32 E2,
			u32 E3
		)
		{
			luassert((E0 < 4) && (E1 < 4) && (E2 < 4) && (E3 < 4));
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const u32 ControlElement[4] =
			{
				0x03020100, // g_swizzle_x
				0x07060504, // g_swizzle_y
				0x0B0A0908, // g_swizzle_z
				0x0F0E0D0C, // g_swizzle_w
			};

			int8x8x2_t tbl;
			tbl.val[0] = vget_low_f32(V);
			tbl.val[1] = vget_high_f32(V);

			uint32x2_t idx = vcreate_u32(static_cast<uint64_t>(ControlElement[E0]) | (static_cast<uint64_t>(ControlElement[E1]) << 32));
			const uint8x8_t rL = vtbl2_u8(tbl, idx);

			idx = vcreate_u32(static_cast<uint64_t>(ControlElement[E2]) | (static_cast<uint64_t>(ControlElement[E3]) << 32));
			const uint8x8_t rH = vtbl2_u8(tbl, idx);

			return vcombine_f32(rL, rH);
#elif defined(LUNA_PLATFORM_AVX)
			unsigned int elem[4] = { E0, E1, E2, E3 };
			__m128i vControl = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&elem[0]));
			return _mm_permutevar_ps(V, vControl);
#else
			auto aPtr = reinterpret_cast<const u32*>(&V);

			vec_float_t Result;
			auto pWork = reinterpret_cast<u32*>(&Result);

			pWork[0] = aPtr[E0];
			pWork[1] = aPtr[E1];
			pWork[2] = aPtr[E2];
			pWork[3] = aPtr[E3];

			return Result;
#endif
		}

		//------------------------------------------------------------------------------
		inline vec_float_t LUNA_SIMD_CALLCONV VectorPermute
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			u32 PermuteX,
			u32 PermuteY,
			u32 PermuteZ,
			u32 PermuteW
		)
		{
			luassert(PermuteX <= 7 && PermuteY <= 7 && PermuteZ <= 7 && PermuteW <= 7);

#if defined(LUNA_PLATFORM_ARM_NEON)
			static const u32 ControlElement[8] =
			{
				0x03020100, // g_permute_0x
				0x07060504, // g_permute_0y
				0x0B0A0908, // g_permute_0z
				0x0F0E0D0C, // g_permute_0w
				0x13121110, // g_permute_1x
				0x17161514, // g_permute_1y
				0x1B1A1918, // g_permute_1z
				0x1F1E1D1C, // g_permute_1w
			};

			int8x8x4_t tbl;
			tbl.val[0] = vget_low_f32(V1);
			tbl.val[1] = vget_high_f32(V1);
			tbl.val[2] = vget_low_f32(V2);
			tbl.val[3] = vget_high_f32(V2);

			uint32x2_t idx = vcreate_u32(static_cast<uint64_t>(ControlElement[PermuteX]) | (static_cast<uint64_t>(ControlElement[PermuteY]) << 32));
			const uint8x8_t rL = vtbl4_u8(tbl, idx);

			idx = vcreate_u32(static_cast<uint64_t>(ControlElement[PermuteZ]) | (static_cast<uint64_t>(ControlElement[PermuteW]) << 32));
			const uint8x8_t rH = vtbl4_u8(tbl, idx);

			return vcombine_f32(rL, rH);
#elif defined(LUNA_PLATFORM_AVX)
			static const vec_u32_t three = { { { 3, 3, 3, 3 } } };

			_declspec(align(16)) unsigned int elem[4] = { PermuteX, PermuteY, PermuteZ, PermuteW };
			__m128i vControl = _mm_load_si128(reinterpret_cast<const __m128i *>(&elem[0]));

			__m128i vSelect = _mm_cmpgt_epi32(vControl, three);
			vControl = _mm_castps_si128(_mm_and_ps(_mm_castsi128_ps(vControl), three));

			__m128 shuffled1 = _mm_permutevar_ps(V1, vControl);
			__m128 shuffled2 = _mm_permutevar_ps(V2, vControl);

			__m128 masked1 = _mm_andnot_ps(_mm_castsi128_ps(vSelect), shuffled1);
			__m128 masked2 = _mm_and_ps(_mm_castsi128_ps(vSelect), shuffled2);

			return _mm_or_ps(masked1, masked2);
#else

			const u32 *aPtr[2];
			aPtr[0] = reinterpret_cast<const u32*>(&V1);
			aPtr[1] = reinterpret_cast<const u32*>(&V2);

			vec_float_t Result;
			auto pWork = reinterpret_cast<u32*>(&Result);

			const u32 i0 = PermuteX & 3;
			const u32 vi0 = PermuteX >> 2;
			pWork[0] = aPtr[vi0][i0];

			const u32 i1 = PermuteY & 3;
			const u32 vi1 = PermuteY >> 2;
			pWork[1] = aPtr[vi1][i1];

			const u32 i2 = PermuteZ & 3;
			const u32 vi2 = PermuteZ >> 2;
			pWork[2] = aPtr[vi2][i2];

			const u32 i3 = PermuteW & 3;
			const u32 vi3 = PermuteW >> 2;
			pWork[3] = aPtr[vi3][i3];

			return Result;
#endif
		}

		//------------------------------------------------------------------------------
		// Define a control vector to be used in VectorSelect 
		// operations.  The four integers specified in VectorSelectControl
		// serve as indices to select between components in two vectors.
		// The first index controls selection for the first component of 
		// the vectors involved in a select operation, the second index 
		// controls selection for the second component etc.  A value of
		// zero for an index causes the corresponding component from the first 
		// vector to be selected whereas a one causes the component from the
		// second vector to be selected instead.

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSelectControl
		(
			u32 VectorIndex0,
			u32 VectorIndex1,
			u32 VectorIndex2,
			u32 VectorIndex3
		)
		{
#if defined(LUNA_PLATFORM_SSE2)
			// x=Index0,y=Index1,z=Index2,w=Index3
			__m128i vTemp = _mm_set_epi32(static_cast<int>(VectorIndex3), static_cast<int>(VectorIndex2), static_cast<int>(VectorIndex1), static_cast<int>(VectorIndex0));
			// Any non-zero entries become 0xFFFFFFFF else 0
			vTemp = _mm_cmpgt_epi32(vTemp, g_Zero);
			return _mm_castsi128_ps(vTemp);
#elif defined(LUNA_PLATFORM_ARM_NEON)
			int32x2_t V0 = vcreate_s32(static_cast<uint64_t>(VectorIndex0) | (static_cast<uint64_t>(VectorIndex1) << 32));
			int32x2_t V1 = vcreate_s32(static_cast<uint64_t>(VectorIndex2) | (static_cast<uint64_t>(VectorIndex3) << 32));
			int32x4_t vTemp = vcombine_s32(V0, V1);
			// Any non-zero entries become 0xFFFFFFFF else 0
			return vcgtq_s32(vTemp, g_Zero);
#else
			vec_float_t    ControlVector;
			const u32  ControlElement[] =
			{
				g_select_0,
				g_select_1
			};

			luassert(VectorIndex0 < 2);
			luassert(VectorIndex1 < 2);
			luassert(VectorIndex2 < 2);
			luassert(VectorIndex3 < 2);

			ControlVector.vector4_u32[0] = ControlElement[VectorIndex0];
			ControlVector.vector4_u32[1] = ControlElement[VectorIndex1];
			ControlVector.vector4_u32[2] = ControlElement[VectorIndex2];
			ControlVector.vector4_u32[3] = ControlElement[VectorIndex3];

			return ControlVector;

#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSelect
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t Control
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vbslq_f32(Control, V2, V1);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp1 = _mm_andnot_ps(Control, V1);
			vec_float_t vTemp2 = _mm_and_ps(V2, Control);
			return _mm_or_ps(vTemp1, vTemp2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMergeXY
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vzipq_f32(V1, V2).val[0];
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_unpacklo_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMergeZW
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vzipq_f32(V1, V2).val[1];
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_unpackhi_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorShiftLeft(vec_float_p1_t V1, vec_float_p1_t V2, u32 Elements)
		{
			luassert(Elements < 4);
			return VectorPermute(V1, V2, Elements, ((Elements)+1), ((Elements)+2), ((Elements)+3));
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorRotateLeft(vec_float_p1_t V, u32 Elements)
		{
			luassert(Elements < 4);
			return VectorSwizzle(V, Elements & 3, (Elements + 1) & 3, (Elements + 2) & 3, (Elements + 3) & 3);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorRotateRight(vec_float_p1_t V, u32 Elements)
		{
			luassert(Elements < 4);
			return VectorSwizzle(V, (4 - (Elements)) & 3, (5 - (Elements)) & 3, (6 - (Elements)) & 3, (7 - (Elements)) & 3);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorInsert(vec_float_p1_t VD, vec_float_p1_t VS, u32 VSLeftRotateElements,
			u32 Select0, u32 Select1, u32 Select2, u32 Select3)
		{
			vec_float_t Control = VectorSelectControl(Select0 & 1, Select1 & 1, Select2 & 1, Select3 & 1);
			return VectorSelect(VD, VectorRotateLeft(VS, VSLeftRotateElements), Control);
		}

		//------------------------------------------------------------------------------
		// Comparison operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vceqq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cmpeq_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------


		inline vec_float_t LUNA_SIMD_CALLCONV VectorEqualR
		(
			u32*    pCR,
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			luassert(pCR != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);
			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				// All elements are equal
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				// All elements are not equal
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			u32 CR = 0;
			int iTest = _mm_movemask_ps(vTemp);
			if (iTest == 0xf)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				// All elements are not greater
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vTemp;
#endif
		}

		//------------------------------------------------------------------------------
		// Treat the components of the vectors as unsigned integers and
		// compare individual bits between the two.  This is useful for
		// comparing control vectors and result vectors returned from
		// other comparison operations.

		inline vec_float_t LUNA_SIMD_CALLCONV VectorEqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vceqq_u32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------


		inline vec_float_t LUNA_SIMD_CALLCONV VectorEqualIntR
		(
			u32*    pCR,
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			luassert(pCR != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);
			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				// All elements are equal
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				// All elements are not equal
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			int iTemp = _mm_movemask_ps(_mm_castsi128_ps(V));
			u32 CR = 0;
			if (iTemp == 0x0F)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTemp)
			{
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorNearEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t Epsilon
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t vDelta = vsubq_f32(V1, V2);
			return vacleq_f32(vDelta, Epsilon);
#elif defined(LUNA_PLATFORM_SSE2)
			// Get the difference
			vec_float_t vDelta = _mm_sub_ps(V1, V2);
			// Get the absolute value of the difference
			vec_float_t vTemp = _mm_setzero_ps();
			vTemp = _mm_sub_ps(vTemp, vDelta);
			vTemp = _mm_max_ps(vTemp, vDelta);
			vTemp = _mm_cmple_ps(vTemp, Epsilon);
			return vTemp;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorNotEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vmvnq_u32(vceqq_f32(V1, V2));
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cmpneq_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorNotEqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vmvnq_u32(vceqq_u32(V1, V2));
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return _mm_xor_ps(_mm_castsi128_ps(V), g_NegOneMask);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorGreater
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vcgtq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cmpgt_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------


		inline vec_float_t LUNA_SIMD_CALLCONV VectorGreaterR
		(
			u32*    pCR,
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			luassert(pCR != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgtq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);
			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				// All elements are greater
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				// All elements are not greater
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			u32 CR = 0;
			int iTest = _mm_movemask_ps(vTemp);
			if (iTest == 0xf)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				// All elements are not greater
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vTemp;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorGreaterOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vcgeq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cmpge_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------


		inline vec_float_t LUNA_SIMD_CALLCONV VectorGreaterOrEqualR
		(
			u32*    pCR,
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			luassert(pCR != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgeq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);
			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				// All elements are greater or equal
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				// All elements are not greater or equal
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			u32 CR = 0;
			int iTest = _mm_movemask_ps(vTemp);
			if (iTest == 0xf)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				// All elements are not greater
				CR = g_crmask_cr6false;
			}
			*pCR = CR;
			return vTemp;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLess
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vcltq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cmplt_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLessOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vcleq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_cmple_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorInBounds
		(
			vec_float_p1_t V,
			vec_float_p1_t Bounds
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test if less than or equal
			vec_float_t vTemp1 = vcleq_f32(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = vnegq_f32(Bounds);
			// Test if greater or equal (Reversed)
			vTemp2 = vcleq_f32(vTemp2, V);
			// Blend answers
			vTemp1 = vandq_u32(vTemp1, vTemp2);
			return vTemp1;
#elif defined(LUNA_PLATFORM_SSE2)
			// Test if less than or equal
			vec_float_t vTemp1 = _mm_cmple_ps(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = _mm_mul_ps(Bounds, g_NegativeOne);
			// Test if greater or equal (Reversed)
			vTemp2 = _mm_cmple_ps(vTemp2, V);
			// Blend answers
			vTemp1 = _mm_and_ps(vTemp1, vTemp2);
			return vTemp1;
#endif
		}

		//------------------------------------------------------------------------------


		inline vec_float_t LUNA_SIMD_CALLCONV VectorInBoundsR
		(
			u32*    pCR,
			vec_float_p1_t V,
			vec_float_p1_t Bounds
		)
		{
			luassert(pCR != nullptr);
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test if less than or equal
			vec_float_t vTemp1 = vcleq_f32(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = vnegq_f32(Bounds);
			// Test if greater or equal (Reversed)
			vTemp2 = vcleq_f32(vTemp2, V);
			// Blend answers
			vTemp1 = vandq_u32(vTemp1, vTemp2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTemp1), vget_high_u8(vTemp1));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);
			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				// All elements are in bounds
				CR = g_crmask_cr6bounds;
			}
			*pCR = CR;
			return vTemp1;
#elif defined(LUNA_PLATFORM_SSE2)
			// Test if less than or equal
			vec_float_t vTemp1 = _mm_cmple_ps(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = _mm_mul_ps(Bounds, g_NegativeOne);
			// Test if greater or equal (Reversed)
			vTemp2 = _mm_cmple_ps(vTemp2, V);
			// Blend answers
			vTemp1 = _mm_and_ps(vTemp1, vTemp2);

			u32 CR = 0;
			if (_mm_movemask_ps(vTemp1) == 0xf) {
				// All elements are in bounds
				CR = g_crmask_cr6bounds;
			}
			*pCR = CR;
			return vTemp1;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorIsNaN
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test against itself. NaN is always not equal
			uint32x4_t vTempNan = vceqq_f32(V, V);
			// Flip results
			return vmvnq_u32(vTempNan);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test against itself. NaN is always not equal
			return _mm_cmpneq_ps(V, V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorIsInfinite
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Mask off the sign bit
			uint32x4_t vTemp = vandq_u32(V, g_AbsMask);
			// Compare to infinity
			vTemp = vceqq_f32(vTemp, g_Infinity);
			// If any are infinity, the signs are true.
			return vTemp;
#elif defined(LUNA_PLATFORM_SSE2)
			// Mask off the sign bit
			__m128 vTemp = _mm_and_ps(V, g_AbsMask);
			// Compare to infinity
			vTemp = _mm_cmpeq_ps(vTemp, g_Infinity);
			// If any are infinity, the signs are true.
			return vTemp;
#endif
		}

		//------------------------------------------------------------------------------
		// Rounding and clamping operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMin
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vminq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_min_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMax
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vmaxq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_max_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		namespace Internal
		{
			// Round to nearest (even) a.k.a. banker's rounding
			inline f32 round_to_nearest(f32 x)
			{
				f32 i = floorf(x);
				x -= i;
				if (x < 0.5f)
					return i;
				if (x > 0.5f)
					return i + 1.f;

				f32 int_part;
				(void)modff(i / 2.f, &int_part);
				if ((2.f*int_part) == i)
				{
					return i;
				}

				return i + 1.f;
			}
		}

#if defined(LUNA_COMPILER_MSVC)
#pragma float_control(push)
#pragma float_control(precise, on)
#endif

		inline vec_float_t LUNA_SIMD_CALLCONV VectorRound
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
			return vrndnq_f32(V);
#else
			uint32x4_t sign = vandq_u32(V, g_NegativeZero);
			uint32x4_t sMagic = vorrq_u32(g_NoFraction, sign);
			float32x4_t R1 = vaddq_f32(V, sMagic);
			R1 = vsubq_f32(R1, sMagic);
			float32x4_t R2 = vabsq_f32(V);
			uint32x4_t mask = vcleq_f32(R2, g_NoFraction);
			vec_float_t vResult = vbslq_f32(mask, R1, V);
			return vResult;
#endif
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_round_ps(V, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 sign = _mm_and_ps(V, g_NegativeZero);
			__m128 sMagic = _mm_or_ps(g_NoFraction, sign);
			__m128 R1 = _mm_add_ps(V, sMagic);
			R1 = _mm_sub_ps(R1, sMagic);
			__m128 R2 = _mm_and_ps(V, g_AbsMask);
			__m128 mask = _mm_cmple_ps(R2, g_NoFraction);
			R2 = _mm_andnot_ps(mask, V);
			R1 = _mm_and_ps(R1, mask);
			vec_float_t vResult = _mm_xor_ps(R1, R2);
			return vResult;
#endif
		}

#if defined(LUNA_COMPILER_MSVC)
#pragma float_control(pop)
#endif

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorTruncate
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
			return vrndq_f32(V);
#else
			float32x4_t vTest = vabsq_f32(V);
			vTest = vcltq_f32(vTest, g_NoFraction);

			int32x4_t vInt = vcvtq_s32_f32(V);
			vec_float_t vResult = vcvtq_f32_s32(vInt);

			// All numbers less than 8388608 will use the round to int
			// All others, use the ORIGINAL value
			return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_round_ps(V, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
#elif defined(LUNA_PLATFORM_SSE2)
			// To handle NAN, INF and numbers greater than 8388608, use masking
			// Get the abs value
			__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			// Test for greater than 8388608 (All floats with NO fractionals, NAN and INF
			vTest = _mm_cmplt_epi32(vTest, g_NoFraction);
			// Convert to int and back to f32 for rounding with truncation
			__m128i vInt = _mm_cvttps_epi32(V);
			// Convert back to floats
			vec_float_t vResult = _mm_cvtepi32_ps(vInt);
			// All numbers less than 8388608 will use the round to int
			vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
			// All others, use the ORIGINAL value
			vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
			vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorFloor
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
			return vrndmq_f32(V);
#else
			float32x4_t vTest = vabsq_f32(V);
			vTest = vcltq_f32(vTest, g_NoFraction);
			// Truncate
			int32x4_t vInt = vcvtq_s32_f32(V);
			vec_float_t vResult = vcvtq_f32_s32(vInt);
			vec_float_t vLarger = vcgtq_f32(vResult, V);
			// 0 -> 0, 0xffffffff -> -1.0f
			vLarger = vcvtq_f32_s32(vLarger);
			vResult = vaddq_f32(vResult, vLarger);
			// All numbers less than 8388608 will use the round to int
			// All others, use the ORIGINAL value
			return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_floor_ps(V);
#elif defined(LUNA_PLATFORM_SSE2)
			// To handle NAN, INF and numbers greater than 8388608, use masking
			__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			vTest = _mm_cmplt_epi32(vTest, g_NoFraction);
			// Truncate
			__m128i vInt = _mm_cvttps_epi32(V);
			vec_float_t vResult = _mm_cvtepi32_ps(vInt);
			__m128 vLarger = _mm_cmpgt_ps(vResult, V);
			// 0 -> 0, 0xffffffff -> -1.0f
			vLarger = _mm_cvtepi32_ps(_mm_castps_si128(vLarger));
			vResult = _mm_add_ps(vResult, vLarger);
			// All numbers less than 8388608 will use the round to int
			vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
			// All others, use the ORIGINAL value
			vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
			vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorCeiling
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
			return vrndpq_f32(V);
#else
			float32x4_t vTest = vabsq_f32(V);
			vTest = vcltq_f32(vTest, g_NoFraction);
			// Truncate
			int32x4_t vInt = vcvtq_s32_f32(V);
			vec_float_t vResult = vcvtq_f32_s32(vInt);
			vec_float_t vSmaller = vcltq_f32(vResult, V);
			// 0 -> 0, 0xffffffff -> -1.0f
			vSmaller = vcvtq_f32_s32(vSmaller);
			vResult = vsubq_f32(vResult, vSmaller);
			// All numbers less than 8388608 will use the round to int
			// All others, use the ORIGINAL value
			return vbslq_f32(vTest, vResult, V);
#endif
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_ceil_ps(V);
#elif defined(LUNA_PLATFORM_SSE2)
			// To handle NAN, INF and numbers greater than 8388608, use masking
			__m128i vTest = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			vTest = _mm_cmplt_epi32(vTest, g_NoFraction);
			// Truncate
			__m128i vInt = _mm_cvttps_epi32(V);
			vec_float_t vResult = _mm_cvtepi32_ps(vInt);
			__m128 vSmaller = _mm_cmplt_ps(vResult, V);
			// 0 -> 0, 0xffffffff -> -1.0f
			vSmaller = _mm_cvtepi32_ps(_mm_castps_si128(vSmaller));
			vResult = _mm_sub_ps(vResult, vSmaller);
			// All numbers less than 8388608 will use the round to int
			vResult = _mm_and_ps(vResult, _mm_castsi128_ps(vTest));
			// All others, use the ORIGINAL value
			vTest = _mm_andnot_si128(vTest, _mm_castps_si128(V));
			vResult = _mm_or_ps(vResult, _mm_castsi128_ps(vTest));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorClamp
		(
			vec_float_p1_t V,
			vec_float_p1_t min,
			vec_float_p1_t max
		)
		{
			luassert(Vector4LessOrEqual(min, max));

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t vResult;
			vResult = vmaxq_f32(Min, V);
			vResult = vminq_f32(Max, vResult);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult;
			vResult = _mm_max_ps(min, V);
			vResult = _mm_min_ps(max, vResult);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSaturate
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Set <0 to 0
			vec_float_t vResult = vmaxq_f32(V, vdupq_n_f32(0));
			// Set>1 to 1
			return vminq_f32(vResult, vdupq_n_f32(1.0f));
#elif defined(LUNA_PLATFORM_SSE2)
			// Set <0 to 0
			vec_float_t vResult = _mm_max_ps(V, g_Zero);
			// Set>1 to 1
			return _mm_min_ps(vResult, g_One);
#endif
		}

		//------------------------------------------------------------------------------
		// Bitwise logical operations
		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorAndInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vandq_u32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_and_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorAndCInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vbicq_u32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_andnot_si128(_mm_castps_si128(V2), _mm_castps_si128(V1));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorOrInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vorrq_u32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_or_si128(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorNorInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t Result = vorrq_u32(V1, V2);
			return vbicq_u32(g_NegOneMask, Result);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i Result;
			Result = _mm_or_si128(_mm_castps_si128(V1), _mm_castps_si128(V2));
			Result = _mm_andnot_si128(Result, g_NegOneMask);
			return _mm_castsi128_ps(Result);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorXorInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return veorq_u32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_xor_si128(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorNegate
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vnegq_f32(V);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t Z;

			Z = _mm_setzero_ps();

			return _mm_sub_ps(Z, V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorAdd
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vaddq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_add_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSum
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
			vec_float_t vTemp = vpaddq_f32(V, V);
			return vpaddq_f32(vTemp, vTemp);
#else
			float32x2_t v1 = vget_low_f32(V);
			float32x2_t v2 = vget_high_f32(V);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			return vcombine_f32(v1, v1);
#endif
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vTemp = _mm_hadd_ps(V, V);
			return _mm_hadd_ps(vTemp, vTemp);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 3, 0, 1));
			vec_float_t vTemp2 = _mm_add_ps(V, vTemp);
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp2, _MM_SHUFFLE(1, 0, 3, 2));
			return _mm_add_ps(vTemp, vTemp2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorAddAngles
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Adjust the angles
			vec_float_t vResult = vaddq_f32(V1, V2);
			// Less than Pi?
			uint32x4_t vOffset = vcltq_f32(vResult, g_NegativePi);
			vOffset = vandq_u32(vOffset, g_TwoPi);
			// Add 2Pi to all entries less than -Pi
			vResult = vaddq_f32(vResult, vOffset);
			// Greater than or equal to Pi?
			vOffset = vcgeq_f32(vResult, g_Pi);
			vOffset = vandq_u32(vOffset, g_TwoPi);
			// Sub 2Pi to all entries greater than Pi
			vResult = vsubq_f32(vResult, vOffset);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Adjust the angles
			vec_float_t vResult = _mm_add_ps(V1, V2);
			// Less than Pi?
			vec_float_t vOffset = _mm_cmplt_ps(vResult, g_NegativePi);
			vOffset = _mm_and_ps(vOffset, g_TwoPi);
			// Add 2Pi to all entries less than -Pi
			vResult = _mm_add_ps(vResult, vOffset);
			// Greater than or equal to Pi?
			vOffset = _mm_cmpge_ps(vResult, g_Pi);
			vOffset = _mm_and_ps(vOffset, g_TwoPi);
			// Sub 2Pi to all entries greater than Pi
			vResult = _mm_sub_ps(vResult, vOffset);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSubtract
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vsubq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_sub_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSubtractAngles
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Adjust the angles
			vec_float_t vResult = vsubq_f32(V1, V2);
			// Less than Pi?
			uint32x4_t vOffset = vcltq_f32(vResult, g_NegativePi);
			vOffset = vandq_u32(vOffset, g_TwoPi);
			// Add 2Pi to all entries less than -Pi
			vResult = vaddq_f32(vResult, vOffset);
			// Greater than or equal to Pi?
			vOffset = vcgeq_f32(vResult, g_Pi);
			vOffset = vandq_u32(vOffset, g_TwoPi);
			// Sub 2Pi to all entries greater than Pi
			vResult = vsubq_f32(vResult, vOffset);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Adjust the angles
			vec_float_t vResult = _mm_sub_ps(V1, V2);
			// Less than Pi?
			vec_float_t vOffset = _mm_cmplt_ps(vResult, g_NegativePi);
			vOffset = _mm_and_ps(vOffset, g_TwoPi);
			// Add 2Pi to all entries less than -Pi
			vResult = _mm_add_ps(vResult, vOffset);
			// Greater than or equal to Pi?
			vOffset = _mm_cmpge_ps(vResult, g_Pi);
			vOffset = _mm_and_ps(vOffset, g_TwoPi);
			// Sub 2Pi to all entries greater than Pi
			vResult = _mm_sub_ps(vResult, vOffset);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMultiply
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vmulq_f32(V1, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_mul_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMultiplyAdd
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t V3
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
			return vfmaq_f32(V3, V1, V2);
#else
			return vmlaq_f32(V3, V1, V2);
#endif
#elif defined(LUNA_PLATFORM_FMA3)
			return _mm_fmadd_ps(V1, V2, V3);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_mul_ps(V1, V2);
			return _mm_add_ps(vResult, V3);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorDivide
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
			return vdivq_f32(V1, V2);
#else
			// 2 iterations of Newton-Raphson refinement of reciprocal
			float32x4_t Reciprocal = vrecpeq_f32(V2);
			float32x4_t S = vrecpsq_f32(Reciprocal, V2);
			Reciprocal = vmulq_f32(S, Reciprocal);
			S = vrecpsq_f32(Reciprocal, V2);
			Reciprocal = vmulq_f32(S, Reciprocal);
			return vmulq_f32(V1, Reciprocal);
#endif
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_div_ps(V1, V2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorNegativeMultiplySubtract
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t V3
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
			return vfmsq_f32(V3, V1, V2);
#else
			return vmlsq_f32(V3, V1, V2);
#endif
#elif defined(LUNA_PLATFORM_FMA3)
			return _mm_fnmadd_ps(V1, V2, V3);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t R = _mm_mul_ps(V1, V2);
			return _mm_sub_ps(V3, R);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorScale
		(
			vec_float_p1_t V,
			f32    ScaleFactor
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vmulq_n_f32(V, ScaleFactor);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_set_ps1(ScaleFactor);
			return _mm_mul_ps(vResult, V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorReciprocalEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vrecpeq_f32(V);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_rcp_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorReciprocal
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
			float32x4_t one = vdupq_n_f32(1.0f);
			return vdivq_f32(one, V);
#else
			// 2 iterations of Newton-Raphson refinement
			float32x4_t Reciprocal = vrecpeq_f32(V);
			float32x4_t S = vrecpsq_f32(Reciprocal, V);
			Reciprocal = vmulq_f32(S, Reciprocal);
			S = vrecpsq_f32(Reciprocal, V);
			return vmulq_f32(S, Reciprocal);
#endif
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_div_ps(g_One, V);
#endif
		}

		//------------------------------------------------------------------------------
		// Return an estimated square root
		inline vec_float_t LUNA_SIMD_CALLCONV VectorSqrtEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// 1 iteration of Newton-Raphson refinment of sqrt
			float32x4_t S0 = vrsqrteq_f32(V);
			float32x4_t P0 = vmulq_f32(V, S0);
			float32x4_t R0 = vrsqrtsq_f32(P0, S0);
			float32x4_t S1 = vmulq_f32(S0, R0);

			vec_float_t VEqualsInfinity = VectorEqualInt(V, g_Infinity.v);
			vec_float_t VEqualsZero = VectorEqual(V, vdupq_n_f32(0));
			vec_float_t Result = vmulq_f32(V, S1);
			vec_float_t Select = VectorEqualInt(VEqualsInfinity, VEqualsZero);
			return VectorSelect(V, Result, Select);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_sqrt_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSqrt
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// 3 iterations of Newton-Raphson refinment of sqrt
			float32x4_t S0 = vrsqrteq_f32(V);
			float32x4_t P0 = vmulq_f32(V, S0);
			float32x4_t R0 = vrsqrtsq_f32(P0, S0);
			float32x4_t S1 = vmulq_f32(S0, R0);
			float32x4_t P1 = vmulq_f32(V, S1);
			float32x4_t R1 = vrsqrtsq_f32(P1, S1);
			float32x4_t S2 = vmulq_f32(S1, R1);
			float32x4_t P2 = vmulq_f32(V, S2);
			float32x4_t R2 = vrsqrtsq_f32(P2, S2);
			float32x4_t S3 = vmulq_f32(S2, R2);

			vec_float_t VEqualsInfinity = VectorEqualInt(V, g_Infinity.v);
			vec_float_t VEqualsZero = VectorEqual(V, vdupq_n_f32(0));
			vec_float_t Result = vmulq_f32(V, S3);
			vec_float_t Select = VectorEqualInt(VEqualsInfinity, VEqualsZero);
			return VectorSelect(V, Result, Select);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_sqrt_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorReciprocalSqrtEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vrsqrteq_f32(V);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_rsqrt_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorReciprocalSqrt
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// 2 iterations of Newton-Raphson refinement of reciprocal
			float32x4_t S0 = vrsqrteq_f32(V);

			float32x4_t P0 = vmulq_f32(V, S0);
			float32x4_t R0 = vrsqrtsq_f32(P0, S0);

			float32x4_t S1 = vmulq_f32(S0, R0);
			float32x4_t P1 = vmulq_f32(V, S1);
			float32x4_t R1 = vrsqrtsq_f32(P1, S1);

			return vmulq_f32(S1, R1);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_sqrt_ps(V);
			vResult = _mm_div_ps(g_One, vResult);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorExp2
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			int32x4_t itrunc = vcvtq_s32_f32(V);
			float32x4_t ftrunc = vcvtq_f32_s32(itrunc);
			float32x4_t y = vsubq_f32(V, ftrunc);

			float32x4_t poly = vmlaq_f32(g_ExpEst6, g_ExpEst7, y);
			poly = vmlaq_f32(g_ExpEst5, poly, y);
			poly = vmlaq_f32(g_ExpEst4, poly, y);
			poly = vmlaq_f32(g_ExpEst3, poly, y);
			poly = vmlaq_f32(g_ExpEst2, poly, y);
			poly = vmlaq_f32(g_ExpEst1, poly, y);
			poly = vmlaq_f32(g_One, poly, y);

			int32x4_t biased = vaddq_s32(itrunc, g_ExponentBias);
			biased = vshlq_n_s32(biased, 23);
			float32x4_t result0 = VectorDivide(biased, poly);

			biased = vaddq_s32(itrunc, g_253);
			biased = vshlq_n_s32(biased, 23);
			float32x4_t result1 = VectorDivide(biased, poly);
			result1 = vmulq_f32(g_MinNormal.v, result1);

			// Use selection to handle the cases
			//  if (V is NaN) -> QNaN;
			//  else if (V sign bit set)
			//      if (V > -150)
			//         if (V.exponent < -126) -> result1
			//         else -> result0
			//      else -> +0
			//  else
			//      if (V < 128) -> result0
			//      else -> +inf

			int32x4_t comp = vcltq_s32(V, g_Bin128);
			float32x4_t result2 = vbslq_f32(comp, result0, g_Infinity);

			comp = vcltq_s32(itrunc, g_SubnormalExponent);
			float32x4_t result3 = vbslq_f32(comp, result1, result0);

			comp = vcltq_s32(V, g_BinNeg150);
			float32x4_t result4 = vbslq_f32(comp, result3, g_Zero);

			int32x4_t sign = vandq_s32(V, g_NegativeZero);
			comp = vceqq_s32(sign, g_NegativeZero);
			float32x4_t result5 = vbslq_f32(comp, result4, result2);

			int32x4_t t0 = vandq_s32(V, g_QNaNTest);
			int32x4_t t1 = vandq_s32(V, g_Infinity);
			t0 = vceqq_s32(t0, g_Zero);
			t1 = vceqq_s32(t1, g_Infinity);
			int32x4_t isNaN = vbicq_s32(t1, t0);

			float32x4_t vResult = vbslq_f32(isNaN, g_QNaN, result5);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i itrunc = _mm_cvttps_epi32(V);
			__m128 ftrunc = _mm_cvtepi32_ps(itrunc);
			__m128 y = _mm_sub_ps(V, ftrunc);
			__m128 poly = _mm_mul_ps(g_ExpEst7, y);
			poly = _mm_add_ps(g_ExpEst6, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst5, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst4, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst3, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst2, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst1, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_One, poly);

			__m128i biased = _mm_add_epi32(itrunc, g_ExponentBias);
			biased = _mm_slli_epi32(biased, 23);
			__m128 result0 = _mm_div_ps(_mm_castsi128_ps(biased), poly);

			biased = _mm_add_epi32(itrunc, g_253);
			biased = _mm_slli_epi32(biased, 23);
			__m128 result1 = _mm_div_ps(_mm_castsi128_ps(biased), poly);
			result1 = _mm_mul_ps(g_MinNormal.v, result1);

			// Use selection to handle the cases
			//  if (V is NaN) -> QNaN;
			//  else if (V sign bit set)
			//      if (V > -150)
			//         if (V.exponent < -126) -> result1
			//         else -> result0
			//      else -> +0
			//  else
			//      if (V < 128) -> result0
			//      else -> +inf

			__m128i comp = _mm_cmplt_epi32(_mm_castps_si128(V), g_Bin128);
			__m128i select0 = _mm_and_si128(comp, _mm_castps_si128(result0));
			__m128i select1 = _mm_andnot_si128(comp, g_Infinity);
			__m128i result2 = _mm_or_si128(select0, select1);

			comp = _mm_cmplt_epi32(itrunc, g_SubnormalExponent);
			select1 = _mm_and_si128(comp, _mm_castps_si128(result1));
			select0 = _mm_andnot_si128(comp, _mm_castps_si128(result0));
			__m128i result3 = _mm_or_si128(select0, select1);

			comp = _mm_cmplt_epi32(_mm_castps_si128(V), g_BinNeg150);
			select0 = _mm_and_si128(comp, result3);
			select1 = _mm_andnot_si128(comp, g_Zero);
			__m128i result4 = _mm_or_si128(select0, select1);

			__m128i sign = _mm_and_si128(_mm_castps_si128(V), g_NegativeZero);
			comp = _mm_cmpeq_epi32(sign, g_NegativeZero);
			select0 = _mm_and_si128(comp, result4);
			select1 = _mm_andnot_si128(comp, result2);
			__m128i result5 = _mm_or_si128(select0, select1);

			__m128i t0 = _mm_and_si128(_mm_castps_si128(V), g_QNaNTest);
			__m128i t1 = _mm_and_si128(_mm_castps_si128(V), g_Infinity);
			t0 = _mm_cmpeq_epi32(t0, g_Zero);
			t1 = _mm_cmpeq_epi32(t1, g_Infinity);
			__m128i isNaN = _mm_andnot_si128(t0, t1);

			select0 = _mm_and_si128(isNaN, g_QNaN);
			select1 = _mm_andnot_si128(isNaN, result5);
			__m128i vResult = _mm_or_si128(select0, select1);

			return _mm_castsi128_ps(vResult);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorExpE
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// expE(V) = exp2(vin*log2(e))
			float32x4_t Ve = vmulq_f32(g_LgE, V);

			int32x4_t itrunc = vcvtq_s32_f32(Ve);
			float32x4_t ftrunc = vcvtq_f32_s32(itrunc);
			float32x4_t y = vsubq_f32(Ve, ftrunc);


			float32x4_t poly = vmlaq_f32(g_ExpEst6, g_ExpEst7, y);
			poly = vmlaq_f32(g_ExpEst5, poly, y);
			poly = vmlaq_f32(g_ExpEst4, poly, y);
			poly = vmlaq_f32(g_ExpEst3, poly, y);
			poly = vmlaq_f32(g_ExpEst2, poly, y);
			poly = vmlaq_f32(g_ExpEst1, poly, y);
			poly = vmlaq_f32(g_One, poly, y);

			int32x4_t biased = vaddq_s32(itrunc, g_ExponentBias);
			biased = vshlq_n_s32(biased, 23);
			float32x4_t result0 = VectorDivide(biased, poly);

			biased = vaddq_s32(itrunc, g_253);
			biased = vshlq_n_s32(biased, 23);
			float32x4_t result1 = VectorDivide(biased, poly);
			result1 = vmulq_f32(g_MinNormal.v, result1);

			// Use selection to handle the cases
			//  if (V is NaN) -> QNaN;
			//  else if (V sign bit set)
			//      if (V > -150)
			//         if (V.exponent < -126) -> result1
			//         else -> result0
			//      else -> +0
			//  else
			//      if (V < 128) -> result0
			//      else -> +inf

			int32x4_t comp = vcltq_s32(Ve, g_Bin128);
			float32x4_t result2 = vbslq_f32(comp, result0, g_Infinity);

			comp = vcltq_s32(itrunc, g_SubnormalExponent);
			float32x4_t result3 = vbslq_f32(comp, result1, result0);

			comp = vcltq_s32(Ve, g_BinNeg150);
			float32x4_t result4 = vbslq_f32(comp, result3, g_Zero);

			int32x4_t sign = vandq_s32(Ve, g_NegativeZero);
			comp = vceqq_s32(sign, g_NegativeZero);
			float32x4_t result5 = vbslq_f32(comp, result4, result2);

			int32x4_t t0 = vandq_s32(Ve, g_QNaNTest);
			int32x4_t t1 = vandq_s32(Ve, g_Infinity);
			t0 = vceqq_s32(t0, g_Zero);
			t1 = vceqq_s32(t1, g_Infinity);
			int32x4_t isNaN = vbicq_s32(t1, t0);

			float32x4_t vResult = vbslq_f32(isNaN, g_QNaN, result5);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// expE(V) = exp2(vin*log2(e))
			__m128 Ve = _mm_mul_ps(g_LgE, V);

			__m128i itrunc = _mm_cvttps_epi32(Ve);
			__m128 ftrunc = _mm_cvtepi32_ps(itrunc);
			__m128 y = _mm_sub_ps(Ve, ftrunc);
			__m128 poly = _mm_mul_ps(g_ExpEst7, y);
			poly = _mm_add_ps(g_ExpEst6, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst5, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst4, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst3, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst2, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_ExpEst1, poly);
			poly = _mm_mul_ps(poly, y);
			poly = _mm_add_ps(g_One, poly);

			__m128i biased = _mm_add_epi32(itrunc, g_ExponentBias);
			biased = _mm_slli_epi32(biased, 23);
			__m128 result0 = _mm_div_ps(_mm_castsi128_ps(biased), poly);

			biased = _mm_add_epi32(itrunc, g_253);
			biased = _mm_slli_epi32(biased, 23);
			__m128 result1 = _mm_div_ps(_mm_castsi128_ps(biased), poly);
			result1 = _mm_mul_ps(g_MinNormal.v, result1);

			// Use selection to handle the cases
			//  if (V is NaN) -> QNaN;
			//  else if (V sign bit set)
			//      if (V > -150)
			//         if (V.exponent < -126) -> result1
			//         else -> result0
			//      else -> +0
			//  else
			//      if (V < 128) -> result0
			//      else -> +inf

			__m128i comp = _mm_cmplt_epi32(_mm_castps_si128(Ve), g_Bin128);
			__m128i select0 = _mm_and_si128(comp, _mm_castps_si128(result0));
			__m128i select1 = _mm_andnot_si128(comp, g_Infinity);
			__m128i result2 = _mm_or_si128(select0, select1);

			comp = _mm_cmplt_epi32(itrunc, g_SubnormalExponent);
			select1 = _mm_and_si128(comp, _mm_castps_si128(result1));
			select0 = _mm_andnot_si128(comp, _mm_castps_si128(result0));
			__m128i result3 = _mm_or_si128(select0, select1);

			comp = _mm_cmplt_epi32(_mm_castps_si128(Ve), g_BinNeg150);
			select0 = _mm_and_si128(comp, result3);
			select1 = _mm_andnot_si128(comp, g_Zero);
			__m128i result4 = _mm_or_si128(select0, select1);

			__m128i sign = _mm_and_si128(_mm_castps_si128(Ve), g_NegativeZero);
			comp = _mm_cmpeq_epi32(sign, g_NegativeZero);
			select0 = _mm_and_si128(comp, result4);
			select1 = _mm_andnot_si128(comp, result2);
			__m128i result5 = _mm_or_si128(select0, select1);

			__m128i t0 = _mm_and_si128(_mm_castps_si128(Ve), g_QNaNTest);
			__m128i t1 = _mm_and_si128(_mm_castps_si128(Ve), g_Infinity);
			t0 = _mm_cmpeq_epi32(t0, g_Zero);
			t1 = _mm_cmpeq_epi32(t1, g_Infinity);
			__m128i isNaN = _mm_andnot_si128(t0, t1);

			select0 = _mm_and_si128(isNaN, g_QNaN);
			select1 = _mm_andnot_si128(isNaN, result5);
			__m128i vResult = _mm_or_si128(select0, select1);

			return _mm_castsi128_ps(vResult);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorExp
		(
			vec_float_p1_t V
		)
		{
			return VectorExp2(V);
		}

		//------------------------------------------------------------------------------

#if defined(LUNA_PLATFORM_SSE2)

		namespace Internal
		{
			inline __m128i multi_sll_epi32(__m128i value, __m128i count)
			{
				__m128i v = _mm_shuffle_epi32(value, _MM_SHUFFLE(0, 0, 0, 0));
				__m128i c = _mm_shuffle_epi32(count, _MM_SHUFFLE(0, 0, 0, 0));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r0 = _mm_sll_epi32(v, c);

				v = _mm_shuffle_epi32(value, _MM_SHUFFLE(1, 1, 1, 1));
				c = _mm_shuffle_epi32(count, _MM_SHUFFLE(1, 1, 1, 1));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r1 = _mm_sll_epi32(v, c);

				v = _mm_shuffle_epi32(value, _MM_SHUFFLE(2, 2, 2, 2));
				c = _mm_shuffle_epi32(count, _MM_SHUFFLE(2, 2, 2, 2));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r2 = _mm_sll_epi32(v, c);

				v = _mm_shuffle_epi32(value, _MM_SHUFFLE(3, 3, 3, 3));
				c = _mm_shuffle_epi32(count, _MM_SHUFFLE(3, 3, 3, 3));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r3 = _mm_sll_epi32(v, c);

				// (r0,r0,r1,r1)
				__m128 r01 = _mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), _MM_SHUFFLE(0, 0, 0, 0));
				// (r2,r2,r3,r3)
				__m128 r23 = _mm_shuffle_ps(_mm_castsi128_ps(r2), _mm_castsi128_ps(r3), _MM_SHUFFLE(0, 0, 0, 0));
				// (r0,r1,r2,r3)
				__m128 result = _mm_shuffle_ps(r01, r23, _MM_SHUFFLE(2, 0, 2, 0));
				return _mm_castps_si128(result);
			}

			inline __m128i multi_srl_epi32(__m128i value, __m128i count)
			{
				__m128i v = _mm_shuffle_epi32(value, _MM_SHUFFLE(0, 0, 0, 0));
				__m128i c = _mm_shuffle_epi32(count, _MM_SHUFFLE(0, 0, 0, 0));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r0 = _mm_srl_epi32(v, c);

				v = _mm_shuffle_epi32(value, _MM_SHUFFLE(1, 1, 1, 1));
				c = _mm_shuffle_epi32(count, _MM_SHUFFLE(1, 1, 1, 1));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r1 = _mm_srl_epi32(v, c);

				v = _mm_shuffle_epi32(value, _MM_SHUFFLE(2, 2, 2, 2));
				c = _mm_shuffle_epi32(count, _MM_SHUFFLE(2, 2, 2, 2));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r2 = _mm_srl_epi32(v, c);

				v = _mm_shuffle_epi32(value, _MM_SHUFFLE(3, 3, 3, 3));
				c = _mm_shuffle_epi32(count, _MM_SHUFFLE(3, 3, 3, 3));
				c = _mm_and_si128(c, g_MaskX);
				__m128i r3 = _mm_srl_epi32(v, c);

				// (r0,r0,r1,r1)
				__m128 r01 = _mm_shuffle_ps(_mm_castsi128_ps(r0), _mm_castsi128_ps(r1), _MM_SHUFFLE(0, 0, 0, 0));
				// (r2,r2,r3,r3)
				__m128 r23 = _mm_shuffle_ps(_mm_castsi128_ps(r2), _mm_castsi128_ps(r3), _MM_SHUFFLE(0, 0, 0, 0));
				// (r0,r1,r2,r3)
				__m128 result = _mm_shuffle_ps(r01, r23, _MM_SHUFFLE(2, 0, 2, 0));
				return _mm_castps_si128(result);
			}

			inline __m128i GetLeadingBit(const __m128i value)
			{
				static const vec_i32_t g_0000FFFF = { { { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF } } };
				static const vec_i32_t g_000000FF = { { { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF } } };
				static const vec_i32_t g_0000000F = { { { 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F } } };
				static const vec_i32_t g_00000003 = { { { 0x00000003, 0x00000003, 0x00000003, 0x00000003 } } };

				__m128i v = value, r, c, b, s;

				c = _mm_cmpgt_epi32(v, g_0000FFFF);   // c = (v > 0xFFFF)
				b = _mm_srli_epi32(c, 31);              // b = (c ? 1 : 0)
				r = _mm_slli_epi32(b, 4);               // r = (b << 4)
				v = multi_srl_epi32(v, r);              // v = (v >> r)

				c = _mm_cmpgt_epi32(v, g_000000FF);   // c = (v > 0xFF)
				b = _mm_srli_epi32(c, 31);              // b = (c ? 1 : 0)
				s = _mm_slli_epi32(b, 3);               // s = (b << 3)
				v = multi_srl_epi32(v, s);              // v = (v >> s)
				r = _mm_or_si128(r, s);                 // r = (r | s)

				c = _mm_cmpgt_epi32(v, g_0000000F);   // c = (v > 0xF)
				b = _mm_srli_epi32(c, 31);              // b = (c ? 1 : 0)
				s = _mm_slli_epi32(b, 2);               // s = (b << 2)
				v = multi_srl_epi32(v, s);              // v = (v >> s)
				r = _mm_or_si128(r, s);                 // r = (r | s)

				c = _mm_cmpgt_epi32(v, g_00000003);   // c = (v > 0x3)
				b = _mm_srli_epi32(c, 31);              // b = (c ? 1 : 0)
				s = _mm_slli_epi32(b, 1);               // s = (b << 1)
				v = multi_srl_epi32(v, s);              // v = (v >> s)
				r = _mm_or_si128(r, s);                 // r = (r | s)

				s = _mm_srli_epi32(v, 1);
				r = _mm_or_si128(r, s);
				return r;
			}
		} // namespace Internal

#endif // LUNA_PLATFORM_SSE2

#if defined(LUNA_PLATFORM_ARM_NEON)

		namespace Internal
		{
			inline int32x4_t GetLeadingBit(const int32x4_t value)
			{
				static const vec_i32_t g_0000FFFF = { { { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF } } };
				static const vec_i32_t g_000000FF = { { { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF } } };
				static const vec_i32_t g_0000000F = { { { 0x0000000F, 0x0000000F, 0x0000000F, 0x0000000F } } };
				static const vec_i32_t g_00000003 = { { { 0x00000003, 0x00000003, 0x00000003, 0x00000003 } } };

				int32x4_t v = value, r, c, b, s;

				c = vcgtq_s32(v, g_0000FFFF);     // c = (v > 0xFFFF)
				b = vshrq_n_u32(c, 31);             // b = (c ? 1 : 0)
				r = vshlq_n_s32(b, 4);              // r = (b << 4)
				r = vnegq_s32(r);
				v = vshlq_u32(v, r);              // v = (v >> r)

				c = vcgtq_s32(v, g_000000FF);     // c = (v > 0xFF)
				b = vshrq_n_u32(c, 31);             // b = (c ? 1 : 0)
				s = vshlq_n_s32(b, 3);              // s = (b << 3)
				s = vnegq_s32(s);
				v = vshlq_u32(v, s);                // v = (v >> s)
				r = vorrq_s32(r, s);                // r = (r | s)

				c = vcgtq_s32(v, g_0000000F);     // c = (v > 0xF)
				b = vshrq_n_u32(c, 31);             // b = (c ? 1 : 0)
				s = vshlq_n_s32(b, 2);              // s = (b << 2)
				s = vnegq_s32(s);
				v = vshlq_u32(v, s);                // v = (v >> s)
				r = vorrq_s32(r, s);                // r = (r | s)

				c = vcgtq_s32(v, g_00000003);     // c = (v > 0x3)
				b = vshrq_n_u32(c, 31);             // b = (c ? 1 : 0)
				s = vshlq_n_s32(b, 1);              // s = (b << 1)
				s = vnegq_s32(s);
				v = vshlq_u32(v, s);                // v = (v >> s)
				r = vorrq_s32(r, s);                // r = (r | s)

				s = vshrq_n_u32(v, 1);
				r = vorrq_s32(r, s);
				return r;
			}

		} // namespace Internal

#endif

//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLog2
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			int32x4_t rawBiased = vandq_s32(V, g_Infinity);
			int32x4_t trailing = vandq_s32(V, g_QNaNTest);
			int32x4_t isExponentZero = vceqq_s32(g_Zero, rawBiased);

			// Compute exponent and significand for normals.
			int32x4_t biased = vshrq_n_u32(rawBiased, 23);
			int32x4_t exponentNor = vsubq_s32(biased, g_ExponentBias);
			int32x4_t trailingNor = trailing;

			// Compute exponent and significand for subnormals.
			int32x4_t leading = Internal::GetLeadingBit(trailing);
			int32x4_t shift = vsubq_s32(g_NumTrailing, leading);
			int32x4_t exponentSub = vsubq_s32(g_SubnormalExponent, shift);
			int32x4_t trailingSub = vshlq_u32(trailing, shift);
			trailingSub = vandq_s32(trailingSub, g_QNaNTest);
			int32x4_t e = vbslq_f32(isExponentZero, exponentSub, exponentNor);
			int32x4_t t = vbslq_f32(isExponentZero, trailingSub, trailingNor);

			// Compute the approximation.
			int32x4_t tmp = vorrq_s32(g_One, t);
			float32x4_t y = vsubq_f32(tmp, g_One);

			float32x4_t log2 = vmlaq_f32(g_LogEst6, g_LogEst7, y);
			log2 = vmlaq_f32(g_LogEst5, log2, y);
			log2 = vmlaq_f32(g_LogEst4, log2, y);
			log2 = vmlaq_f32(g_LogEst3, log2, y);
			log2 = vmlaq_f32(g_LogEst2, log2, y);
			log2 = vmlaq_f32(g_LogEst1, log2, y);
			log2 = vmlaq_f32(g_LogEst0, log2, y);
			log2 = vmlaq_f32(vcvtq_f32_s32(e), log2, y);

			//  if (x is NaN) -> QNaN
			//  else if (V is positive)
			//      if (V is infinite) -> +inf
			//      else -> log2(V)
			//  else
			//      if (V is zero) -> -inf
			//      else -> -QNaN

			int32x4_t isInfinite = vandq_s32((V), g_AbsMask);
			isInfinite = vceqq_s32(isInfinite, g_Infinity);

			int32x4_t isGreaterZero = vcgtq_s32((V), g_Zero);
			int32x4_t isNotFinite = vcgtq_s32((V), g_Infinity);
			int32x4_t isPositive = vbicq_s32(isGreaterZero, isNotFinite);

			int32x4_t isZero = vandq_s32((V), g_AbsMask);
			isZero = vceqq_s32(isZero, g_Zero);

			int32x4_t t0 = vandq_s32((V), g_QNaNTest);
			int32x4_t t1 = vandq_s32((V), g_Infinity);
			t0 = vceqq_s32(t0, g_Zero);
			t1 = vceqq_s32(t1, g_Infinity);
			int32x4_t isNaN = vbicq_s32(t1, t0);

			float32x4_t result = vbslq_f32(isInfinite, g_Infinity, log2);
			tmp = vbslq_f32(isZero, g_NegInfinity, g_NegQNaN);
			result = vbslq_f32(isPositive, result, tmp);
			result = vbslq_f32(isNaN, g_QNaN, result);
			return result;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i rawBiased = _mm_and_si128(_mm_castps_si128(V), g_Infinity);
			__m128i trailing = _mm_and_si128(_mm_castps_si128(V), g_QNaNTest);
			__m128i isExponentZero = _mm_cmpeq_epi32(g_Zero, rawBiased);

			// Compute exponent and significand for normals.
			__m128i biased = _mm_srli_epi32(rawBiased, 23);
			__m128i exponentNor = _mm_sub_epi32(biased, g_ExponentBias);
			__m128i trailingNor = trailing;

			// Compute exponent and significand for subnormals.
			__m128i leading = Internal::GetLeadingBit(trailing);
			__m128i shift = _mm_sub_epi32(g_NumTrailing, leading);
			__m128i exponentSub = _mm_sub_epi32(g_SubnormalExponent, shift);
			__m128i trailingSub = Internal::multi_sll_epi32(trailing, shift);
			trailingSub = _mm_and_si128(trailingSub, g_QNaNTest);

			__m128i select0 = _mm_and_si128(isExponentZero, exponentSub);
			__m128i select1 = _mm_andnot_si128(isExponentZero, exponentNor);
			__m128i e = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isExponentZero, trailingSub);
			select1 = _mm_andnot_si128(isExponentZero, trailingNor);
			__m128i t = _mm_or_si128(select0, select1);

			// Compute the approximation.
			__m128i tmp = _mm_or_si128(g_One, t);
			__m128 y = _mm_sub_ps(_mm_castsi128_ps(tmp), g_One);

			__m128 log2 = _mm_mul_ps(g_LogEst7, y);
			log2 = _mm_add_ps(g_LogEst6, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst5, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst4, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst3, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst2, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst1, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst0, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(log2, _mm_cvtepi32_ps(e));

			//  if (x is NaN) -> QNaN
			//  else if (V is positive)
			//      if (V is infinite) -> +inf
			//      else -> log2(V)
			//  else
			//      if (V is zero) -> -inf
			//      else -> -QNaN

			__m128i isInfinite = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			isInfinite = _mm_cmpeq_epi32(isInfinite, g_Infinity);

			__m128i isGreaterZero = _mm_cmpgt_epi32(_mm_castps_si128(V), g_Zero);
			__m128i isNotFinite = _mm_cmpgt_epi32(_mm_castps_si128(V), g_Infinity);
			__m128i isPositive = _mm_andnot_si128(isNotFinite, isGreaterZero);

			__m128i isZero = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			isZero = _mm_cmpeq_epi32(isZero, g_Zero);

			__m128i t0 = _mm_and_si128(_mm_castps_si128(V), g_QNaNTest);
			__m128i t1 = _mm_and_si128(_mm_castps_si128(V), g_Infinity);
			t0 = _mm_cmpeq_epi32(t0, g_Zero);
			t1 = _mm_cmpeq_epi32(t1, g_Infinity);
			__m128i isNaN = _mm_andnot_si128(t0, t1);

			select0 = _mm_and_si128(isInfinite, g_Infinity);
			select1 = _mm_andnot_si128(isInfinite, _mm_castps_si128(log2));
			__m128i result = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isZero, g_NegInfinity);
			select1 = _mm_andnot_si128(isZero, g_NegQNaN);
			tmp = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isPositive, result);
			select1 = _mm_andnot_si128(isPositive, tmp);
			result = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isNaN, g_QNaN);
			select1 = _mm_andnot_si128(isNaN, result);
			result = _mm_or_si128(select0, select1);

			return _mm_castsi128_ps(result);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLogE
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			int32x4_t rawBiased = vandq_s32(V, g_Infinity);
			int32x4_t trailing = vandq_s32(V, g_QNaNTest);
			int32x4_t isExponentZero = vceqq_s32(g_Zero, rawBiased);

			// Compute exponent and significant for normals.
			int32x4_t biased = vshrq_n_u32(rawBiased, 23);
			int32x4_t exponentNor = vsubq_s32(biased, g_ExponentBias);
			int32x4_t trailingNor = trailing;

			// Compute exponent and significant for subnormals.
			int32x4_t leading = Internal::GetLeadingBit(trailing);
			int32x4_t shift = vsubq_s32(g_NumTrailing, leading);
			int32x4_t exponentSub = vsubq_s32(g_SubnormalExponent, shift);
			int32x4_t trailingSub = vshlq_u32(trailing, shift);
			trailingSub = vandq_s32(trailingSub, g_QNaNTest);
			int32x4_t e = vbslq_f32(isExponentZero, exponentSub, exponentNor);
			int32x4_t t = vbslq_f32(isExponentZero, trailingSub, trailingNor);

			// Compute the approximation.
			int32x4_t tmp = vorrq_s32(g_One, t);
			float32x4_t y = vsubq_f32(tmp, g_One);

			float32x4_t log2 = vmlaq_f32(g_LogEst6, g_LogEst7, y);
			log2 = vmlaq_f32(g_LogEst5, log2, y);
			log2 = vmlaq_f32(g_LogEst4, log2, y);
			log2 = vmlaq_f32(g_LogEst3, log2, y);
			log2 = vmlaq_f32(g_LogEst2, log2, y);
			log2 = vmlaq_f32(g_LogEst1, log2, y);
			log2 = vmlaq_f32(g_LogEst0, log2, y);
			log2 = vmlaq_f32(vcvtq_f32_s32(e), log2, y);

			log2 = vmulq_f32(g_InvLgE, log2);

			//  if (x is NaN) -> QNaN
			//  else if (V is positive)
			//      if (V is infinite) -> +inf
			//      else -> log2(V)
			//  else
			//      if (V is zero) -> -inf
			//      else -> -QNaN

			int32x4_t isInfinite = vandq_s32((V), g_AbsMask);
			isInfinite = vceqq_s32(isInfinite, g_Infinity);

			int32x4_t isGreaterZero = vcgtq_s32((V), g_Zero);
			int32x4_t isNotFinite = vcgtq_s32((V), g_Infinity);
			int32x4_t isPositive = vbicq_s32(isGreaterZero, isNotFinite);

			int32x4_t isZero = vandq_s32((V), g_AbsMask);
			isZero = vceqq_s32(isZero, g_Zero);

			int32x4_t t0 = vandq_s32((V), g_QNaNTest);
			int32x4_t t1 = vandq_s32((V), g_Infinity);
			t0 = vceqq_s32(t0, g_Zero);
			t1 = vceqq_s32(t1, g_Infinity);
			int32x4_t isNaN = vbicq_s32(t1, t0);

			float32x4_t result = vbslq_f32(isInfinite, g_Infinity, log2);
			tmp = vbslq_f32(isZero, g_NegInfinity, g_NegQNaN);
			result = vbslq_f32(isPositive, result, tmp);
			result = vbslq_f32(isNaN, g_QNaN, result);
			return result;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i rawBiased = _mm_and_si128(_mm_castps_si128(V), g_Infinity);
			__m128i trailing = _mm_and_si128(_mm_castps_si128(V), g_QNaNTest);
			__m128i isExponentZero = _mm_cmpeq_epi32(g_Zero, rawBiased);

			// Compute exponent and significand for normals.
			__m128i biased = _mm_srli_epi32(rawBiased, 23);
			__m128i exponentNor = _mm_sub_epi32(biased, g_ExponentBias);
			__m128i trailingNor = trailing;

			// Compute exponent and significand for subnormals.
			__m128i leading = Internal::GetLeadingBit(trailing);
			__m128i shift = _mm_sub_epi32(g_NumTrailing, leading);
			__m128i exponentSub = _mm_sub_epi32(g_SubnormalExponent, shift);
			__m128i trailingSub = Internal::multi_sll_epi32(trailing, shift);
			trailingSub = _mm_and_si128(trailingSub, g_QNaNTest);

			__m128i select0 = _mm_and_si128(isExponentZero, exponentSub);
			__m128i select1 = _mm_andnot_si128(isExponentZero, exponentNor);
			__m128i e = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isExponentZero, trailingSub);
			select1 = _mm_andnot_si128(isExponentZero, trailingNor);
			__m128i t = _mm_or_si128(select0, select1);

			// Compute the approximation.
			__m128i tmp = _mm_or_si128(g_One, t);
			__m128 y = _mm_sub_ps(_mm_castsi128_ps(tmp), g_One);

			__m128 log2 = _mm_mul_ps(g_LogEst7, y);
			log2 = _mm_add_ps(g_LogEst6, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst5, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst4, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst3, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst2, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst1, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(g_LogEst0, log2);
			log2 = _mm_mul_ps(log2, y);
			log2 = _mm_add_ps(log2, _mm_cvtepi32_ps(e));

			log2 = _mm_mul_ps(g_InvLgE, log2);

			//  if (x is NaN) -> QNaN
			//  else if (V is positive)
			//      if (V is infinite) -> +inf
			//      else -> log2(V)
			//  else
			//      if (V is zero) -> -inf
			//      else -> -QNaN

			__m128i isInfinite = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			isInfinite = _mm_cmpeq_epi32(isInfinite, g_Infinity);

			__m128i isGreaterZero = _mm_cmpgt_epi32(_mm_castps_si128(V), g_Zero);
			__m128i isNotFinite = _mm_cmpgt_epi32(_mm_castps_si128(V), g_Infinity);
			__m128i isPositive = _mm_andnot_si128(isNotFinite, isGreaterZero);

			__m128i isZero = _mm_and_si128(_mm_castps_si128(V), g_AbsMask);
			isZero = _mm_cmpeq_epi32(isZero, g_Zero);

			__m128i t0 = _mm_and_si128(_mm_castps_si128(V), g_QNaNTest);
			__m128i t1 = _mm_and_si128(_mm_castps_si128(V), g_Infinity);
			t0 = _mm_cmpeq_epi32(t0, g_Zero);
			t1 = _mm_cmpeq_epi32(t1, g_Infinity);
			__m128i isNaN = _mm_andnot_si128(t0, t1);

			select0 = _mm_and_si128(isInfinite, g_Infinity);
			select1 = _mm_andnot_si128(isInfinite, _mm_castps_si128(log2));
			__m128i result = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isZero, g_NegInfinity);
			select1 = _mm_andnot_si128(isZero, g_NegQNaN);
			tmp = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isPositive, result);
			select1 = _mm_andnot_si128(isPositive, tmp);
			result = _mm_or_si128(select0, select1);

			select0 = _mm_and_si128(isNaN, g_QNaN);
			select1 = _mm_andnot_si128(isNaN, result);
			result = _mm_or_si128(select0, select1);

			return _mm_castsi128_ps(result);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLog
		(
			vec_float_p1_t V
		)
		{
			return VectorLog2(V);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorPow
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_f32_t vResult = { { {
					powf(vgetq_lane_f32(V1, 0), vgetq_lane_f32(V2, 0)),
					powf(vgetq_lane_f32(V1, 1), vgetq_lane_f32(V2, 1)),
					powf(vgetq_lane_f32(V1, 2), vgetq_lane_f32(V2, 2)),
					powf(vgetq_lane_f32(V1, 3), vgetq_lane_f32(V2, 3))
				} } };
			return vResult.v;
#elif defined(LUNA_PLATFORM_SSE2)
			__declspec(align(16)) f32 a[4];
			__declspec(align(16)) f32 b[4];
			_mm_store_ps(a, V1);
			_mm_store_ps(b, V2);
			vec_float_t vResult = _mm_setr_ps(
				powf(a[0], b[0]),
				powf(a[1], b[1]),
				powf(a[2], b[2]),
				powf(a[3], b[3]));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorAbs
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vabsq_f32(V);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_setzero_ps();
			vResult = _mm_sub_ps(vResult, V);
			vResult = _mm_max_ps(vResult, V);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorMod
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			// V1 % V2 = V1 - V2 * truncate(V1 / V2)

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t vResult = VectorDivide(V1, V2);
			vResult = VectorTruncate(vResult);
			return vmlsq_f32(V1, vResult, V2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = _mm_div_ps(V1, V2);
			vResult = VectorTruncate(vResult);
			vResult = _mm_mul_ps(vResult, V2);
			vResult = _mm_sub_ps(V1, vResult);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorModAngles
		(
			vec_float_p1_t Angles
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Modulo the range of the given angles such that -_PI <= Angles < _PI
			vec_float_t vResult = vmulq_f32(Angles, g_ReciprocalTwoPi);
			// Use the inline function due to complexity for rounding
			vResult = VectorRound(vResult);
			return vmlsq_f32(Angles, vResult, g_TwoPi);
#elif defined(LUNA_PLATFORM_SSE2)
			// Modulo the range of the given angles such that -_PI <= Angles < _PI
			vec_float_t vResult = _mm_mul_ps(Angles, g_ReciprocalTwoPi);
			// Use the inline function due to complexity for rounding
			vResult = VectorRound(vResult);
			vResult = _mm_mul_ps(vResult, g_TwoPi);
			vResult = _mm_sub_ps(Angles, vResult);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSin
		(
			vec_float_p1_t V
		)
		{
			// 11-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with sin(y) = sin(x).
			uint32x4_t sign = vandq_u32(x, g_NegativeZero);
			uint32x4_t c = vorrq_u32(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			float32x4_t absx = vabsq_f32(x);
			float32x4_t rflx = vsubq_f32(c, x);
			uint32x4_t comp = vcleq_f32(absx, g_HalfPi);
			x = vbslq_f32(comp, x, rflx);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation
			const vec_float_t SC1 = g_SinCoefficients1;
			const vec_float_t SC0 = g_SinCoefficients0;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(SC0), 1);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_low_f32(SC1), 0);

			vConstants = vdupq_lane_f32(vget_high_f32(SC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(SC0), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(SC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			Result = vmulq_f32(Result, x);
			return Result;
#elif defined(LUNA_PLATFORM_SSE2)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with sin(y) = sin(x).
			__m128 sign = _mm_and_ps(x, g_NegativeZero);
			__m128 c = _mm_or_ps(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			__m128 absx = _mm_andnot_ps(sign, x);  // |x|
			__m128 rflx = _mm_sub_ps(c, x);
			__m128 comp = _mm_cmple_ps(absx, g_HalfPi);
			__m128 select0 = _mm_and_ps(comp, x);
			__m128 select1 = _mm_andnot_ps(comp, rflx);
			x = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation
			const vec_float_t SC1 = g_SinCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(SC1, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			const vec_float_t SC0 = g_SinCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(3, 3, 3, 3));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);
			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, x);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorCos
		(
			vec_float_p1_t V
		)
		{
			// 10-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
	// Map V to x in [-pi,pi].
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with cos(y) = sign*cos(x).
			uint32x4_t sign = vandq_u32(x, g_NegativeZero);
			uint32x4_t c = vorrq_u32(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			float32x4_t absx = vabsq_f32(x);
			float32x4_t rflx = vsubq_f32(c, x);
			uint32x4_t comp = vcleq_f32(absx, g_HalfPi);
			x = vbslq_f32(comp, x, rflx);
			sign = vbslq_f32(comp, g_One, g_NegativeOne);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation
			const vec_float_t CC1 = g_CosCoefficients1;
			const vec_float_t CC0 = g_CosCoefficients0;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(CC0), 1);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_low_f32(CC1), 0);

			vConstants = vdupq_lane_f32(vget_high_f32(CC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(CC0), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(CC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			Result = vmulq_f32(Result, sign);
			return Result;
#elif defined(LUNA_PLATFORM_SSE2)
	// Map V to x in [-pi,pi].
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with cos(y) = sign*cos(x).
			vec_float_t sign = _mm_and_ps(x, g_NegativeZero);
			__m128 c = _mm_or_ps(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			__m128 absx = _mm_andnot_ps(sign, x);  // |x|
			__m128 rflx = _mm_sub_ps(c, x);
			__m128 comp = _mm_cmple_ps(absx, g_HalfPi);
			__m128 select0 = _mm_and_ps(comp, x);
			__m128 select1 = _mm_andnot_ps(comp, rflx);
			x = _mm_or_ps(select0, select1);
			select0 = _mm_and_ps(comp, g_One);
			select1 = _mm_andnot_ps(comp, g_NegativeOne);
			sign = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation
			const vec_float_t CC1 = g_CosCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(CC1, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			const vec_float_t CC0 = g_CosCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(3, 3, 3, 3));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);
			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, sign);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------


		inline void LUNA_SIMD_CALLCONV VectorSinCos
		(
			vec_float_t* pSin,
			vec_float_t* pCos,
			vec_float_p1_t V
		)
		{
			luassert(pSin != nullptr);
			luassert(pCos != nullptr);

			// 11/10-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with cos(y) = sign*cos(x).
			uint32x4_t sign = vandq_u32(x, g_NegativeZero);
			uint32x4_t c = vorrq_u32(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			float32x4_t absx = vabsq_f32(x);
			float32x4_t  rflx = vsubq_f32(c, x);
			uint32x4_t comp = vcleq_f32(absx, g_HalfPi);
			x = vbslq_f32(comp, x, rflx);
			sign = vbslq_f32(comp, g_One, g_NegativeOne);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation for sine
			const vec_float_t SC1 = g_SinCoefficients1;
			const vec_float_t SC0 = g_SinCoefficients0;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(SC0), 1);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_low_f32(SC1), 0);

			vConstants = vdupq_lane_f32(vget_high_f32(SC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(SC0), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(SC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			*pSin = vmulq_f32(Result, x);

			// Compute polynomial approximation for cosine
			const vec_float_t CC1 = g_CosCoefficients1;
			const vec_float_t CC0 = g_CosCoefficients0;
			vConstants = vdupq_lane_f32(vget_high_f32(CC0), 1);
			Result = vmlaq_lane_f32(vConstants, x2, vget_low_f32(CC1), 0);

			vConstants = vdupq_lane_f32(vget_high_f32(CC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(CC0), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(CC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			*pCos = vmulq_f32(Result, sign);
#elif defined(LUNA_PLATFORM_SSE2)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with sin(y) = sin(x), cos(y) = sign*cos(x).
			vec_float_t sign = _mm_and_ps(x, g_NegativeZero);
			__m128 c = _mm_or_ps(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			__m128 absx = _mm_andnot_ps(sign, x);  // |x|
			__m128 rflx = _mm_sub_ps(c, x);
			__m128 comp = _mm_cmple_ps(absx, g_HalfPi);
			__m128 select0 = _mm_and_ps(comp, x);
			__m128 select1 = _mm_andnot_ps(comp, rflx);
			x = _mm_or_ps(select0, select1);
			select0 = _mm_and_ps(comp, g_One);
			select1 = _mm_andnot_ps(comp, g_NegativeOne);
			sign = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation of sine
			const vec_float_t SC1 = g_SinCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(SC1, _MM_SHUFFLE(0, 0, 0, 0));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			const vec_float_t SC0 = g_SinCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(3, 3, 3, 3));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SC0, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);
			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, x);
			*pSin = Result;

			// Compute polynomial approximation of cosine
			const vec_float_t CC1 = g_CosCoefficients1;
			vConstants = LUNA_SIMD_PERMUTE_PS(CC1, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_mul_ps(vConstants, x2);

			const vec_float_t CC0 = g_CosCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(3, 3, 3, 3));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CC0, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);
			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, sign);
			*pCos = Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorTan
		(
			vec_float_p1_t V
		)
		{
			// Cody and Waite algorithm to compute tangent.

#if defined(LUNA_PLATFORM_SSE2) || defined(LUNA_PLATFORM_ARM_NEON) 

			static const vec_f32_t TanCoefficients0 = { { { 1.0f, -4.667168334e-1f, 2.566383229e-2f, -3.118153191e-4f } } };
			static const vec_f32_t TanCoefficients1 = { { { 4.981943399e-7f, -1.333835001e-1f, 3.424887824e-3f, -1.786170734e-5f } } };
			static const vec_f32_t TanConstants = { { { 1.570796371f, 6.077100628e-11f, 0.000244140625f, 0.63661977228f /*2 / Pi*/ } } };
			static const vec_u32_t Mask = { { { 0x1, 0x1, 0x1, 0x1 } } };

			vec_float_t TwoDivPi = vector_splat_w(TanConstants.v);

			vec_float_t Zero = vector_zero();

			vec_float_t C0 = vector_splat_x(TanConstants.v);
			vec_float_t C1 = vector_splat_y(TanConstants.v);
			vec_float_t Epsilon = vector_splat_z(TanConstants.v);

			vec_float_t VA = VectorMultiply(V, TwoDivPi);

			VA = VectorRound(VA);

			vec_float_t VC = VectorNegativeMultiplySubtract(VA, C0, V);

			vec_float_t VB = VectorAbs(VA);

			VC = VectorNegativeMultiplySubtract(VA, C1, VC);

#if defined(LUNA_PLATFORM_ARM_NEON)
			VB = vcvtq_u32_f32(VB);
#elif defined(LUNA_PLATFORM_SSE2)
			reinterpret_cast<__m128i *>(&VB)[0] = _mm_cvttps_epi32(VB);
#else
			for (usize i = 0; i < 4; i++)
			{
				VB.vector4_u32[i] = static_cast<u32>(VB.vector4_f32[i]);
			}
#endif

			vec_float_t VC2 = VectorMultiply(VC, VC);

			vec_float_t T7 = vector_splat_w(TanCoefficients1.v);
			vec_float_t T6 = vector_splat_z(TanCoefficients1.v);
			vec_float_t T4 = vector_splat_x(TanCoefficients1.v);
			vec_float_t T3 = vector_splat_w(TanCoefficients0.v);
			vec_float_t T5 = vector_splat_y(TanCoefficients1.v);
			vec_float_t T2 = vector_splat_z(TanCoefficients0.v);
			vec_float_t T1 = vector_splat_y(TanCoefficients0.v);
			vec_float_t T0 = vector_splat_x(TanCoefficients0.v);

			vec_float_t VBIsEven = VectorAndInt(VB, Mask.v);
			VBIsEven = VectorEqualInt(VBIsEven, Zero);

			vec_float_t N = VectorMultiplyAdd(VC2, T7, T6);
			vec_float_t D = VectorMultiplyAdd(VC2, T4, T3);
			N = VectorMultiplyAdd(VC2, N, T5);
			D = VectorMultiplyAdd(VC2, D, T2);
			N = VectorMultiply(VC2, N);
			D = VectorMultiplyAdd(VC2, D, T1);
			N = VectorMultiplyAdd(VC, N, VC);
			vec_float_t VCNearZero = VectorInBounds(VC, Epsilon);
			D = VectorMultiplyAdd(VC2, D, T0);

			N = VectorSelect(N, VC, VCNearZero);
			D = VectorSelect(D, g_One.v, VCNearZero);

			vec_float_t R0 = VectorNegate(N);
			vec_float_t R1 = VectorDivide(N, D);
			R0 = VectorDivide(D, R0);

			vec_float_t VIsZero = VectorEqual(V, Zero);

			vec_float_t Result = VectorSelect(R0, R1, VBIsEven);

			Result = VectorSelect(Result, Zero, VIsZero);

			return Result;

#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSinH
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Scale = { { { 1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f } } }; // 1.0f / ln(2.0f)

			vec_float_t V1 = vmlaq_f32(g_NegativeOne.v, V, Scale.v);
			vec_float_t V2 = vmlsq_f32(g_NegativeOne.v, V, Scale.v);
			vec_float_t E1 = VectorExp(V1);
			vec_float_t E2 = VectorExp(V2);

			return vsubq_f32(E1, E2);
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t Scale = { { { 1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f } } }; // 1.0f / ln(2.0f)

			vec_float_t V1 = _mm_mul_ps(V, Scale);
			V1 = _mm_add_ps(V1, g_NegativeOne);
			vec_float_t V2 = _mm_mul_ps(V, Scale);
			V2 = _mm_sub_ps(g_NegativeOne, V2);
			vec_float_t E1 = VectorExp(V1);
			vec_float_t E2 = VectorExp(V2);

			return _mm_sub_ps(E1, E2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorCosH
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Scale = { { { 1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f } } }; // 1.0f / ln(2.0f)

			vec_float_t V1 = vmlaq_f32(g_NegativeOne.v, V, Scale.v);
			vec_float_t V2 = vmlsq_f32(g_NegativeOne.v, V, Scale.v);
			vec_float_t E1 = VectorExp(V1);
			vec_float_t E2 = VectorExp(V2);
			return vaddq_f32(E1, E2);
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t Scale = { { { 1.442695040888963f, 1.442695040888963f, 1.442695040888963f, 1.442695040888963f } } }; // 1.0f / ln(2.0f)

			vec_float_t V1 = _mm_mul_ps(V, Scale.v);
			V1 = _mm_add_ps(V1, g_NegativeOne.v);
			vec_float_t V2 = _mm_mul_ps(V, Scale.v);
			V2 = _mm_sub_ps(g_NegativeOne.v, V2);
			vec_float_t E1 = VectorExp(V1);
			vec_float_t E2 = VectorExp(V2);
			return _mm_add_ps(E1, E2);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorTanH
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Scale = { { { 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f } } }; // 2.0f / ln(2.0f)

			vec_float_t E = vmulq_f32(V, Scale.v);
			E = VectorExp(E);
			E = vmlaq_f32(g_OneHalf.v, E, g_OneHalf.v);
			E = VectorReciprocal(E);
			return vsubq_f32(g_One.v, E);
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t Scale = { { { 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f, 2.8853900817779268f } } }; // 2.0f / ln(2.0f)

			vec_float_t E = _mm_mul_ps(V, Scale.v);
			E = VectorExp(E);
			E = _mm_mul_ps(E, g_OneHalf.v);
			E = _mm_add_ps(E, g_OneHalf.v);
			E = _mm_div_ps(g_One.v, E);
			return _mm_sub_ps(g_One.v, E);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorASin
		(
			vec_float_p1_t V
		)
		{
			// 7-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t nonnegative = vcgeq_f32(V, g_Zero);
			float32x4_t x = vabsq_f32(V);

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			float32x4_t oneMValue = vsubq_f32(g_One, x);
			float32x4_t clampOneMValue = vmaxq_f32(g_Zero, oneMValue);
			float32x4_t root = VectorSqrt(clampOneMValue);

			// Compute polynomial approximation
			const vec_float_t AC1 = g_ArcCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(AC1), 0);
			vec_float_t t0 = vmlaq_lane_f32(vConstants, x, vget_high_f32(AC1), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(AC1), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AC1), 0);
			t0 = vmlaq_f32(vConstants, t0, x);

			const vec_float_t AC0 = g_ArcCoefficients0;
			vConstants = vdupq_lane_f32(vget_high_f32(AC0), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_high_f32(AC0), 0);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AC0), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AC0), 0);
			t0 = vmlaq_f32(vConstants, t0, x);
			t0 = vmulq_f32(t0, root);

			float32x4_t t1 = vsubq_f32(g_Pi, t0);
			t0 = vbslq_f32(nonnegative, t0, t1);
			t0 = vsubq_f32(g_HalfPi, t0);
			return t0;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 nonnegative = _mm_cmpge_ps(V, g_Zero);
			__m128 mvalue = _mm_sub_ps(g_Zero, V);
			__m128 x = _mm_max_ps(V, mvalue);  // |V|

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			__m128 oneMValue = _mm_sub_ps(g_One, x);
			__m128 clampOneMValue = _mm_max_ps(g_Zero, oneMValue);
			__m128 root = _mm_sqrt_ps(clampOneMValue);  // sqrt(1-|V|)

			// Compute polynomial approximation
			const vec_float_t AC1 = g_ArcCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 t0 = _mm_mul_ps(vConstants, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(2, 2, 2, 2));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(1, 1, 1, 1));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(0, 0, 0, 0));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			const vec_float_t AC0 = g_ArcCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(3, 3, 3, 3));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(2, 2, 2, 2));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(1, 1, 1, 1));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(0, 0, 0, 0));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, root);

			__m128 t1 = _mm_sub_ps(g_Pi, t0);
			t0 = _mm_and_ps(nonnegative, t0);
			t1 = _mm_andnot_ps(nonnegative, t1);
			t0 = _mm_or_ps(t0, t1);
			t0 = _mm_sub_ps(g_HalfPi, t0);
			return t0;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorACos
		(
			vec_float_p1_t V
		)
		{
			// 7-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t nonnegative = vcgeq_f32(V, g_Zero);
			float32x4_t x = vabsq_f32(V);

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			float32x4_t oneMValue = vsubq_f32(g_One, x);
			float32x4_t clampOneMValue = vmaxq_f32(g_Zero, oneMValue);
			float32x4_t root = VectorSqrt(clampOneMValue);

			// Compute polynomial approximation
			const vec_float_t AC1 = g_ArcCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(AC1), 0);
			vec_float_t t0 = vmlaq_lane_f32(vConstants, x, vget_high_f32(AC1), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(AC1), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AC1), 0);
			t0 = vmlaq_f32(vConstants, t0, x);

			const vec_float_t AC0 = g_ArcCoefficients0;
			vConstants = vdupq_lane_f32(vget_high_f32(AC0), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_high_f32(AC0), 0);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AC0), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AC0), 0);
			t0 = vmlaq_f32(vConstants, t0, x);
			t0 = vmulq_f32(t0, root);

			float32x4_t t1 = vsubq_f32(g_Pi, t0);
			t0 = vbslq_f32(nonnegative, t0, t1);
			return t0;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 nonnegative = _mm_cmpge_ps(V, g_Zero);
			__m128 mvalue = _mm_sub_ps(g_Zero, V);
			__m128 x = _mm_max_ps(V, mvalue);  // |V|

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			__m128 oneMValue = _mm_sub_ps(g_One, x);
			__m128 clampOneMValue = _mm_max_ps(g_Zero, oneMValue);
			__m128 root = _mm_sqrt_ps(clampOneMValue);  // sqrt(1-|V|)

			// Compute polynomial approximation
			const vec_float_t AC1 = g_ArcCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 t0 = _mm_mul_ps(vConstants, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(2, 2, 2, 2));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(1, 1, 1, 1));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC1, _MM_SHUFFLE(0, 0, 0, 0));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			const vec_float_t AC0 = g_ArcCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(3, 3, 3, 3));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(2, 2, 2, 2));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(1, 1, 1, 1));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AC0, _MM_SHUFFLE(0, 0, 0, 0));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, root);

			__m128 t1 = _mm_sub_ps(g_Pi, t0);
			t0 = _mm_and_ps(nonnegative, t0);
			t1 = _mm_andnot_ps(nonnegative, t1);
			t0 = _mm_or_ps(t0, t1);
			return t0;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorATan
		(
			vec_float_p1_t V
		)
		{
			// 17-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t absV = vabsq_f32(V);
			float32x4_t invV = VectorReciprocal(V);
			uint32x4_t comp = vcgtq_f32(V, g_One);
			uint32x4_t sign = vbslq_f32(comp, g_One, g_NegativeOne);
			comp = vcleq_f32(absV, g_One);
			sign = vbslq_f32(comp, g_Zero, sign);
			uint32x4_t x = vbslq_f32(comp, V, invV);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation
			const vec_float_t TC1 = g_ATanCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(TC1), 0);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_high_f32(TC1), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(TC1), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(TC1), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			const vec_float_t TC0 = g_ATanCoefficients0;
			vConstants = vdupq_lane_f32(vget_high_f32(TC0), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_high_f32(TC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(TC0), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(TC0), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			Result = vmulq_f32(Result, x);

			float32x4_t result1 = vmulq_f32(sign, g_HalfPi);
			result1 = vsubq_f32(result1, Result);

			comp = vceqq_f32(sign, g_Zero);
			Result = vbslq_f32(comp, Result, result1);
			return Result;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 absV = VectorAbs(V);
			__m128 invV = _mm_div_ps(g_One, V);
			__m128 comp = _mm_cmpgt_ps(V, g_One);
			__m128 select0 = _mm_and_ps(comp, g_One);
			__m128 select1 = _mm_andnot_ps(comp, g_NegativeOne);
			__m128 sign = _mm_or_ps(select0, select1);
			comp = _mm_cmple_ps(absV, g_One);
			select0 = _mm_and_ps(comp, g_Zero);
			select1 = _mm_andnot_ps(comp, sign);
			sign = _mm_or_ps(select0, select1);
			select0 = _mm_and_ps(comp, V);
			select1 = _mm_andnot_ps(comp, invV);
			__m128 x = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation
			const vec_float_t TC1 = g_ATanCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(TC1, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(TC1, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(TC1, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(TC1, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			const vec_float_t TC0 = g_ATanCoefficients0;
			vConstants = LUNA_SIMD_PERMUTE_PS(TC0, _MM_SHUFFLE(3, 3, 3, 3));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(TC0, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(TC0, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(TC0, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);
			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, x);
			__m128 result1 = _mm_mul_ps(sign, g_HalfPi);
			result1 = _mm_sub_ps(result1, Result);

			comp = _mm_cmpeq_ps(sign, g_Zero);
			select0 = _mm_and_ps(comp, Result);
			select1 = _mm_andnot_ps(comp, result1);
			Result = _mm_or_ps(select0, select1);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorATan2
		(
			vec_float_p1_t Y,
			vec_float_p1_t X
		)
		{

			// Return the inverse tangent of Y / X in the range of -Pi to Pi with the following exceptions:

			//     Y == 0 and X is Negative         -> Pi with the sign of Y
			//     y == 0 and x is positive         -> 0 with the sign of y
			//     Y != 0 and X == 0                -> Pi / 2 with the sign of Y
			//     Y != 0 and X is Negative         -> atan(y/x) + (PI with the sign of Y)
			//     X == -Infinity and Finite Y      -> Pi with the sign of Y
			//     X == +Infinity and Finite Y      -> 0 with the sign of Y
			//     Y == Infinity and X is Finite    -> Pi / 2 with the sign of Y
			//     Y == Infinity and X == -Infinity -> 3Pi / 4 with the sign of Y
			//     Y == Infinity and X == +Infinity -> Pi / 4 with the sign of Y

			static const vec_f32_t ATan2Constants = { { { pi, pi_div_two, pi_div_four, pi * 3.0f / 4.0f } } };

			vec_float_t Zero = vector_zero();
			vec_float_t ATanResultValid = vector_true_int();

			vec_float_t Pi = vector_splat_x(ATan2Constants);
			vec_float_t PiOverTwo = vector_splat_y(ATan2Constants);
			vec_float_t PiOverFour = vector_splat_z(ATan2Constants);
			vec_float_t ThreePiOverFour = vector_splat_w(ATan2Constants);

			vec_float_t YEqualsZero = VectorEqual(Y, Zero);
			vec_float_t XEqualsZero = VectorEqual(X, Zero);
			vec_float_t XIsPositive = VectorAndInt(X, g_NegativeZero.v);
			XIsPositive = VectorEqualInt(XIsPositive, Zero);
			vec_float_t YEqualsInfinity = VectorIsInfinite(Y);
			vec_float_t XEqualsInfinity = VectorIsInfinite(X);

			vec_float_t YSign = VectorAndInt(Y, g_NegativeZero.v);
			Pi = VectorOrInt(Pi, YSign);
			PiOverTwo = VectorOrInt(PiOverTwo, YSign);
			PiOverFour = VectorOrInt(PiOverFour, YSign);
			ThreePiOverFour = VectorOrInt(ThreePiOverFour, YSign);

			vec_float_t R1 = VectorSelect(Pi, YSign, XIsPositive);
			vec_float_t R2 = VectorSelect(ATanResultValid, PiOverTwo, XEqualsZero);
			vec_float_t R3 = VectorSelect(R2, R1, YEqualsZero);
			vec_float_t R4 = VectorSelect(ThreePiOverFour, PiOverFour, XIsPositive);
			vec_float_t R5 = VectorSelect(PiOverTwo, R4, XEqualsInfinity);
			vec_float_t Result = VectorSelect(R3, R5, YEqualsInfinity);
			ATanResultValid = VectorEqualInt(Result, ATanResultValid);

			vec_float_t V = VectorDivide(Y, X);

			vec_float_t R0 = VectorATan(V);

			R1 = VectorSelect(Pi, g_NegativeZero, XIsPositive);
			R2 = VectorAdd(R0, R1);

			return VectorSelect(Result, R2, ATanResultValid);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorSinEst
		(
			vec_float_p1_t V
		)
		{
			// 7-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with sin(y) = sin(x).
			uint32x4_t sign = vandq_u32(x, g_NegativeZero);
			uint32x4_t c = vorrq_u32(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			float32x4_t absx = vabsq_f32(x);
			float32x4_t rflx = vsubq_f32(c, x);
			uint32x4_t comp = vcleq_f32(absx, g_HalfPi);
			x = vbslq_f32(comp, x, rflx);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation
			const vec_float_t SEC = g_SinCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(SEC), 0);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_high_f32(SEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(SEC), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			Result = vmulq_f32(Result, x);
			return Result;
#elif defined(LUNA_PLATFORM_SSE2)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with sin(y) = sin(x).
			__m128 sign = _mm_and_ps(x, g_NegativeZero);
			__m128 c = _mm_or_ps(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			__m128 absx = _mm_andnot_ps(sign, x);  // |x|
			__m128 rflx = _mm_sub_ps(c, x);
			__m128 comp = _mm_cmple_ps(absx, g_HalfPi);
			__m128 select0 = _mm_and_ps(comp, x);
			__m128 select1 = _mm_andnot_ps(comp, rflx);
			x = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation
			const vec_float_t SEC = g_SinCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(SEC, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SEC, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SEC, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, x);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorCosEst
		(
			vec_float_p1_t V
		)
		{
			// 6-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
	// Map V to x in [-pi,pi].
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with cos(y) = sign*cos(x).
			uint32x4_t sign = vandq_u32(x, g_NegativeZero);
			uint32x4_t c = vorrq_u32(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			float32x4_t absx = vabsq_f32(x);
			float32x4_t rflx = vsubq_f32(c, x);
			uint32x4_t comp = vcleq_f32(absx, g_HalfPi);
			x = vbslq_f32(comp, x, rflx);
			sign = vbslq_f32(comp, g_One, g_NegativeOne);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation
			const vec_float_t CEC = g_CosCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(CEC), 0);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_high_f32(CEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(CEC), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			Result = vmulq_f32(Result, sign);
			return Result;
#elif defined(LUNA_PLATFORM_SSE2)
	// Map V to x in [-pi,pi].
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with cos(y) = sign*cos(x).
			vec_float_t sign = _mm_and_ps(x, g_NegativeZero);
			__m128 c = _mm_or_ps(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			__m128 absx = _mm_andnot_ps(sign, x);  // |x|
			__m128 rflx = _mm_sub_ps(c, x);
			__m128 comp = _mm_cmple_ps(absx, g_HalfPi);
			__m128 select0 = _mm_and_ps(comp, x);
			__m128 select1 = _mm_andnot_ps(comp, rflx);
			x = _mm_or_ps(select0, select1);
			select0 = _mm_and_ps(comp, g_One);
			select1 = _mm_andnot_ps(comp, g_NegativeOne);
			sign = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation
			const vec_float_t CEC = g_CosCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(CEC, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CEC, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CEC, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, sign);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------


		inline void LUNA_SIMD_CALLCONV VectorSinCosEst
		(
			vec_float_t* pSin,
			vec_float_t* pCos,
			vec_float_p1_t  V
		)
		{
			luassert(pSin != nullptr);
			luassert(pCos != nullptr);

			// 7/6-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with cos(y) = sign*cos(x).
			uint32x4_t sign = vandq_u32(x, g_NegativeZero);
			uint32x4_t c = vorrq_u32(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			float32x4_t absx = vabsq_f32(x);
			float32x4_t rflx = vsubq_f32(c, x);
			uint32x4_t comp = vcleq_f32(absx, g_HalfPi);
			x = vbslq_f32(comp, x, rflx);
			sign = vbslq_f32(comp, g_One, g_NegativeOne);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation for sine
			const vec_float_t SEC = g_SinCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(SEC), 0);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_high_f32(SEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(SEC), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			*pSin = vmulq_f32(Result, x);

			// Compute polynomial approximation
			const vec_float_t CEC = g_CosCoefficients1;
			vConstants = vdupq_lane_f32(vget_high_f32(CEC), 0);
			Result = vmlaq_lane_f32(vConstants, x2, vget_high_f32(CEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(CEC), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			Result = vmlaq_f32(g_One, Result, x2);
			*pCos = vmulq_f32(Result, sign);
#elif defined(LUNA_PLATFORM_SSE2)
	// Force the value within the bounds of pi
			vec_float_t x = VectorModAngles(V);

			// Map in [-pi/2,pi/2] with sin(y) = sin(x), cos(y) = sign*cos(x).
			vec_float_t sign = _mm_and_ps(x, g_NegativeZero);
			__m128 c = _mm_or_ps(g_Pi, sign);  // pi when x >= 0, -pi when x < 0
			__m128 absx = _mm_andnot_ps(sign, x);  // |x|
			__m128 rflx = _mm_sub_ps(c, x);
			__m128 comp = _mm_cmple_ps(absx, g_HalfPi);
			__m128 select0 = _mm_and_ps(comp, x);
			__m128 select1 = _mm_andnot_ps(comp, rflx);
			x = _mm_or_ps(select0, select1);
			select0 = _mm_and_ps(comp, g_One);
			select1 = _mm_andnot_ps(comp, g_NegativeOne);
			sign = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation for sine
			const vec_float_t SEC = g_SinCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(SEC, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SEC, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(SEC, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, x);
			*pSin = Result;

			// Compute polynomial approximation for cosine
			const vec_float_t CEC = g_CosCoefficients1;
			vConstants = LUNA_SIMD_PERMUTE_PS(CEC, _MM_SHUFFLE(3, 3, 3, 3));
			Result = _mm_mul_ps(vConstants, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CEC, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(CEC, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			Result = _mm_add_ps(Result, g_One);
			Result = _mm_mul_ps(Result, sign);
			*pCos = Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorTanEst
		(
			vec_float_p1_t V
		)
		{

			vec_float_t OneOverPi = vector_splat_w(g_TanEstCoefficients.v);

			vec_float_t V1 = VectorMultiply(V, OneOverPi);
			V1 = VectorRound(V1);

			V1 = VectorNegativeMultiplySubtract(g_Pi.v, V1, V);

			vec_float_t T0 = vector_splat_x(g_TanEstCoefficients.v);
			vec_float_t T1 = vector_splat_y(g_TanEstCoefficients.v);
			vec_float_t T2 = vector_splat_z(g_TanEstCoefficients.v);

			vec_float_t V2T2 = VectorNegativeMultiplySubtract(V1, V1, T2);
			vec_float_t V2 = VectorMultiply(V1, V1);
			vec_float_t V1T0 = VectorMultiply(V1, T0);
			vec_float_t V1T1 = VectorMultiply(V1, T1);

			vec_float_t D = VectorReciprocalEst(V2T2);
			vec_float_t N = VectorMultiplyAdd(V2, V1T1, V1T0);

			return VectorMultiply(N, D);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorASinEst
		(
			vec_float_p1_t V
		)
		{
			// 3-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t nonnegative = vcgeq_f32(V, g_Zero);
			float32x4_t x = vabsq_f32(V);

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			float32x4_t oneMValue = vsubq_f32(g_One, x);
			float32x4_t clampOneMValue = vmaxq_f32(g_Zero, oneMValue);
			float32x4_t root = VectorSqrt(clampOneMValue);

			// Compute polynomial approximation
			const vec_float_t AEC = g_ArcEstCoefficients;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(AEC), 0);
			vec_float_t t0 = vmlaq_lane_f32(vConstants, x, vget_high_f32(AEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(AEC), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AEC), 0);
			t0 = vmlaq_f32(vConstants, t0, x);
			t0 = vmulq_f32(t0, root);

			float32x4_t t1 = vsubq_f32(g_Pi, t0);
			t0 = vbslq_f32(nonnegative, t0, t1);
			t0 = vsubq_f32(g_HalfPi, t0);
			return t0;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 nonnegative = _mm_cmpge_ps(V, g_Zero);
			__m128 mvalue = _mm_sub_ps(g_Zero, V);
			__m128 x = _mm_max_ps(V, mvalue);  // |V|

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			__m128 oneMValue = _mm_sub_ps(g_One, x);
			__m128 clampOneMValue = _mm_max_ps(g_Zero, oneMValue);
			__m128 root = _mm_sqrt_ps(clampOneMValue);  // sqrt(1-|V|)

			// Compute polynomial approximation
			const vec_float_t AEC = g_ArcEstCoefficients;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 t0 = _mm_mul_ps(vConstants, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(2, 2, 2, 2));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(1, 1, 1, 1));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(0, 0, 0, 0));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, root);

			__m128 t1 = _mm_sub_ps(g_Pi, t0);
			t0 = _mm_and_ps(nonnegative, t0);
			t1 = _mm_andnot_ps(nonnegative, t1);
			t0 = _mm_or_ps(t0, t1);
			t0 = _mm_sub_ps(g_HalfPi, t0);
			return t0;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorACosEst
		(
			vec_float_p1_t V
		)
		{
			// 3-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t nonnegative = vcgeq_f32(V, g_Zero);
			float32x4_t x = vabsq_f32(V);

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			float32x4_t oneMValue = vsubq_f32(g_One, x);
			float32x4_t clampOneMValue = vmaxq_f32(g_Zero, oneMValue);
			float32x4_t root = VectorSqrt(clampOneMValue);

			// Compute polynomial approximation
			const vec_float_t AEC = g_ArcEstCoefficients;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(AEC), 0);
			vec_float_t t0 = vmlaq_lane_f32(vConstants, x, vget_high_f32(AEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(AEC), 1);
			t0 = vmlaq_f32(vConstants, t0, x);

			vConstants = vdupq_lane_f32(vget_low_f32(AEC), 0);
			t0 = vmlaq_f32(vConstants, t0, x);
			t0 = vmulq_f32(t0, root);

			float32x4_t t1 = vsubq_f32(g_Pi, t0);
			t0 = vbslq_f32(nonnegative, t0, t1);
			return t0;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 nonnegative = _mm_cmpge_ps(V, g_Zero);
			__m128 mvalue = _mm_sub_ps(g_Zero, V);
			__m128 x = _mm_max_ps(V, mvalue);  // |V|

			// Compute (1-|V|), clamp to zero to avoid sqrt of negative number.
			__m128 oneMValue = _mm_sub_ps(g_One, x);
			__m128 clampOneMValue = _mm_max_ps(g_Zero, oneMValue);
			__m128 root = _mm_sqrt_ps(clampOneMValue);  // sqrt(1-|V|)

			// Compute polynomial approximation
			const vec_float_t AEC = g_ArcEstCoefficients;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 t0 = _mm_mul_ps(vConstants, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(2, 2, 2, 2));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(1, 1, 1, 1));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, x);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(0, 0, 0, 0));
			t0 = _mm_add_ps(t0, vConstants);
			t0 = _mm_mul_ps(t0, root);

			__m128 t1 = _mm_sub_ps(g_Pi, t0);
			t0 = _mm_and_ps(nonnegative, t0);
			t1 = _mm_andnot_ps(nonnegative, t1);
			t0 = _mm_or_ps(t0, t1);
			return t0;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorATanEst
		(
			vec_float_p1_t V
		)
		{
			// 9-degree minimax approximation

#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t absV = vabsq_f32(V);
			float32x4_t invV = VectorReciprocalEst(V);
			uint32x4_t comp = vcgtq_f32(V, g_One);
			uint32x4_t sign = vbslq_f32(comp, g_One, g_NegativeOne);
			comp = vcleq_f32(absV, g_One);
			sign = vbslq_f32(comp, g_Zero, sign);
			uint32x4_t x = vbslq_f32(comp, V, invV);

			float32x4_t x2 = vmulq_f32(x, x);

			// Compute polynomial approximation
			const vec_float_t AEC = g_ATanEstCoefficients1;
			vec_float_t vConstants = vdupq_lane_f32(vget_high_f32(AEC), 0);
			vec_float_t Result = vmlaq_lane_f32(vConstants, x2, vget_high_f32(AEC), 1);

			vConstants = vdupq_lane_f32(vget_low_f32(AEC), 1);
			Result = vmlaq_f32(vConstants, Result, x2);

			vConstants = vdupq_lane_f32(vget_low_f32(AEC), 0);
			Result = vmlaq_f32(vConstants, Result, x2);

			// ATanEstCoefficients0 is already splatted
			Result = vmlaq_f32(g_ATanEstCoefficients0, Result, x2);
			Result = vmulq_f32(Result, x);

			float32x4_t result1 = vmulq_f32(sign, g_HalfPi);
			result1 = vsubq_f32(result1, Result);

			comp = vceqq_f32(sign, g_Zero);
			Result = vbslq_f32(comp, Result, result1);
			return Result;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 absV = VectorAbs(V);
			__m128 invV = _mm_div_ps(g_One, V);
			__m128 comp = _mm_cmpgt_ps(V, g_One);
			__m128 select0 = _mm_and_ps(comp, g_One);
			__m128 select1 = _mm_andnot_ps(comp, g_NegativeOne);
			__m128 sign = _mm_or_ps(select0, select1);
			comp = _mm_cmple_ps(absV, g_One);
			select0 = _mm_and_ps(comp, g_Zero);
			select1 = _mm_andnot_ps(comp, sign);
			sign = _mm_or_ps(select0, select1);
			select0 = _mm_and_ps(comp, V);
			select1 = _mm_andnot_ps(comp, invV);
			__m128 x = _mm_or_ps(select0, select1);

			__m128 x2 = _mm_mul_ps(x, x);

			// Compute polynomial approximation
			const vec_float_t AEC = g_ATanEstCoefficients1;
			vec_float_t vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(3, 3, 3, 3));
			__m128 Result = _mm_mul_ps(vConstants, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(2, 2, 2, 2));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(1, 1, 1, 1));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			vConstants = LUNA_SIMD_PERMUTE_PS(AEC, _MM_SHUFFLE(0, 0, 0, 0));
			Result = _mm_add_ps(Result, vConstants);
			Result = _mm_mul_ps(Result, x2);

			// ATanEstCoefficients0 is already splatted
			Result = _mm_add_ps(Result, g_ATanEstCoefficients0);
			Result = _mm_mul_ps(Result, x);
			__m128 result1 = _mm_mul_ps(sign, g_HalfPi);
			result1 = _mm_sub_ps(result1, Result);

			comp = _mm_cmpeq_ps(sign, g_Zero);
			select0 = _mm_and_ps(comp, Result);
			select1 = _mm_andnot_ps(comp, result1);
			Result = _mm_or_ps(select0, select1);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorATan2Est
		(
			vec_float_p1_t Y,
			vec_float_p1_t X
		)
		{
			static const vec_f32_t ATan2Constants = { { { pi, pi_div_two, pi_div_four, 2.3561944905f /* Pi*3/4 */ } } };

			const vec_float_t Zero = vector_zero();
			vec_float_t ATanResultValid = vector_true_int();

			vec_float_t Pi = vector_splat_x(ATan2Constants);
			vec_float_t PiOverTwo = vector_splat_y(ATan2Constants);
			vec_float_t PiOverFour = vector_splat_z(ATan2Constants);
			vec_float_t ThreePiOverFour = vector_splat_w(ATan2Constants);

			vec_float_t YEqualsZero = VectorEqual(Y, Zero);
			vec_float_t XEqualsZero = VectorEqual(X, Zero);
			vec_float_t XIsPositive = VectorAndInt(X, g_NegativeZero.v);
			XIsPositive = VectorEqualInt(XIsPositive, Zero);
			vec_float_t YEqualsInfinity = VectorIsInfinite(Y);
			vec_float_t XEqualsInfinity = VectorIsInfinite(X);

			vec_float_t YSign = VectorAndInt(Y, g_NegativeZero.v);
			Pi = VectorOrInt(Pi, YSign);
			PiOverTwo = VectorOrInt(PiOverTwo, YSign);
			PiOverFour = VectorOrInt(PiOverFour, YSign);
			ThreePiOverFour = VectorOrInt(ThreePiOverFour, YSign);

			vec_float_t R1 = VectorSelect(Pi, YSign, XIsPositive);
			vec_float_t R2 = VectorSelect(ATanResultValid, PiOverTwo, XEqualsZero);
			vec_float_t R3 = VectorSelect(R2, R1, YEqualsZero);
			vec_float_t R4 = VectorSelect(ThreePiOverFour, PiOverFour, XIsPositive);
			vec_float_t R5 = VectorSelect(PiOverTwo, R4, XEqualsInfinity);
			vec_float_t Result = VectorSelect(R3, R5, YEqualsInfinity);
			ATanResultValid = VectorEqualInt(Result, ATanResultValid);

			vec_float_t Reciprocal = VectorReciprocalEst(X);
			vec_float_t V = VectorMultiply(Y, Reciprocal);
			vec_float_t R0 = VectorATanEst(V);

			R1 = VectorSelect(Pi, g_NegativeZero, XIsPositive);
			R2 = VectorAdd(R0, R1);

			Result = VectorSelect(Result, R2, ATanResultValid);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLerp
		(
			vec_float_p1_t V0,
			vec_float_p1_t V1,
			f32    t
		)
		{
			// V0 + t * (V1 - V0)

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t L = vsubq_f32(V1, V0);
			return vmlaq_n_f32(V0, L, t);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t L = _mm_sub_ps(V1, V0);
			vec_float_t S = _mm_set_ps1(t);
			vec_float_t Result = _mm_mul_ps(L, S);
			return _mm_add_ps(Result, V0);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorLerpV
		(
			vec_float_p1_t V0,
			vec_float_p1_t V1,
			vec_float_p1_t T
		)
		{
			// V0 + T * (V1 - V0)

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t L = vsubq_f32(V1, V0);
			return vmlaq_f32(V0, L, T);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t length = _mm_sub_ps(V1, V0);
			vec_float_t Result = _mm_mul_ps(length, T);
			return _mm_add_ps(Result, V0);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorHermite
		(
			vec_float_p1_t Position0,
			vec_float_p1_t Tangent0,
			vec_float_p1_t Position1,
			vec_float_p2_t Tangent1,
			f32    t
		)
		{
			// Result = (2 * t^3 - 3 * t^2 + 1) * Position0 +
			//          (t^3 - 2 * t^2 + t) * Tangent0 +
			//          (-2 * t^3 + 3 * t^2) * Position1 +
			//          (t^3 - t^2) * Tangent1

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 t2 = t * t;
			f32 t3 = t * t2;

			f32 p0 = 2.0f * t3 - 3.0f * t2 + 1.0f;
			f32 t0 = t3 - 2.0f * t2 + t;
			f32 p1 = -2.0f * t3 + 3.0f * t2;
			f32 t1 = t3 - t2;

			vec_float_t vResult = vmulq_n_f32(Position0, p0);
			vResult = vmlaq_n_f32(vResult, Tangent0, t0);
			vResult = vmlaq_n_f32(vResult, Position1, p1);
			vResult = vmlaq_n_f32(vResult, Tangent1, t1);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			f32 t2 = t * t;
			f32 t3 = t * t2;

			vec_float_t P0 = _mm_set_ps1(2.0f * t3 - 3.0f * t2 + 1.0f);
			vec_float_t T0 = _mm_set_ps1(t3 - 2.0f * t2 + t);
			vec_float_t P1 = _mm_set_ps1(-2.0f * t3 + 3.0f * t2);
			vec_float_t T1 = _mm_set_ps1(t3 - t2);

			vec_float_t vResult = _mm_mul_ps(P0, Position0);
			vec_float_t vTemp = _mm_mul_ps(T0, Tangent0);
			vResult = _mm_add_ps(vResult, vTemp);
			vTemp = _mm_mul_ps(P1, Position1);
			vResult = _mm_add_ps(vResult, vTemp);
			vTemp = _mm_mul_ps(T1, Tangent1);
			vResult = _mm_add_ps(vResult, vTemp);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorHermiteV
		(
			vec_float_p1_t Position0,
			vec_float_p1_t Tangent0,
			vec_float_p1_t Position1,
			vec_float_p2_t Tangent1,
			vec_float_p3_t T
		)
		{
			// Result = (2 * t^3 - 3 * t^2 + 1) * Position0 +
			//          (t^3 - 2 * t^2 + t) * Tangent0 +
			//          (-2 * t^3 + 3 * t^2) * Position1 +
			//          (t^3 - t^2) * Tangent1

#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t CatMulT2 = { { { -3.0f, -2.0f, 3.0f, -1.0f } } };
			static const vec_f32_t CatMulT3 = { { { 2.0f, 1.0f, -2.0f, 1.0f } } };

			vec_float_t T2 = vmulq_f32(T, T);
			vec_float_t T3 = vmulq_f32(T, T2);
			// Mul by the constants against t^2
			T2 = vmulq_f32(T2, CatMulT2);
			// Mul by the constants against t^3
			T3 = vmlaq_f32(T2, T3, CatMulT3);
			// T3 now has the pre-result.
			// I need to add t.y only
			T2 = vandq_u32(T, g_MaskY);
			T3 = vaddq_f32(T3, T2);
			// Add 1.0f to x
			T3 = vaddq_f32(T3, g_IdentityR0);
			// Now, I have the constants created
			// Mul the x constant to Position0
			vec_float_t vResult = vmulq_lane_f32(Position0, vget_low_f32(T3), 0); // T3[0]
			// Mul the y constant to Tangent0
			vResult = vmlaq_lane_f32(vResult, Tangent0, vget_low_f32(T3), 1); // T3[1]
			// Mul the z constant to Position1
			vResult = vmlaq_lane_f32(vResult, Position1, vget_high_f32(T3), 0); // T3[2]
			// Mul the w constant to Tangent1
			vResult = vmlaq_lane_f32(vResult, Tangent1, vget_high_f32(T3), 1); // T3[3]
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t CatMulT2 = { { { -3.0f, -2.0f, 3.0f, -1.0f } } };
			static const vec_f32_t CatMulT3 = { { { 2.0f, 1.0f, -2.0f, 1.0f } } };

			vec_float_t T2 = _mm_mul_ps(T, T);
			vec_float_t T3 = _mm_mul_ps(T, T2);
			// Mul by the constants against t^2
			T2 = _mm_mul_ps(T2, CatMulT2);
			// Mul by the constants against t^3
			T3 = _mm_mul_ps(T3, CatMulT3);
			// T3 now has the pre-result.
			T3 = _mm_add_ps(T3, T2);
			// I need to add t.y only
			T2 = _mm_and_ps(T, g_MaskY);
			T3 = _mm_add_ps(T3, T2);
			// Add 1.0f to x
			T3 = _mm_add_ps(T3, g_IdentityR0);
			// Now, I have the constants created
			// Mul the x constant to Position0
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(T3, _MM_SHUFFLE(0, 0, 0, 0));
			vResult = _mm_mul_ps(vResult, Position0);
			// Mul the y constant to Tangent0
			T2 = LUNA_SIMD_PERMUTE_PS(T3, _MM_SHUFFLE(1, 1, 1, 1));
			T2 = _mm_mul_ps(T2, Tangent0);
			vResult = _mm_add_ps(vResult, T2);
			// Mul the z constant to Position1
			T2 = LUNA_SIMD_PERMUTE_PS(T3, _MM_SHUFFLE(2, 2, 2, 2));
			T2 = _mm_mul_ps(T2, Position1);
			vResult = _mm_add_ps(vResult, T2);
			// Mul the w constant to Tangent1
			T3 = LUNA_SIMD_PERMUTE_PS(T3, _MM_SHUFFLE(3, 3, 3, 3));
			T3 = _mm_mul_ps(T3, Tangent1);
			vResult = _mm_add_ps(vResult, T3);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorCatmullRom
		(
			vec_float_p1_t Position0,
			vec_float_p1_t Position1,
			vec_float_p1_t Position2,
			vec_float_p2_t Position3,
			f32    t
		)
		{
			// Result = ((-t^3 + 2 * t^2 - t) * Position0 +
			//           (3 * t^3 - 5 * t^2 + 2) * Position1 +
			//           (-3 * t^3 + 4 * t^2 + t) * Position2 +
			//           (t^3 - t^2) * Position3) * 0.5

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 t2 = t * t;
			f32 t3 = t * t2;

			f32 p0 = (-t3 + 2.0f * t2 - t) * 0.5f;
			f32 p1 = (3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
			f32 p2 = (-3.0f * t3 + 4.0f * t2 + t) * 0.5f;
			f32 p3 = (t3 - t2) * 0.5f;

			vec_float_t P1 = vmulq_n_f32(Position1, p1);
			vec_float_t P0 = vmlaq_n_f32(P1, Position0, p0);
			vec_float_t P3 = vmulq_n_f32(Position3, p3);
			vec_float_t P2 = vmlaq_n_f32(P3, Position2, p2);
			P0 = vaddq_f32(P0, P2);
			return P0;
#elif defined(LUNA_PLATFORM_SSE2)
			f32 t2 = t * t;
			f32 t3 = t * t2;

			vec_float_t P0 = _mm_set_ps1((-t3 + 2.0f * t2 - t) * 0.5f);
			vec_float_t P1 = _mm_set_ps1((3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f);
			vec_float_t P2 = _mm_set_ps1((-3.0f * t3 + 4.0f * t2 + t) * 0.5f);
			vec_float_t P3 = _mm_set_ps1((t3 - t2) * 0.5f);

			P0 = _mm_mul_ps(P0, Position0);
			P1 = _mm_mul_ps(P1, Position1);
			P2 = _mm_mul_ps(P2, Position2);
			P3 = _mm_mul_ps(P3, Position3);
			P0 = _mm_add_ps(P0, P1);
			P2 = _mm_add_ps(P2, P3);
			P0 = _mm_add_ps(P0, P2);
			return P0;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorCatmullRomV
		(
			vec_float_p1_t Position0,
			vec_float_p1_t Position1,
			vec_float_p1_t Position2,
			vec_float_p2_t Position3,
			vec_float_p3_t T
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Catmul2 = { { { 2.0f, 2.0f, 2.0f, 2.0f } } };
			static const vec_f32_t Catmul3 = { { { 3.0f, 3.0f, 3.0f, 3.0f } } };
			static const vec_f32_t Catmul4 = { { { 4.0f, 4.0f, 4.0f, 4.0f } } };
			static const vec_f32_t Catmul5 = { { { 5.0f, 5.0f, 5.0f, 5.0f } } };
			// Cache T^2 and T^3
			vec_float_t T2 = vmulq_f32(T, T);
			vec_float_t T3 = vmulq_f32(T, T2);
			// Perform the Position0 term
			vec_float_t vResult = vaddq_f32(T2, T2);
			vResult = vsubq_f32(vResult, T);
			vResult = vsubq_f32(vResult, T3);
			vResult = vmulq_f32(vResult, Position0);
			// Perform the Position1 term and add
			vec_float_t vTemp = vmulq_f32(T3, Catmul3);
			vTemp = vmlsq_f32(vTemp, T2, Catmul5);
			vTemp = vaddq_f32(vTemp, Catmul2);
			vResult = vmlaq_f32(vResult, vTemp, Position1);
			// Perform the Position2 term and add
			vTemp = vmulq_f32(T2, Catmul4);
			vTemp = vmlsq_f32(vTemp, T3, Catmul3);
			vTemp = vaddq_f32(vTemp, T);
			vResult = vmlaq_f32(vResult, vTemp, Position2);
			// Position3 is the last term
			T3 = vsubq_f32(T3, T2);
			vResult = vmlaq_f32(vResult, T3, Position3);
			// Multiply by 0.5f and exit
			vResult = vmulq_f32(vResult, g_OneHalf);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t Catmul2 = { { { 2.0f, 2.0f, 2.0f, 2.0f } } };
			static const vec_f32_t Catmul3 = { { { 3.0f, 3.0f, 3.0f, 3.0f } } };
			static const vec_f32_t Catmul4 = { { { 4.0f, 4.0f, 4.0f, 4.0f } } };
			static const vec_f32_t Catmul5 = { { { 5.0f, 5.0f, 5.0f, 5.0f } } };
			// Cache T^2 and T^3
			vec_float_t T2 = _mm_mul_ps(T, T);
			vec_float_t T3 = _mm_mul_ps(T, T2);
			// Perform the Position0 term
			vec_float_t vResult = _mm_add_ps(T2, T2);
			vResult = _mm_sub_ps(vResult, T);
			vResult = _mm_sub_ps(vResult, T3);
			vResult = _mm_mul_ps(vResult, Position0);
			// Perform the Position1 term and add
			vec_float_t vTemp = _mm_mul_ps(T3, Catmul3);
			vec_float_t vTemp2 = _mm_mul_ps(T2, Catmul5);
			vTemp = _mm_sub_ps(vTemp, vTemp2);
			vTemp = _mm_add_ps(vTemp, Catmul2);
			vTemp = _mm_mul_ps(vTemp, Position1);
			vResult = _mm_add_ps(vResult, vTemp);
			// Perform the Position2 term and add
			vTemp = _mm_mul_ps(T2, Catmul4);
			vTemp2 = _mm_mul_ps(T3, Catmul3);
			vTemp = _mm_sub_ps(vTemp, vTemp2);
			vTemp = _mm_add_ps(vTemp, T);
			vTemp = _mm_mul_ps(vTemp, Position2);
			vResult = _mm_add_ps(vResult, vTemp);
			// Position3 is the last term
			T3 = _mm_sub_ps(T3, T2);
			T3 = _mm_mul_ps(T3, Position3);
			vResult = _mm_add_ps(vResult, T3);
			// Multiply by 0.5f and exit
			vResult = _mm_mul_ps(vResult, g_OneHalf);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorBaryCentric
		(
			vec_float_p1_t Position0,
			vec_float_p1_t Position1,
			vec_float_p1_t Position2,
			f32    f,
			f32    g
		)
		{
			// Result = Position0 + f * (Position1 - Position0) + g * (Position2 - Position0)

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t R1 = vsubq_f32(Position1, Position0);
			vec_float_t R2 = vsubq_f32(Position2, Position0);
			R1 = vmlaq_n_f32(Position0, R1, f);
			return vmlaq_n_f32(R1, R2, g);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t R1 = _mm_sub_ps(Position1, Position0);
			vec_float_t SF = _mm_set_ps1(f);
			vec_float_t R2 = _mm_sub_ps(Position2, Position0);
			vec_float_t SG = _mm_set_ps1(g);
			R1 = _mm_mul_ps(R1, SF);
			R2 = _mm_mul_ps(R2, SG);
			R1 = _mm_add_ps(R1, Position0);
			R1 = _mm_add_ps(R1, R2);
			return R1;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV VectorBaryCentricV
		(
			vec_float_p1_t Position0,
			vec_float_p1_t Position1,
			vec_float_p1_t Position2,
			vec_float_p2_t F,
			vec_float_p3_t G
		)
		{
			// Result = Position0 + f * (Position1 - Position0) + g * (Position2 - Position0)

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t R1 = vsubq_f32(Position1, Position0);
			vec_float_t R2 = vsubq_f32(Position2, Position0);
			R1 = vmlaq_f32(Position0, R1, F);
			return vmlaq_f32(R1, R2, G);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t R1 = _mm_sub_ps(Position1, Position0);
			vec_float_t R2 = _mm_sub_ps(Position2, Position0);
			R1 = _mm_mul_ps(R1, F);
			R2 = _mm_mul_ps(R2, G);
			R1 = _mm_add_ps(R1, Position0);
			R1 = _mm_add_ps(R1, R2);
			return R1;
#endif
		}

		/****************************************************************************
		 *
		 * 2D Vector
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2Equal
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vceq_f32(vget_low_f32(V1), vget_low_f32(V2));
			return (vget_lane_u64(vTemp, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			// z and w are don't care
			return (((_mm_movemask_ps(vTemp) & 3) == 3) != 0);
#endif
		}


		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector2EqualR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vceq_f32(vget_low_f32(V1), vget_low_f32(V2));
			uint64_t r = vget_lane_u64(vTemp, 0);
			u32 CR = 0;
			if (r == 0xFFFFFFFFFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			// z and w are don't care
			int iTest = _mm_movemask_ps(vTemp) & 3;
			u32 CR = 0;
			if (iTest == 3)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2EqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vceq_u32(vget_low_u32(V1), vget_low_u32(V2));
			return (vget_lane_u64(vTemp, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return (((_mm_movemask_ps(_mm_castsi128_ps(vTemp)) & 3) == 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector2EqualIntR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vceq_u32(vget_low_u32(V1), vget_low_u32(V2));
			uint64_t r = vget_lane_u64(vTemp, 0);
			u32 CR = 0;
			if (r == 0xFFFFFFFFFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			int iTest = _mm_movemask_ps(_mm_castsi128_ps(vTemp)) & 3;
			u32 CR = 0;
			if (iTest == 3)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2NearEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t Epsilon
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t vDelta = vsub_f32(vget_low_u32(V1), vget_low_u32(V2));
			uint32x2_t vTemp = vacle_f32(vDelta, vget_low_u32(Epsilon));
			uint64_t r = vget_lane_u64(vTemp, 0);
			return (r == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Get the difference
			vec_float_t vDelta = _mm_sub_ps(V1, V2);
			// Get the absolute value of the difference
			vec_float_t vTemp = _mm_setzero_ps();
			vTemp = _mm_sub_ps(vTemp, vDelta);
			vTemp = _mm_max_ps(vTemp, vDelta);
			vTemp = _mm_cmple_ps(vTemp, Epsilon);
			// z and w are don't care
			return (((_mm_movemask_ps(vTemp) & 3) == 0x3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2NotEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vceq_f32(vget_low_f32(V1), vget_low_f32(V2));
			return (vget_lane_u64(vTemp, 0) != 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			// z and w are don't care
			return (((_mm_movemask_ps(vTemp) & 3) != 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2NotEqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vceq_u32(vget_low_u32(V1), vget_low_u32(V2));
			return (vget_lane_u64(vTemp, 0) != 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return (((_mm_movemask_ps(_mm_castsi128_ps(vTemp)) & 3) != 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2Greater
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vcgt_f32(vget_low_f32(V1), vget_low_f32(V2));
			return (vget_lane_u64(vTemp, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			// z and w are don't care
			return (((_mm_movemask_ps(vTemp) & 3) == 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector2GreaterR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vcgt_f32(vget_low_f32(V1), vget_low_f32(V2));
			uint64_t r = vget_lane_u64(vTemp, 0);
			u32 CR = 0;
			if (r == 0xFFFFFFFFFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			int iTest = _mm_movemask_ps(vTemp) & 3;
			u32 CR = 0;
			if (iTest == 3)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2GreaterOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vcge_f32(vget_low_f32(V1), vget_low_f32(V2));
			return (vget_lane_u64(vTemp, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 3) == 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector2GreaterOrEqualR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vcge_f32(vget_low_f32(V1), vget_low_f32(V2));
			uint64_t r = vget_lane_u64(vTemp, 0);
			u32 CR = 0;
			if (r == 0xFFFFFFFFFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			int iTest = _mm_movemask_ps(vTemp) & 3;
			u32 CR = 0;
			if (iTest == 3)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2Less
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vclt_f32(vget_low_f32(V1), vget_low_f32(V2));
			return (vget_lane_u64(vTemp, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmplt_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 3) == 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2LessOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t vTemp = vcle_f32(vget_low_f32(V1), vget_low_f32(V2));
			return (vget_lane_u64(vTemp, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmple_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 3) == 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2InBounds
		(
			vec_float_p1_t V,
			vec_float_p1_t Bounds
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			float32x2_t B = vget_low_f32(Bounds);
			// Test if less than or equal
			uint32x2_t ivTemp1 = vcle_f32(VL, B);
			// Negate the bounds
			float32x2_t vTemp2 = vneg_f32(B);
			// Test if greater or equal (Reversed)
			uint32x2_t ivTemp2 = vcle_f32(vTemp2, VL);
			// Blend answers
			ivTemp1 = vand_u32(ivTemp1, ivTemp2);
			// x and y in bounds?
			return (vget_lane_u64(ivTemp1, 0) == 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test if less than or equal
			vec_float_t vTemp1 = _mm_cmple_ps(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = _mm_mul_ps(Bounds, g_NegativeOne);
			// Test if greater or equal (Reversed)
			vTemp2 = _mm_cmple_ps(vTemp2, V);
			// Blend answers
			vTemp1 = _mm_and_ps(vTemp1, vTemp2);
			// x and y in bounds? (z and w are don't care)
			return (((_mm_movemask_ps(vTemp1) & 0x3) == 0x3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2IsNaN
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Test against itself. NaN is always not equal
			uint32x2_t vTempNan = vceq_f32(VL, VL);
			// If x or y are NaN, the mask is zero
			return (vget_lane_u64(vTempNan, 0) != 0xFFFFFFFFFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test against itself. NaN is always not equal
			vec_float_t vTempNan = _mm_cmpneq_ps(V, V);
			// If x or y are NaN, the mask is non-zero
			return ((_mm_movemask_ps(vTempNan) & 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector2IsInfinite
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Mask off the sign bit
			uint32x2_t vTemp = vand_u32(vget_low_f32(V), vget_low_f32(g_AbsMask));
			// Compare to infinity
			vTemp = vceq_f32(vTemp, vget_low_f32(g_Infinity));
			// If any are infinity, the signs are true.
			return vget_lane_u64(vTemp, 0) != 0;
#elif defined(LUNA_PLATFORM_SSE2)
			// Mask off the sign bit
			__m128 vTemp = _mm_and_ps(V, g_AbsMask);
			// Compare to infinity
			vTemp = _mm_cmpeq_ps(vTemp, g_Infinity);
			// If x or z are infinity, the signs are true.
			return ((_mm_movemask_ps(vTemp) & 3) != 0);
#endif
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Dot
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Perform the dot product on x and y
			float32x2_t vTemp = vmul_f32(vget_low_f32(V1), vget_low_f32(V2));
			vTemp = vpadd_f32(vTemp, vTemp);
			return vcombine_f32(vTemp, vTemp);
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_dp_ps(V1, V2, 0x3f);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vDot = _mm_mul_ps(V1, V2);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_moveldup_ps(vDot);
			return vDot;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y
			vec_float_t vLengthSq = _mm_mul_ps(V1, V2);
			// vTemp has y splatted
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			// x+y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Cross
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			// [ V1.x*V2.y - V1.y*V2.x, V1.x*V2.y - V1.y*V2.x ]

#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Negate = { { { 1.f, -1.f, 0, 0 } } };

			float32x2_t vTemp = vmul_f32(vget_low_f32(V1), vrev64_f32(vget_low_f32(V2)));
			vTemp = vmul_f32(vTemp, vget_low_f32(Negate));
			vTemp = vpadd_f32(vTemp, vTemp);
			return vcombine_f32(vTemp, vTemp);
#elif defined(LUNA_PLATFORM_SSE2)
	// Swap x and y
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 1, 0, 1));
			// Perform the muls
			vResult = _mm_mul_ps(vResult, V1);
			// Splat y
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(1, 1, 1, 1));
			// Sub the values
			vResult = _mm_sub_ss(vResult, vTemp);
			// Splat the cross product
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(0, 0, 0, 0));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2LengthSq
		(
			vec_float_p1_t V
		)
		{
			return Vector2Dot(V, V);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2ReciprocalLengthEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Dot2
			float32x2_t vTemp = vmul_f32(VL, VL);
			vTemp = vpadd_f32(vTemp, vTemp);
			// Reciprocal sqrt (estimate)
			vTemp = vrsqrte_f32(vTemp);
			return vcombine_f32(vTemp, vTemp);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x3f);
			return _mm_rsqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vec_float_t vTemp = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_rsqrt_ss(vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has y splatted
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			// x+y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = _mm_rsqrt_ss(vLengthSq);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2ReciprocalLength
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Dot2
			float32x2_t vTemp = vmul_f32(VL, VL);
			vTemp = vpadd_f32(vTemp, vTemp);
			// Reciprocal sqrt
			float32x2_t  S0 = vrsqrte_f32(vTemp);
			float32x2_t  P0 = vmul_f32(vTemp, S0);
			float32x2_t  R0 = vrsqrts_f32(P0, S0);
			float32x2_t  S1 = vmul_f32(S0, R0);
			float32x2_t  P1 = vmul_f32(vTemp, S1);
			float32x2_t  R1 = vrsqrts_f32(P1, S1);
			float32x2_t Result = vmul_f32(S1, R1);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x3f);
			vec_float_t vLengthSq = _mm_sqrt_ps(vTemp);
			return _mm_div_ps(g_One, vLengthSq);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vec_float_t vTemp = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ss(vTemp);
			vLengthSq = _mm_div_ss(g_One, vLengthSq);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has y splatted
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			// x+y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = _mm_sqrt_ss(vLengthSq);
			vLengthSq = _mm_div_ss(g_One, vLengthSq);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2LengthEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Dot2
			float32x2_t vTemp = vmul_f32(VL, VL);
			vTemp = vpadd_f32(vTemp, vTemp);
			const float32x2_t zero = vdup_n_f32(0);
			uint32x2_t VEqualsZero = vceq_f32(vTemp, zero);
			// Sqrt (estimate)
			float32x2_t Result = vrsqrte_f32(vTemp);
			Result = vmul_f32(vTemp, Result);
			Result = vbsl_f32(VEqualsZero, zero, Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x3f);
			return _mm_sqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vec_float_t vTemp = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ss(vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has y splatted
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			// x+y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = _mm_sqrt_ss(vLengthSq);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Length
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Dot2
			float32x2_t vTemp = vmul_f32(VL, VL);
			vTemp = vpadd_f32(vTemp, vTemp);
			const float32x2_t zero = vdup_n_f32(0);
			uint32x2_t VEqualsZero = vceq_f32(vTemp, zero);
			// Sqrt
			float32x2_t S0 = vrsqrte_f32(vTemp);
			float32x2_t P0 = vmul_f32(vTemp, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(vTemp, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			float32x2_t Result = vmul_f32(S1, R1);
			Result = vmul_f32(vTemp, Result);
			Result = vbsl_f32(VEqualsZero, zero, Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x3f);
			return _mm_sqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vec_float_t vTemp = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ss(vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has y splatted
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			// x+y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------
		// Vector2NormalizeEst uses a reciprocal estimate and
		// returns QNaN on zero and infinite vectors.

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2NormalizeEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Dot2
			float32x2_t vTemp = vmul_f32(VL, VL);
			vTemp = vpadd_f32(vTemp, vTemp);
			// Reciprocal sqrt (estimate)
			vTemp = vrsqrte_f32(vTemp);
			// normalize
			float32x2_t Result = vmul_f32(VL, vTemp);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x3f);
			vec_float_t vResult = _mm_rsqrt_ps(vTemp);
			return _mm_mul_ps(vResult, V);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_rsqrt_ss(vLengthSq);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			vLengthSq = _mm_mul_ps(vLengthSq, V);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has y splatted
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			// x+y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = _mm_rsqrt_ss(vLengthSq);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			vLengthSq = _mm_mul_ps(vLengthSq, V);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Normalize
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			// Dot2
			float32x2_t vTemp = vmul_f32(VL, VL);
			vTemp = vpadd_f32(vTemp, vTemp);
			uint32x2_t VEqualsZero = vceq_f32(vTemp, vdup_n_f32(0));
			uint32x2_t VEqualsInf = vceq_f32(vTemp, vget_low_f32(g_Infinity));
			// Reciprocal sqrt (2 iterations of Newton-Raphson)
			float32x2_t S0 = vrsqrte_f32(vTemp);
			float32x2_t P0 = vmul_f32(vTemp, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(vTemp, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			vTemp = vmul_f32(S1, R1);
			// normalize
			float32x2_t Result = vmul_f32(VL, vTemp);
			Result = vbsl_f32(VEqualsZero, vdup_n_f32(0), Result);
			Result = vbsl_f32(VEqualsInf, vget_low_f32(g_QNaN), Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vLengthSq = _mm_dp_ps(V, V, 0x3f);
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Reciprocal mul to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE3)
			// Perform the dot product on x and y only
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_moveldup_ps(vLengthSq);
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Reciprocal mul to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x and y only
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Reciprocal mul to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2ClampLength
		(
			vec_float_p1_t V,
			f32    LengthMin,
			f32    LengthMax
		)
		{
			vec_float_t ClampMax = vector_replicate(LengthMax);
			vec_float_t ClampMin = vector_replicate(LengthMin);
			return Vector2ClampLengthV(V, ClampMin, ClampMax);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2ClampLengthV
		(
			vec_float_p1_t V,
			vec_float_p1_t LengthMin,
			vec_float_p1_t LengthMax
		)
		{
			luassert((VectorGetY(LengthMin) == VectorGetX(LengthMin)));
			luassert((VectorGetY(LengthMax) == VectorGetX(LengthMax)));
			luassert(Vector2GreaterOrEqual(LengthMin, g_Zero));
			luassert(Vector2GreaterOrEqual(LengthMax, g_Zero));
			luassert(Vector2GreaterOrEqual(LengthMax, LengthMin));

			vec_float_t LengthSq = Vector2LengthSq(V);

			const vec_float_t Zero = vector_zero();

			vec_float_t RcpLength = VectorReciprocalSqrt(LengthSq);

			vec_float_t InfiniteLength = VectorEqualInt(LengthSq, g_Infinity.v);
			vec_float_t ZeroLength = VectorEqual(LengthSq, Zero);

			vec_float_t length = VectorMultiply(LengthSq, RcpLength);

			vec_float_t Normal = VectorMultiply(V, RcpLength);

			vec_float_t Select = VectorEqualInt(InfiniteLength, ZeroLength);
			length = VectorSelect(LengthSq, length, Select);
			Normal = VectorSelect(LengthSq, Normal, Select);

			vec_float_t ControlMax = VectorGreater(length, LengthMax);
			vec_float_t ControlMin = VectorLess(length, LengthMin);

			vec_float_t ClampLength = VectorSelect(length, LengthMax, ControlMax);
			ClampLength = VectorSelect(ClampLength, LengthMin, ControlMin);

			vec_float_t Result = VectorMultiply(Normal, ClampLength);

			// Preserve the original vector (with no precision loss) if the length falls within the given range
			vec_float_t Control = VectorEqualInt(ControlMax, ControlMin);
			Result = VectorSelect(Result, V, Control);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Reflect
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal
		)
		{
			// Result = Incident - (2 * dot(Incident, Normal)) * Normal

			vec_float_t Result;
			Result = Vector2Dot(Incident, Normal);
			Result = VectorAdd(Result, Result);
			Result = VectorNegativeMultiplySubtract(Result, Normal, Incident);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Refract
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal,
			f32    RefractionIndex
		)
		{
			vec_float_t Index = vector_replicate(RefractionIndex);
			return Vector2RefractV(Incident, Normal, Index);
		}

		//------------------------------------------------------------------------------

		// Return the refraction of a 2D vector
		inline vec_float_t LUNA_SIMD_CALLCONV Vector2RefractV
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal,
			vec_float_p1_t RefractionIndex
		)
		{
			// Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
			// sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))

#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t IL = vget_low_f32(Incident);
			float32x2_t NL = vget_low_f32(Normal);
			float32x2_t RIL = vget_low_f32(RefractionIndex);
			// Get the 2D dot product of Incident-Normal
			float32x2_t vTemp = vmul_f32(IL, NL);
			float32x2_t IDotN = vpadd_f32(vTemp, vTemp);
			// vTemp = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
			vTemp = vmls_f32(vget_low_f32(g_One), IDotN, IDotN);
			vTemp = vmul_f32(vTemp, RIL);
			vTemp = vmls_f32(vget_low_f32(g_One), vTemp, RIL);
			// If any terms are <=0, sqrt() will fail, punt to zero
			uint32x2_t vMask = vcgt_f32(vTemp, vget_low_f32(g_Zero));
			// Sqrt(vTemp)
			float32x2_t S0 = vrsqrte_f32(vTemp);
			float32x2_t P0 = vmul_f32(vTemp, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(vTemp, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			float32x2_t S2 = vmul_f32(S1, R1);
			vTemp = vmul_f32(vTemp, S2);
			// R = RefractionIndex * IDotN + sqrt(R)
			vTemp = vmla_f32(vTemp, RIL, IDotN);
			// Result = RefractionIndex * Incident - Normal * R
			float32x2_t vResult = vmul_f32(RIL, IL);
			vResult = vmls_f32(vResult, vTemp, NL);
			vResult = vand_u32(vResult, vMask);
			return vcombine_f32(vResult, vResult);
#elif defined(LUNA_PLATFORM_SSE2)
	// Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
	// sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))
	// Get the 2D dot product of Incident-Normal
			vec_float_t IDotN = Vector2Dot(Incident, Normal);
			// vTemp = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
			vec_float_t vTemp = _mm_mul_ps(IDotN, IDotN);
			vTemp = _mm_sub_ps(g_One, vTemp);
			vTemp = _mm_mul_ps(vTemp, RefractionIndex);
			vTemp = _mm_mul_ps(vTemp, RefractionIndex);
			vTemp = _mm_sub_ps(g_One, vTemp);
			// If any terms are <=0, sqrt() will fail, punt to zero
			vec_float_t vMask = _mm_cmpgt_ps(vTemp, g_Zero);
			// R = RefractionIndex * IDotN + sqrt(R)
			vTemp = _mm_sqrt_ps(vTemp);
			vec_float_t vResult = _mm_mul_ps(RefractionIndex, IDotN);
			vTemp = _mm_add_ps(vTemp, vResult);
			// Result = RefractionIndex * Incident - Normal * R
			vResult = _mm_mul_ps(RefractionIndex, Incident);
			vTemp = _mm_mul_ps(vTemp, Normal);
			vResult = _mm_sub_ps(vResult, vTemp);
			vResult = _mm_and_ps(vResult, vMask);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Orthogonal
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Negate = { { { -1.f, 1.f, 0, 0 } } };
			const float32x2_t zero = vdup_n_f32(0);

			float32x2_t VL = vget_low_f32(V);
			float32x2_t Result = vmul_f32(vrev64_f32(VL), vget_low_f32(Negate));
			return vcombine_f32(Result, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 2, 0, 1));
			vResult = _mm_mul_ps(vResult, g_NegateX);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2AngleBetweenNormalsEst
		(
			vec_float_p1_t N1,
			vec_float_p1_t N2
		)
		{
			vec_float_t Result = Vector2Dot(N1, N2);
			Result = VectorClamp(Result, g_NegativeOne.v, g_One.v);
			Result = VectorACosEst(Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2AngleBetweenNormals
		(
			vec_float_p1_t N1,
			vec_float_p1_t N2
		)
		{
			vec_float_t Result = Vector2Dot(N1, N2);
			Result = VectorClamp(Result, g_NegativeOne, g_One);
			Result = VectorACos(Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2AngleBetweenVectors
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			vec_float_t L1 = Vector2ReciprocalLength(V1);
			vec_float_t L2 = Vector2ReciprocalLength(V2);

			vec_float_t dot = Vector2Dot(V1, V2);

			L1 = VectorMultiply(L1, L2);

			vec_float_t CosAngle = VectorMultiply(dot, L1);
			CosAngle = VectorClamp(CosAngle, g_NegativeOne.v, g_One.v);

			return VectorACos(CosAngle);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2LinePointDistance
		(
			vec_float_p1_t LinePoint1,
			vec_float_p1_t LinePoint2,
			vec_float_p1_t Point
		)
		{
			// Given a vector PointVector from LinePoint1 to Point and a vector
			// LineVector from LinePoint1 to LinePoint2, the scaled distance 
			// PointProjectionScale from LinePoint1 to the perpendicular projection
			// of PointVector onto the line is defined as:
			//
			//     PointProjectionScale = dot(PointVector, LineVector) / LengthSq(LineVector)

			vec_float_t PointVector = VectorSubtract(Point, LinePoint1);
			vec_float_t LineVector = VectorSubtract(LinePoint2, LinePoint1);

			vec_float_t LengthSq = Vector2LengthSq(LineVector);

			vec_float_t PointProjectionScale = Vector2Dot(PointVector, LineVector);
			PointProjectionScale = VectorDivide(PointProjectionScale, LengthSq);

			vec_float_t DistanceVector = VectorMultiply(LineVector, PointProjectionScale);
			DistanceVector = VectorSubtract(PointVector, DistanceVector);

			return Vector2Length(DistanceVector);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2IntersectLine
		(
			vec_float_p1_t Line1Point1,
			vec_float_p1_t Line1Point2,
			vec_float_p1_t Line2Point1,
			vec_float_p2_t Line2Point2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)

			vec_float_t V1 = VectorSubtract(Line1Point2, Line1Point1);
			vec_float_t V2 = VectorSubtract(Line2Point2, Line2Point1);
			vec_float_t V3 = VectorSubtract(Line1Point1, Line2Point1);

			vec_float_t C1 = Vector2Cross(V1, V2);
			vec_float_t C2 = Vector2Cross(V2, V3);

			vec_float_t Result;
			const vec_float_t Zero = vector_zero();
			if (Vector2NearEqual(C1, Zero, g_Epsilon.v))
			{
				if (Vector2NearEqual(C2, Zero, g_Epsilon.v))
				{
					// Coincident
					Result = g_Infinity.v;
				}
				else
				{
					// Parallel
					Result = g_QNaN.v;
				}
			}
			else
			{
				// Intersection point = Line1Point1 + V1 * (C2 / C1)
				vec_float_t Scale = VectorReciprocal(C1);
				Scale = VectorMultiply(C2, Scale);
				Result = VectorMultiplyAdd(V1, Scale, Line1Point1);
			}

			return Result;

#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t V1 = _mm_sub_ps(Line1Point2, Line1Point1);
			vec_float_t V2 = _mm_sub_ps(Line2Point2, Line2Point1);
			vec_float_t V3 = _mm_sub_ps(Line1Point1, Line2Point1);
			// Generate the cross products
			vec_float_t C1 = Vector2Cross(V1, V2);
			vec_float_t C2 = Vector2Cross(V2, V3);
			// If C1 is not close to epsilon, use the calculated value
			vec_float_t vResultMask = _mm_setzero_ps();
			vResultMask = _mm_sub_ps(vResultMask, C1);
			vResultMask = _mm_max_ps(vResultMask, C1);
			// 0xFFFFFFFF if the calculated value is to be used
			vResultMask = _mm_cmpgt_ps(vResultMask, g_Epsilon);
			// If C1 is close to epsilon, which fail type is it? INFINITY or NAN?
			vec_float_t vFailMask = _mm_setzero_ps();
			vFailMask = _mm_sub_ps(vFailMask, C2);
			vFailMask = _mm_max_ps(vFailMask, C2);
			vFailMask = _mm_cmple_ps(vFailMask, g_Epsilon);
			vec_float_t vFail = _mm_and_ps(vFailMask, g_Infinity);
			vFailMask = _mm_andnot_ps(vFailMask, g_QNaN);
			// vFail is NAN or INF
			vFail = _mm_or_ps(vFail, vFailMask);
			// Intersection point = Line1Point1 + V1 * (C2 / C1)
			vec_float_t vResult = _mm_div_ps(C2, C1);
			vResult = _mm_mul_ps(vResult, V1);
			vResult = _mm_add_ps(vResult, Line1Point1);
			// Use result, or failure value
			vResult = _mm_and_ps(vResult, vResultMask);
			vResultMask = _mm_andnot_ps(vResultMask, vFail);
			vResult = _mm_or_ps(vResult, vResultMask);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2Transform
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			float32x4_t Result = vmlaq_lane_f32(M.r[3], M.r[1], VL, 1); // Y
			return vmlaq_lane_f32(Result, M.r[0], VL, 0); // X
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
			vResult = _mm_mul_ps(vResult, M.r[0]);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vTemp = _mm_mul_ps(vTemp, M.r[1]);
			vResult = _mm_add_ps(vResult, vTemp);
			vResult = _mm_add_ps(vResult, M.r[3]);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------


		inline f32* LUNA_SIMD_CALLCONV Vector2TransformStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 8);

			luassert(OutputStride >= 16);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 8) && (OutputStride == 16))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x2_t V = vld2q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 8 * 4;

						float32x2_t r3 = vget_low_f32(row3);
						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Ax+M
						vec_float_t vResult1 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Bx+N

						__prefetch(pInputVector);

						r3 = vget_high_f32(row3);
						r = vget_high_f32(row0);
						vec_float_t vResult2 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Cx+O
						vec_float_t vResult3 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Dx+P

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy+N

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(row1);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[1], r, 0); // Cx+Gy+O
						vResult3 = vmlaq_lane_f32(vResult3, V.val[1], r, 1); // Dx+Hy+P

						__prefetch(pInputVector + (g_cache_line_size * 3));

						float32x4x4_t R;
						R.val[0] = vResult0;
						R.val[1] = vResult1;
						R.val[2] = vResult2;
						R.val[3] = vResult3;

						vst4q_f32(reinterpret_cast<float*>(pOutputVector), R);
						pOutputVector += 16 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				float32x2_t V = vld1_f32(reinterpret_cast<const float*>(pInputVector));
				pInputVector += InputStride;

				vec_float_t vResult = vmlaq_lane_f32(row3, row0, V, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, V, 1); // Y

				vst1q_f32(reinterpret_cast<float*>(pOutputVector), vResult);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize two = VectorCount >> 1;
			if (two > 0)
			{
				if (InputStride == 8)
				{
					if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF) && !(OutputStride & 0xF))
					{
						// Packed input, aligned output
						for (usize j = 0; j < two; ++j)
						{
							vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							pInputVector += 8 * 2;

							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Y, row1);
							vec_float_t vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);

							LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
							X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

							vTemp = _mm_mul_ps(Y, row1);
							vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);

							LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 2;
						}
					}
					else
					{
						// Packed input, unaligned output
						for (usize j = 0; j < two; ++j)
						{
							vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							pInputVector += 8 * 2;

							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Y, row1);
							vec_float_t vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);

							_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
							X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

							vTemp = _mm_mul_ps(Y, row1);
							vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);

							_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 2;
						}
					}
				}
			}

			if (!(reinterpret_cast<uintptr_t>(pInputVector) & 0xF) && !(InputStride & 0xF))
			{
				if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF) && !(OutputStride & 0xF))
				{
					// Aligned input, aligned output
					for (; i < VectorCount; i++)
					{
						vec_float_t V = _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(pInputVector)));
						pInputVector += InputStride;

						vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
						vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

						vec_float_t vTemp = _mm_mul_ps(Y, row1);
						vec_float_t vTemp2 = _mm_mul_ps(X, row0);
						vTemp = _mm_add_ps(vTemp, row3);
						vTemp = _mm_add_ps(vTemp, vTemp2);

						LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
						pOutputVector += OutputStride;
					}
				}
				else
				{
					// Aligned input, unaligned output
					for (; i < VectorCount; i++)
					{
						vec_float_t V = _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(pInputVector)));
						pInputVector += InputStride;

						vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
						vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

						vec_float_t vTemp = _mm_mul_ps(Y, row1);
						vec_float_t vTemp2 = _mm_mul_ps(X, row0);
						vTemp = _mm_add_ps(vTemp, row3);
						vTemp = _mm_add_ps(vTemp, vTemp2);

						_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
						pOutputVector += OutputStride;
					}
				}
			}
			else
			{
				// Unaligned input
				for (; i < VectorCount; i++)
				{
					__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pInputVector));
					__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pInputVector + 4));
					pInputVector += InputStride;

					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(y, _MM_SHUFFLE(0, 0, 0, 0));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(x, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Y, row1);
					vec_float_t vTemp2 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, row3);
					vTemp = _mm_add_ps(vTemp, vTemp2);

					_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
					pOutputVector += OutputStride;
				}
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2TransformCoord
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
			vec_float_t Y = vector_splat_y(V);
			vec_float_t X = vector_splat_x(V);

			vec_float_t Result = VectorMultiplyAdd(Y, M.r[1], M.r[3]);
			Result = VectorMultiplyAdd(X, M.r[0], Result);

			vec_float_t W = vector_splat_w(Result);
			return VectorDivide(Result, W);
		}

		//------------------------------------------------------------------------------


		inline f32* LUNA_SIMD_CALLCONV Vector2TransformCoordStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 8);

			luassert(OutputStride >= 8);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 8) && (OutputStride == 8))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x2_t V = vld2q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 8 * 4;

						float32x2_t r3 = vget_low_f32(row3);
						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Ax+M
						vec_float_t vResult1 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Bx+N

						__prefetch(pInputVector);

						r3 = vget_high_f32(row3);
						r = vget_high_f32(row0);
						vec_float_t W = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Dx+P

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy+N

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(row1);
						W = vmlaq_lane_f32(W, V.val[1], r, 1); // Dx+Hy+P

						__prefetch(pInputVector + (g_cache_line_size * 3));

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
						V.val[0] = vdivq_f32(vResult0, W);
						V.val[1] = vdivq_f32(vResult1, W);
#else
						// 2 iterations of Newton-Raphson refinement of reciprocal
						float32x4_t Reciprocal = vrecpeq_f32(W);
						float32x4_t S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);
						S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);

						V.val[0] = vmulq_f32(vResult0, Reciprocal);
						V.val[1] = vmulq_f32(vResult1, Reciprocal);
#endif

						vst2q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 8 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				float32x2_t V = vld1_f32(reinterpret_cast<const float*>(pInputVector));
				pInputVector += InputStride;

				vec_float_t vResult = vmlaq_lane_f32(row3, row0, V, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, V, 1); // Y

				V = vget_high_f32(vResult);
				float32x2_t W = vdup_lane_f32(V, 1);

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
				V = vget_low_f32(vResult);
				V = vdiv_f32(V, W);
#else
				// 2 iterations of Newton-Raphson refinement of reciprocal for W
				float32x2_t Reciprocal = vrecpe_f32(W);
				float32x2_t S = vrecps_f32(Reciprocal, W);
				Reciprocal = vmul_f32(S, Reciprocal);
				S = vrecps_f32(Reciprocal, W);
				Reciprocal = vmul_f32(S, Reciprocal);

				V = vget_low_f32(vResult);
				V = vmul_f32(V, Reciprocal);
#endif

				vst1_f32(reinterpret_cast<float*>(pOutputVector), V);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize two = VectorCount >> 1;
			if (two > 0)
			{
				if (InputStride == 8)
				{
					if (OutputStride == 8)
					{
						if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF))
						{
							// Packed input, aligned & packed output
							for (usize j = 0; j < two; ++j)
							{
								vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								pInputVector += 8 * 2;

								// Result 1
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Y, row1);
								vec_float_t vTemp2 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								vec_float_t V1 = _mm_div_ps(vTemp, W);

								// Result 2
								Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
								X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

								vTemp = _mm_mul_ps(Y, row1);
								vTemp2 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								vec_float_t V2 = _mm_div_ps(vTemp, W);

								vTemp = _mm_movelh_ps(V1, V2);

								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
								pOutputVector += 8 * 2;

								i += 2;
							}
						}
						else
						{
							// Packed input, unaligned & packed output
							for (usize j = 0; j < two; ++j)
							{
								vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								pInputVector += 8 * 2;

								// Result 1
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Y, row1);
								vec_float_t vTemp2 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								vec_float_t V1 = _mm_div_ps(vTemp, W);

								// Result 2
								Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
								X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

								vTemp = _mm_mul_ps(Y, row1);
								vTemp2 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								vec_float_t V2 = _mm_div_ps(vTemp, W);

								vTemp = _mm_movelh_ps(V1, V2);

								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
								pOutputVector += 8 * 2;

								i += 2;
							}
						}
					}
					else
					{
						// Packed input, unpacked output
						for (usize j = 0; j < two; ++j)
						{
							vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							pInputVector += 8 * 2;

							// Result 1
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Y, row1);
							vec_float_t vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);

							vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

							vTemp = _mm_div_ps(vTemp, W);
							vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

							_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
							_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
							pOutputVector += OutputStride;

							// Result 2
							Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
							X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

							vTemp = _mm_mul_ps(Y, row1);
							vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

							vTemp = _mm_div_ps(vTemp, W);
							vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

							_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
							_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
							pOutputVector += OutputStride;

							i += 2;
						}
					}
				}
			}

			if (!(reinterpret_cast<uintptr_t>(pInputVector) & 0xF) && !(InputStride & 0xF))
			{
				// Aligned input
				for (; i < VectorCount; i++)
				{
					vec_float_t V = _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(pInputVector)));
					pInputVector += InputStride;

					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Y, row1);
					vec_float_t vTemp2 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, row3);
					vTemp = _mm_add_ps(vTemp, vTemp2);

					vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

					vTemp = _mm_div_ps(vTemp, W);
					vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

					_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
					_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
					pOutputVector += OutputStride;
				}
			}
			else
			{
				// Unaligned input
				for (; i < VectorCount; i++)
				{
					__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pInputVector));
					__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pInputVector + 4));
					pInputVector += InputStride;

					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(y, _MM_SHUFFLE(0, 0, 0, 0));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(x, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Y, row1);
					vec_float_t vTemp2 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, row3);
					vTemp = _mm_add_ps(vTemp, vTemp2);

					vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

					vTemp = _mm_div_ps(vTemp, W);
					vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

					_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
					_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
					pOutputVector += OutputStride;
				}
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector2TransformNormal
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			float32x4_t Result = vmulq_lane_f32(M.r[1], VL, 1); // Y
			return vmlaq_lane_f32(Result, M.r[0], VL, 0); // X
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
			vResult = _mm_mul_ps(vResult, M.r[0]);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vTemp = _mm_mul_ps(vTemp, M.r[1]);
			vResult = _mm_add_ps(vResult, vTemp);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------


		inline f32* LUNA_SIMD_CALLCONV Vector2TransformNormalStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 8);

			luassert(OutputStride >= 8);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 8) && (OutputStride == 8))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x2_t V = vld2q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 8 * 4;

						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmulq_lane_f32(V.val[0], r, 0); // Ax
						vec_float_t vResult1 = vmulq_lane_f32(V.val[0], r, 1); // Bx

						__prefetch(pInputVector);
						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy

						__prefetch(pInputVector + (g_cache_line_size * 2));
						__prefetch(pInputVector + (g_cache_line_size * 3));

						V.val[0] = vResult0;
						V.val[1] = vResult1;

						vst2q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 8 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				float32x2_t V = vld1_f32(reinterpret_cast<const float*>(pInputVector));
				pInputVector += InputStride;

				vec_float_t vResult = vmulq_lane_f32(row0, V, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, V, 1); // Y

				V = vget_low_f32(vResult);
				vst1_f32(reinterpret_cast<float*>(pOutputVector), V);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];

			usize i = 0;
			usize two = VectorCount >> 1;
			if (two > 0)
			{
				if (InputStride == 8)
				{
					if (OutputStride == 8)
					{
						if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF))
						{
							// Packed input, aligned & packed output
							for (usize j = 0; j < two; ++j)
							{
								vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								pInputVector += 8 * 2;

								// Result 1
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Y, row1);
								vec_float_t vTemp2 = _mm_mul_ps(X, row0);
								vec_float_t V1 = _mm_add_ps(vTemp, vTemp2);

								// Result 2
								Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
								X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

								vTemp = _mm_mul_ps(Y, row1);
								vTemp2 = _mm_mul_ps(X, row0);
								vec_float_t V2 = _mm_add_ps(vTemp, vTemp2);

								vTemp = _mm_movelh_ps(V1, V2);

								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
								pOutputVector += 8 * 2;

								i += 2;
							}
						}
						else
						{
							// Packed input, unaligned & packed output
							for (usize j = 0; j < two; ++j)
							{
								vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								pInputVector += 8 * 2;

								// Result 1
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Y, row1);
								vec_float_t vTemp2 = _mm_mul_ps(X, row0);
								vec_float_t V1 = _mm_add_ps(vTemp, vTemp2);

								// Result 2
								Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
								X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

								vTemp = _mm_mul_ps(Y, row1);
								vTemp2 = _mm_mul_ps(X, row0);
								vec_float_t V2 = _mm_add_ps(vTemp, vTemp2);

								vTemp = _mm_movelh_ps(V1, V2);

								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
								pOutputVector += 8 * 2;

								i += 2;
							}
						}
					}
					else
					{
						// Packed input, unpacked output
						for (usize j = 0; j < two; ++j)
						{
							vec_float_t V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							pInputVector += 8 * 2;

							// Result 1
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Y, row1);
							vec_float_t vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

							_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
							_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
							pOutputVector += OutputStride;

							// Result 2
							Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
							X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));

							vTemp = _mm_mul_ps(Y, row1);
							vTemp2 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

							_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
							_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
							pOutputVector += OutputStride;

							i += 2;
						}
					}
				}
			}

			if (!(reinterpret_cast<uintptr_t>(pInputVector) & 0xF) && !(InputStride & 0xF))
			{
				// Aligned input
				for (; i < VectorCount; i++)
				{
					vec_float_t V = _mm_castsi128_ps(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(pInputVector)));
					pInputVector += InputStride;

					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Y, row1);
					vec_float_t vTemp2 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, vTemp2);
					vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

					_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
					_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
					pOutputVector += OutputStride;
				}
			}
			else
			{
				// Unaligned input
				for (; i < VectorCount; i++)
				{
					__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pInputVector));
					__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pInputVector + 4));
					pInputVector += InputStride;

					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(y, _MM_SHUFFLE(0, 0, 0, 0));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(x, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Y, row1);
					vec_float_t vTemp2 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, vTemp2);
					vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));

					_mm_store_ss(reinterpret_cast<float*>(pOutputVector), vTemp);
					_mm_store_ss(reinterpret_cast<float*>(pOutputVector + 4), vTemp2);
					pOutputVector += OutputStride;
				}
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

		/****************************************************************************
		 *
		 * 3D Vector
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3Equal
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 7) == 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector3EqualR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU;

			u32 CR = 0;
			if (r == 0xFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			int iTest = _mm_movemask_ps(vTemp) & 7;
			u32 CR = 0;
			if (iTest == 7)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3EqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return (((_mm_movemask_ps(_mm_castsi128_ps(vTemp)) & 7) == 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector3EqualIntR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU;

			u32 CR = 0;
			if (r == 0xFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			int iTemp = _mm_movemask_ps(_mm_castsi128_ps(vTemp)) & 7;
			u32 CR = 0;
			if (iTemp == 7)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTemp)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3NearEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t Epsilon
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t vDelta = vsubq_f32(V1, V2);
			uint32x4_t vResult = vacleq_f32(vDelta, Epsilon);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Get the difference
			vec_float_t vDelta = _mm_sub_ps(V1, V2);
			// Get the absolute value of the difference
			vec_float_t vTemp = _mm_setzero_ps();
			vTemp = _mm_sub_ps(vTemp, vDelta);
			vTemp = _mm_max_ps(vTemp, vDelta);
			vTemp = _mm_cmple_ps(vTemp, Epsilon);
			// w is don't care
			return (((_mm_movemask_ps(vTemp) & 7) == 0x7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3NotEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) != 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 7) != 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3NotEqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) != 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return (((_mm_movemask_ps(_mm_castsi128_ps(vTemp)) & 7) != 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3Greater
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgtq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 7) == 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector3GreaterR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgtq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU;

			u32 CR = 0;
			if (r == 0xFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			u32 CR = 0;
			int iTest = _mm_movemask_ps(vTemp) & 7;
			if (iTest == 7)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3GreaterOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgeq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 7) == 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector3GreaterOrEqualR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgeq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU;

			u32 CR = 0;
			if (r == 0xFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			u32 CR = 0;
			int iTest = _mm_movemask_ps(vTemp) & 7;
			if (iTest == 7)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3Less
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcltq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmplt_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 7) == 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3LessOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcleq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmple_ps(V1, V2);
			return (((_mm_movemask_ps(vTemp) & 7) == 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3InBounds
		(
			vec_float_p1_t V,
			vec_float_p1_t Bounds
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test if less than or equal
			uint32x4_t ivTemp1 = vcleq_f32(V, Bounds);
			// Negate the bounds
			float32x4_t vTemp2 = vnegq_f32(Bounds);
			// Test if greater or equal (Reversed)
			uint32x4_t ivTemp2 = vcleq_f32(vTemp2, V);
			// Blend answers
			ivTemp1 = vandq_u32(ivTemp1, ivTemp2);
			// in bounds?
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(ivTemp1), vget_high_u8(ivTemp1));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) == 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test if less than or equal
			vec_float_t vTemp1 = _mm_cmple_ps(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = _mm_mul_ps(Bounds, g_NegativeOne);
			// Test if greater or equal (Reversed)
			vTemp2 = _mm_cmple_ps(vTemp2, V);
			// Blend answers
			vTemp1 = _mm_and_ps(vTemp1, vTemp2);
			// x,y and z in bounds? (w is don't care)
			return (((_mm_movemask_ps(vTemp1) & 0x7) == 0x7) != 0);
#else
			return comparison_all_in_bounds(Vector3InBoundsR(V, Bounds));
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3IsNaN
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test against itself. NaN is always not equal
			uint32x4_t vTempNan = vceqq_f32(V, V);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTempNan), vget_high_u8(vTempNan));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			// If x or y or z are NaN, the mask is zero
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) != 0xFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test against itself. NaN is always not equal
			vec_float_t vTempNan = _mm_cmpneq_ps(V, V);
			// If x or y or z are NaN, the mask is non-zero
			return ((_mm_movemask_ps(vTempNan) & 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector3IsInfinite
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Mask off the sign bit
			uint32x4_t vTempInf = vandq_u32(V, g_AbsMask);
			// Compare to infinity
			vTempInf = vceqq_f32(vTempInf, g_Infinity);
			// If any are infinity, the signs are true.
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTempInf), vget_high_u8(vTempInf));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return ((vget_lane_u32(vTemp.val[1], 1) & 0xFFFFFFU) != 0);
#elif defined(LUNA_PLATFORM_SSE2)
			// Mask off the sign bit
			__m128 vTemp = _mm_and_ps(V, g_AbsMask);
			// Compare to infinity
			vTemp = _mm_cmpeq_ps(vTemp, g_Infinity);
			// If x,y or z are infinity, the signs are true.
			return ((_mm_movemask_ps(vTemp) & 7) != 0);
#endif
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Dot
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t vTemp = vmulq_f32(V1, V2);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			return vcombine_f32(v1, v1);
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_dp_ps(V1, V2, 0x7f);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vTemp = _mm_mul_ps(V1, V2);
			vTemp = _mm_and_ps(vTemp, g_Mask3);
			vTemp = _mm_hadd_ps(vTemp, vTemp);
			return _mm_hadd_ps(vTemp, vTemp);
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product
			vec_float_t vDot = _mm_mul_ps(V1, V2);
			// x=dot.vector4_f32[1], y=dot.vector4_f32[2]
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vDot, _MM_SHUFFLE(2, 1, 2, 1));
			// Result.vector4_f32[0] = x+y
			vDot = _mm_add_ss(vDot, vTemp);
			// x=dot.vector4_f32[2]
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			// Result.vector4_f32[0] = (x+y)+z
			vDot = _mm_add_ss(vDot, vTemp);
			// Splat x
			return LUNA_SIMD_PERMUTE_PS(vDot, _MM_SHUFFLE(0, 0, 0, 0));
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Cross
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			// [ V1.y*V2.z - V1.z*V2.y, V1.z*V2.x - V1.x*V2.z, V1.x*V2.y - V1.y*V2.x ]

#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t v1xy = vget_low_f32(V1);
			float32x2_t v2xy = vget_low_f32(V2);

			float32x2_t v1yx = vrev64_f32(v1xy);
			float32x2_t v2yx = vrev64_f32(v2xy);

			float32x2_t v1zz = vdup_lane_f32(vget_high_f32(V1), 0);
			float32x2_t v2zz = vdup_lane_f32(vget_high_f32(V2), 0);

			vec_float_t vResult = vmulq_f32(vcombine_f32(v1yx, v1xy), vcombine_f32(v2zz, v2yx));
			vResult = vmlsq_f32(vResult, vcombine_f32(v1zz, v1yx), vcombine_f32(v2yx, v2xy));
			vResult = veorq_u32(vResult, g_FlipY);
			return vandq_u32(vResult, g_Mask3);
#elif defined(LUNA_PLATFORM_SSE2)
	// y1,z1,x1,w1
			vec_float_t vTemp1 = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(3, 0, 2, 1));
			// z2,x2,y2,w2
			vec_float_t vTemp2 = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(3, 1, 0, 2));
			// Perform the left operation
			vec_float_t vResult = _mm_mul_ps(vTemp1, vTemp2);
			// z1,x1,y1,w1
			vTemp1 = LUNA_SIMD_PERMUTE_PS(vTemp1, _MM_SHUFFLE(3, 0, 2, 1));
			// y2,z2,x2,w2
			vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp2, _MM_SHUFFLE(3, 1, 0, 2));
			// Perform the right operation
			vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
			// Subract the right from left, and return answer
			vResult = _mm_sub_ps(vResult, vTemp1);
			// Set w to zero
			return _mm_and_ps(vResult, g_Mask3);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3LengthSq
		(
			vec_float_p1_t V
		)
		{
			return Vector3Dot(V, V);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3ReciprocalLengthEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot3
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			// Reciprocal sqrt (estimate)
			v2 = vrsqrte_f32(v1);
			return vcombine_f32(v2, v2);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x7f);
			return _mm_rsqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_and_ps(vLengthSq, g_Mask3);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_rsqrt_ps(vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y and z
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and y
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 2, 1, 2));
			// x+z, y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			// y,y,y,y
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			// x+z+y,??,??,??
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			// Splat the length squared
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			// Get the reciprocal
			vLengthSq = _mm_rsqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3ReciprocalLength
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot3
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			// Reciprocal sqrt
			float32x2_t  S0 = vrsqrte_f32(v1);
			float32x2_t  P0 = vmul_f32(v1, S0);
			float32x2_t  R0 = vrsqrts_f32(P0, S0);
			float32x2_t  S1 = vmul_f32(S0, R0);
			float32x2_t  P1 = vmul_f32(v1, S1);
			float32x2_t  R1 = vrsqrts_f32(P1, S1);
			float32x2_t Result = vmul_f32(S1, R1);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x7f);
			vec_float_t vLengthSq = _mm_sqrt_ps(vTemp);
			return _mm_div_ps(g_One, vLengthSq);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vDot = _mm_mul_ps(V, V);
			vDot = _mm_and_ps(vDot, g_Mask3);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_sqrt_ps(vDot);
			vDot = _mm_div_ps(g_One, vDot);
			return vDot;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product
			vec_float_t vDot = _mm_mul_ps(V, V);
			// x=dot.y, y=dot.z
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vDot, _MM_SHUFFLE(2, 1, 2, 1));
			// Result.x = x+y
			vDot = _mm_add_ss(vDot, vTemp);
			// x=dot.z
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			// Result.x = (x+y)+z
			vDot = _mm_add_ss(vDot, vTemp);
			// Splat x
			vDot = LUNA_SIMD_PERMUTE_PS(vDot, _MM_SHUFFLE(0, 0, 0, 0));
			// Get the reciprocal
			vDot = _mm_sqrt_ps(vDot);
			// Get the reciprocal
			vDot = _mm_div_ps(g_One, vDot);
			return vDot;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3LengthEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot3
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			const float32x2_t zero = vdup_n_f32(0);
			uint32x2_t VEqualsZero = vceq_f32(v1, zero);
			// Sqrt (estimate)
			float32x2_t Result = vrsqrte_f32(v1);
			Result = vmul_f32(v1, Result);
			Result = vbsl_f32(VEqualsZero, zero, Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x7f);
			return _mm_sqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_and_ps(vLengthSq, g_Mask3);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y and z
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and y
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 2, 1, 2));
			// x+z, y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			// y,y,y,y
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			// x+z+y,??,??,??
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			// Splat the length squared
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			// Get the length
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Length
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot3
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			const float32x2_t zero = vdup_n_f32(0);
			uint32x2_t VEqualsZero = vceq_f32(v1, zero);
			// Sqrt
			float32x2_t S0 = vrsqrte_f32(v1);
			float32x2_t P0 = vmul_f32(v1, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(v1, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			float32x2_t Result = vmul_f32(S1, R1);
			Result = vmul_f32(v1, Result);
			Result = vbsl_f32(VEqualsZero, zero, Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x7f);
			return _mm_sqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_and_ps(vLengthSq, g_Mask3);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y and z
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and y
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 2, 1, 2));
			// x+z, y
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			// y,y,y,y
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			// x+z+y,??,??,??
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			// Splat the length squared
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			// Get the length
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------
		// Vector3NormalizeEst uses a reciprocal estimate and
		// returns QNaN on zero and infinite vectors.

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3NormalizeEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot3
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			// Reciprocal sqrt (estimate)
			v2 = vrsqrte_f32(v1);
			// normalize
			return vmulq_f32(V, vcombine_f32(v2, v2));
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0x7f);
			vec_float_t vResult = _mm_rsqrt_ps(vTemp);
			return _mm_mul_ps(vResult, V);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vDot = _mm_mul_ps(V, V);
			vDot = _mm_and_ps(vDot, g_Mask3);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_rsqrt_ps(vDot);
			vDot = _mm_mul_ps(vDot, V);
			return vDot;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product
			vec_float_t vDot = _mm_mul_ps(V, V);
			// x=dot.y, y=dot.z
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vDot, _MM_SHUFFLE(2, 1, 2, 1));
			// Result.x = x+y
			vDot = _mm_add_ss(vDot, vTemp);
			// x=dot.z
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			// Result.x = (x+y)+z
			vDot = _mm_add_ss(vDot, vTemp);
			// Splat x
			vDot = LUNA_SIMD_PERMUTE_PS(vDot, _MM_SHUFFLE(0, 0, 0, 0));
			// Get the reciprocal
			vDot = _mm_rsqrt_ps(vDot);
			// Perform the normalization
			vDot = _mm_mul_ps(vDot, V);
			return vDot;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Normalize
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot3
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vpadd_f32(v1, v1);
			v2 = vdup_lane_f32(v2, 0);
			v1 = vadd_f32(v1, v2);
			uint32x2_t VEqualsZero = vceq_f32(v1, vdup_n_f32(0));
			uint32x2_t VEqualsInf = vceq_f32(v1, vget_low_f32(g_Infinity));
			// Reciprocal sqrt (2 iterations of Newton-Raphson)
			float32x2_t S0 = vrsqrte_f32(v1);
			float32x2_t P0 = vmul_f32(v1, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(v1, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			v2 = vmul_f32(S1, R1);
			// normalize
			vec_float_t vResult = vmulq_f32(V, vcombine_f32(v2, v2));
			vResult = vbslq_f32(vcombine_f32(VEqualsZero, VEqualsZero), vdupq_n_f32(0), vResult);
			return vbslq_f32(vcombine_f32(VEqualsInf, VEqualsInf), g_QNaN, vResult);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vLengthSq = _mm_dp_ps(V, V, 0x7f);
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Divide to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE3)
			// Perform the dot product on x,y and z only
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_and_ps(vLengthSq, g_Mask3);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Divide to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y and z only
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 1, 2, 1));
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Divide to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3ClampLength
		(
			vec_float_p1_t V,
			f32    LengthMin,
			f32    LengthMax
		)
		{
			vec_float_t ClampMax = vector_replicate(LengthMax);
			vec_float_t ClampMin = vector_replicate(LengthMin);

			return Vector3ClampLengthV(V, ClampMin, ClampMax);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3ClampLengthV
		(
			vec_float_p1_t V,
			vec_float_p1_t LengthMin,
			vec_float_p1_t LengthMax
		)
		{
			luassert((VectorGetY(LengthMin) == VectorGetX(LengthMin)) && (VectorGetZ(LengthMin) == VectorGetX(LengthMin)));
			luassert((VectorGetY(LengthMax) == VectorGetX(LengthMax)) && (VectorGetZ(LengthMax) == VectorGetX(LengthMax)));
			luassert(Vector3GreaterOrEqual(LengthMin, vector_zero()));
			luassert(Vector3GreaterOrEqual(LengthMax, vector_zero()));
			luassert(Vector3GreaterOrEqual(LengthMax, LengthMin));

			vec_float_t LengthSq = Vector3LengthSq(V);

			const vec_float_t Zero = vector_zero();

			vec_float_t RcpLength = VectorReciprocalSqrt(LengthSq);

			vec_float_t InfiniteLength = VectorEqualInt(LengthSq, g_Infinity.v);
			vec_float_t ZeroLength = VectorEqual(LengthSq, Zero);

			vec_float_t Normal = VectorMultiply(V, RcpLength);

			vec_float_t length = VectorMultiply(LengthSq, RcpLength);

			vec_float_t Select = VectorEqualInt(InfiniteLength, ZeroLength);
			length = VectorSelect(LengthSq, length, Select);
			Normal = VectorSelect(LengthSq, Normal, Select);

			vec_float_t ControlMax = VectorGreater(length, LengthMax);
			vec_float_t ControlMin = VectorLess(length, LengthMin);

			vec_float_t ClampLength = VectorSelect(length, LengthMax, ControlMax);
			ClampLength = VectorSelect(ClampLength, LengthMin, ControlMin);

			vec_float_t Result = VectorMultiply(Normal, ClampLength);

			// Preserve the original vector (with no precision loss) if the length falls within the given range
			vec_float_t Control = VectorEqualInt(ControlMax, ControlMin);
			Result = VectorSelect(Result, V, Control);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Reflect
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal
		)
		{
			// Result = Incident - (2 * dot(Incident, Normal)) * Normal

			vec_float_t Result = Vector3Dot(Incident, Normal);
			Result = VectorAdd(Result, Result);
			Result = VectorNegativeMultiplySubtract(Result, Normal, Incident);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Refract
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal,
			f32    RefractionIndex
		)
		{
			vec_float_t Index = vector_replicate(RefractionIndex);
			return Vector3RefractV(Incident, Normal, Index);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3RefractV
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal,
			vec_float_p1_t RefractionIndex
		)
		{
			// Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
			// sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t IDotN = Vector3Dot(Incident, Normal);

			// R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
			float32x4_t R = vmlsq_f32(g_One, IDotN, IDotN);
			R = vmulq_f32(R, RefractionIndex);
			R = vmlsq_f32(g_One, R, RefractionIndex);

			uint32x4_t vResult = vcleq_f32(R, g_Zero);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			if (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU)
			{
				// Total internal reflection
				vResult = g_Zero;
			}
			else
			{
				// Sqrt(R)
				float32x4_t S0 = vrsqrteq_f32(R);
				float32x4_t P0 = vmulq_f32(R, S0);
				float32x4_t R0 = vrsqrtsq_f32(P0, S0);
				float32x4_t S1 = vmulq_f32(S0, R0);
				float32x4_t P1 = vmulq_f32(R, S1);
				float32x4_t R1 = vrsqrtsq_f32(P1, S1);
				float32x4_t S2 = vmulq_f32(S1, R1);
				R = vmulq_f32(R, S2);
				// R = RefractionIndex * IDotN + sqrt(R)
				R = vmlaq_f32(R, RefractionIndex, IDotN);
				// Result = RefractionIndex * Incident - Normal * R
				vResult = vmulq_f32(RefractionIndex, Incident);
				vResult = vmlsq_f32(vResult, R, Normal);
			}
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
	// Result = RefractionIndex * Incident - Normal * (RefractionIndex * dot(Incident, Normal) + 
	// sqrt(1 - RefractionIndex * RefractionIndex * (1 - dot(Incident, Normal) * dot(Incident, Normal))))
			vec_float_t IDotN = Vector3Dot(Incident, Normal);
			// R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
			vec_float_t R = _mm_mul_ps(IDotN, IDotN);
			R = _mm_sub_ps(g_One, R);
			R = _mm_mul_ps(R, RefractionIndex);
			R = _mm_mul_ps(R, RefractionIndex);
			R = _mm_sub_ps(g_One, R);

			vec_float_t vResult = _mm_cmple_ps(R, g_Zero);
			if (_mm_movemask_ps(vResult) == 0x0f)
			{
				// Total internal reflection
				vResult = g_Zero;
			}
			else
			{
				// R = RefractionIndex * IDotN + sqrt(R)
				R = _mm_sqrt_ps(R);
				vResult = _mm_mul_ps(RefractionIndex, IDotN);
				R = _mm_add_ps(R, vResult);
				// Result = RefractionIndex * Incident - Normal * R
				vResult = _mm_mul_ps(RefractionIndex, Incident);
				R = _mm_mul_ps(R, Normal);
				vResult = _mm_sub_ps(vResult, R);
			}
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Orthogonal
		(
			vec_float_p1_t V
		)
		{
			vec_float_t Zero = vector_zero();
			vec_float_t Z = vector_splat_z(V);
			vec_float_t YZYY = VectorSwizzle<g_swizzle_y, g_swizzle_z, g_swizzle_y, g_swizzle_y>(V);

			vec_float_t NegativeV = VectorSubtract(Zero, V);

			vec_float_t ZIsNegative = VectorLess(Z, Zero);
			vec_float_t YZYYIsNegative = VectorLess(YZYY, Zero);

			vec_float_t S = VectorAdd(YZYY, Z);
			vec_float_t D = VectorSubtract(YZYY, Z);

			vec_float_t Select = VectorEqualInt(ZIsNegative, YZYYIsNegative);

			vec_float_t R0 = VectorPermute<g_permute_1x, g_permute_0x, g_permute_0x, g_permute_0x>(NegativeV, S);
			vec_float_t R1 = VectorPermute<g_permute_1x, g_permute_0x, g_permute_0x, g_permute_0x>(V, D);

			return VectorSelect(R1, R0, Select);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3AngleBetweenNormalsEst
		(
			vec_float_p1_t N1,
			vec_float_p1_t N2
		)
		{
			vec_float_t Result = Vector3Dot(N1, N2);
			Result = VectorClamp(Result, g_NegativeOne.v, g_One.v);
			Result = VectorACosEst(Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3AngleBetweenNormals
		(
			vec_float_p1_t N1,
			vec_float_p1_t N2
		)
		{
			vec_float_t Result = Vector3Dot(N1, N2);
			Result = VectorClamp(Result, g_NegativeOne.v, g_One.v);
			Result = VectorACos(Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3AngleBetweenVectors
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			vec_float_t L1 = Vector3ReciprocalLength(V1);
			vec_float_t L2 = Vector3ReciprocalLength(V2);

			vec_float_t dot = Vector3Dot(V1, V2);

			L1 = VectorMultiply(L1, L2);

			vec_float_t CosAngle = VectorMultiply(dot, L1);
			CosAngle = VectorClamp(CosAngle, g_NegativeOne.v, g_One.v);

			return VectorACos(CosAngle);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3LinePointDistance
		(
			vec_float_p1_t LinePoint1,
			vec_float_p1_t LinePoint2,
			vec_float_p1_t Point
		)
		{
			// Given a vector PointVector from LinePoint1 to Point and a vector
			// LineVector from LinePoint1 to LinePoint2, the scaled distance 
			// PointProjectionScale from LinePoint1 to the perpendicular projection
			// of PointVector onto the line is defined as:
			//
			//     PointProjectionScale = dot(PointVector, LineVector) / LengthSq(LineVector)

			vec_float_t PointVector = VectorSubtract(Point, LinePoint1);
			vec_float_t LineVector = VectorSubtract(LinePoint2, LinePoint1);

			vec_float_t LengthSq = Vector3LengthSq(LineVector);

			vec_float_t PointProjectionScale = Vector3Dot(PointVector, LineVector);
			PointProjectionScale = VectorDivide(PointProjectionScale, LengthSq);

			vec_float_t DistanceVector = VectorMultiply(LineVector, PointProjectionScale);
			DistanceVector = VectorSubtract(PointVector, DistanceVector);

			return Vector3Length(DistanceVector);
		}

		//------------------------------------------------------------------------------


		inline void LUNA_SIMD_CALLCONV Vector3ComponentsFromNormal
		(
			vec_float_t* pParallel,
			vec_float_t* pPerpendicular,
			vec_float_p1_t  V,
			vec_float_p1_t  Normal
		)
		{
			luassert(pParallel != nullptr);
			luassert(pPerpendicular != nullptr);

			vec_float_t Scale = Vector3Dot(V, Normal);

			vec_float_t Parallel = VectorMultiply(Normal, Scale);

			*pParallel = Parallel;
			*pPerpendicular = VectorSubtract(V, Parallel);
		}

		//------------------------------------------------------------------------------
		// Transform a vector using a rotation expressed as a unit Quaternion

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Rotate
		(
			vec_float_p1_t V,
			vec_float_p1_t RotationQuaternion
		)
		{
			vec_float_t A = VectorSelect(g_Select1110.v, V, g_Select1110.v);
			vec_float_t Q = QuaternionConjugate(RotationQuaternion);
			vec_float_t Result = QuaternionMultiply(Q, A);
			return QuaternionMultiply(Result, RotationQuaternion);
		}

		//------------------------------------------------------------------------------
		// Transform a vector using the inverse of a rotation expressed as a unit Quaternion

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3InverseRotate
		(
			vec_float_p1_t V,
			vec_float_p1_t RotationQuaternion
		)
		{
			vec_float_t A = VectorSelect(g_Select1110.v, V, g_Select1110.v);
			vec_float_t Result = QuaternionMultiply(RotationQuaternion, A);
			vec_float_t Q = QuaternionConjugate(RotationQuaternion);
			return QuaternionMultiply(Result, Q);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Transform
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vec_float_t vResult = vmlaq_lane_f32(M.r[3], M.r[0], VL, 0); // X
			vResult = vmlaq_lane_f32(vResult, M.r[1], VL, 1); // Y
			return vmlaq_lane_f32(vResult, M.r[2], vget_high_f32(V), 0); // Z
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
			vResult = _mm_mul_ps(vResult, M.r[0]);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vTemp = _mm_mul_ps(vTemp, M.r[1]);
			vResult = _mm_add_ps(vResult, vTemp);
			vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			vTemp = _mm_mul_ps(vTemp, M.r[2]);
			vResult = _mm_add_ps(vResult, vTemp);
			vResult = _mm_add_ps(vResult, M.r[3]);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable : 26015 26019, "PREfast noise: Esp:1307" )
#endif


		inline f32* LUNA_SIMD_CALLCONV Vector3TransformStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 12);

			luassert(OutputStride >= 16);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 12) && (OutputStride == 16))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x3_t V = vld3q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 12 * 4;

						float32x2_t r3 = vget_low_f32(row3);
						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Ax+M
						vec_float_t vResult1 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Bx+N

						__prefetch(pInputVector);

						r3 = vget_high_f32(row3);
						r = vget_high_f32(row0);
						vec_float_t vResult2 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Cx+O
						vec_float_t vResult3 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Dx+P

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy+N

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(row1);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[1], r, 0); // Cx+Gy+O
						vResult3 = vmlaq_lane_f32(vResult3, V.val[1], r, 1); // Dx+Hy+P

						__prefetch(pInputVector + (g_cache_line_size * 3));

						r = vget_low_f32(row2);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[2], r, 0); // Ax+Ey+Iz+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[2], r, 1); // Bx+Fy+Jz+N

						__prefetch(pInputVector + (g_cache_line_size * 4));

						r = vget_high_f32(row2);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[2], r, 0); // Cx+Gy+Kz+O
						vResult3 = vmlaq_lane_f32(vResult3, V.val[2], r, 1); // Dx+Hy+Lz+P

						__prefetch(pInputVector + (g_cache_line_size * 5));

						float32x4x4_t R;
						R.val[0] = vResult0;
						R.val[1] = vResult1;
						R.val[2] = vResult2;
						R.val[3] = vResult3;

						vst4q_f32(reinterpret_cast<float*>(pOutputVector), R);
						pOutputVector += 16 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				float32x2_t VL = vld1_f32(reinterpret_cast<const float*>(pInputVector));
				float32x2_t zero = vdup_n_f32(0);
				float32x2_t VH = vld1_lane_f32(reinterpret_cast<const float*>(pInputVector) + 2, zero, 0);
				pInputVector += InputStride;

				vec_float_t vResult = vmlaq_lane_f32(row3, row0, VL, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, VL, 1); // Y
				vResult = vmlaq_lane_f32(vResult, row2, VH, 0); // Z

				vst1q_f32(reinterpret_cast<float*>(pOutputVector), vResult);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if (InputStride == 12)
				{
					if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF) && !(OutputStride & 0xF))
					{
						// Packed input, aligned output
						for (usize j = 0; j < four; ++j)
						{
							__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
							__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
							pInputVector += 12 * 4;

							// Unpack the 4 vectors (.w components are junk)
							LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

							// Result 1
							vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Z, row2);
							vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
							vec_float_t vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 2
							Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 3
							Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 4
							Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 4;
						}
					}
					else
					{
						// Packed input, unaligned output
						for (usize j = 0; j < four; ++j)
						{
							__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
							__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
							pInputVector += 12 * 4;

							// Unpack the 4 vectors (.w components are junk)
							LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

							// Result 1
							vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Z, row2);
							vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
							vec_float_t vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 2
							Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 3
							Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 4
							Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);
							_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 4;
						}
					}
				}
			}

			if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF) && !(OutputStride & 0xF))
			{
				// Aligned output
				for (; i < VectorCount; ++i)
				{
					vec_float_t V = load_float3(reinterpret_cast<const f32*>(pInputVector));
					pInputVector += InputStride;

					vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Z, row2);
					vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
					vec_float_t vTemp3 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, row3);
					vTemp = _mm_add_ps(vTemp, vTemp2);
					vTemp = _mm_add_ps(vTemp, vTemp3);

					LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTemp);
					pOutputVector += OutputStride;
				}
			}
			else
			{
				// Unaligned output
				for (; i < VectorCount; ++i)
				{
					vec_float_t V = load_float3(reinterpret_cast<const f32*>(pInputVector));
					pInputVector += InputStride;

					vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
					vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
					vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

					vec_float_t vTemp = _mm_mul_ps(Z, row2);
					vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
					vec_float_t vTemp3 = _mm_mul_ps(X, row0);
					vTemp = _mm_add_ps(vTemp, row3);
					vTemp = _mm_add_ps(vTemp, vTemp2);
					vTemp = _mm_add_ps(vTemp, vTemp3);

					_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTemp);
					pOutputVector += OutputStride;
				}
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3TransformCoord
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
			vec_float_t Z = vector_splat_z(V);
			vec_float_t Y = vector_splat_y(V);
			vec_float_t X = vector_splat_x(V);

			vec_float_t Result = VectorMultiplyAdd(Z, M.r[2], M.r[3]);
			Result = VectorMultiplyAdd(Y, M.r[1], Result);
			Result = VectorMultiplyAdd(X, M.r[0], Result);

			vec_float_t W = vector_splat_w(Result);
			return VectorDivide(Result, W);
		}

		//------------------------------------------------------------------------------

#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable : 26015 26019, "PREfast noise: Esp:1307" )
#endif


		inline f32* LUNA_SIMD_CALLCONV Vector3TransformCoordStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 12);

			luassert(OutputStride >= 12);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 12) && (OutputStride == 12))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x3_t V = vld3q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 12 * 4;

						float32x2_t r3 = vget_low_f32(row3);
						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Ax+M
						vec_float_t vResult1 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Bx+N

						__prefetch(pInputVector);

						r3 = vget_high_f32(row3);
						r = vget_high_f32(row0);
						vec_float_t vResult2 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Cx+O
						vec_float_t W = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Dx+P

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy+N

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(row1);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[1], r, 0); // Cx+Gy+O
						W = vmlaq_lane_f32(W, V.val[1], r, 1); // Dx+Hy+P

						__prefetch(pInputVector + (g_cache_line_size * 3));

						r = vget_low_f32(row2);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[2], r, 0); // Ax+Ey+Iz+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[2], r, 1); // Bx+Fy+Jz+N

						__prefetch(pInputVector + (g_cache_line_size * 4));

						r = vget_high_f32(row2);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[2], r, 0); // Cx+Gy+Kz+O
						W = vmlaq_lane_f32(W, V.val[2], r, 1); // Dx+Hy+Lz+P

						__prefetch(pInputVector + (g_cache_line_size * 5));

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
						V.val[0] = vdivq_f32(vResult0, W);
						V.val[1] = vdivq_f32(vResult1, W);
						V.val[2] = vdivq_f32(vResult2, W);
#else
						// 2 iterations of Newton-Raphson refinement of reciprocal
						float32x4_t Reciprocal = vrecpeq_f32(W);
						float32x4_t S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);
						S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);

						V.val[0] = vmulq_f32(vResult0, Reciprocal);
						V.val[1] = vmulq_f32(vResult1, Reciprocal);
						V.val[2] = vmulq_f32(vResult2, Reciprocal);
#endif

						vst3q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 12 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				float32x2_t VL = vld1_f32(reinterpret_cast<const float*>(pInputVector));
				float32x2_t zero = vdup_n_f32(0);
				float32x2_t VH = vld1_lane_f32(reinterpret_cast<const float*>(pInputVector) + 2, zero, 0);
				pInputVector += InputStride;

				vec_float_t vResult = vmlaq_lane_f32(row3, row0, VL, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, VL, 1); // Y
				vResult = vmlaq_lane_f32(vResult, row2, VH, 0); // Z

				VH = vget_high_f32(vResult);
				vec_float_t W = vdupq_lane_f32(VH, 1);

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
				vResult = vdivq_f32(vResult, W);
#else
				// 2 iterations of Newton-Raphson refinement of reciprocal for W
				float32x4_t Reciprocal = vrecpeq_f32(W);
				float32x4_t S = vrecpsq_f32(Reciprocal, W);
				Reciprocal = vmulq_f32(S, Reciprocal);
				S = vrecpsq_f32(Reciprocal, W);
				Reciprocal = vmulq_f32(S, Reciprocal);

				vResult = vmulq_f32(vResult, Reciprocal);
#endif

				VL = vget_low_f32(vResult);
				vst1_f32(reinterpret_cast<float*>(pOutputVector), VL);
				vst1q_lane_f32(reinterpret_cast<float*>(pOutputVector) + 2, vResult, 2);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if (InputStride == 12)
				{
					if (OutputStride == 12)
					{
						if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF))
						{
							// Packed input, aligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, row2);
								vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
								vec_float_t vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V1 = _mm_div_ps(vTemp, W);

								// Result 2
								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V2 = _mm_div_ps(vTemp, W);

								// Result 3
								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V3 = _mm_div_ps(vTemp, W);

								// Result 4
								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V4 = _mm_div_ps(vTemp, W);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), V1);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
						else
						{
							// Packed input, unaligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, row2);
								vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
								vec_float_t vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V1 = _mm_div_ps(vTemp, W);

								// Result 2
								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V2 = _mm_div_ps(vTemp, W);

								// Result 3
								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V3 = _mm_div_ps(vTemp, W);

								// Result 4
								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, row3);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

								V4 = _mm_div_ps(vTemp, W);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), V1);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
					}
					else
					{
						// Packed input, unpacked output
						for (usize j = 0; j < four; ++j)
						{
							__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
							__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
							pInputVector += 12 * 4;

							// Unpack the 4 vectors (.w components are junk)
							LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

							// Result 1
							vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Z, row2);
							vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
							vec_float_t vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

							vTemp = _mm_div_ps(vTemp, W);
							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 2
							Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

							vTemp = _mm_div_ps(vTemp, W);
							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 3
							Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

							vTemp = _mm_div_ps(vTemp, W);
							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 4
							Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, row3);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

							vTemp = _mm_div_ps(vTemp, W);
							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 4;
						}
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				vec_float_t V = load_float3(reinterpret_cast<const f32*>(pInputVector));
				pInputVector += InputStride;

				vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
				vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
				vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

				vec_float_t vTemp = _mm_mul_ps(Z, row2);
				vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
				vec_float_t vTemp3 = _mm_mul_ps(X, row0);
				vTemp = _mm_add_ps(vTemp, row3);
				vTemp = _mm_add_ps(vTemp, vTemp2);
				vTemp = _mm_add_ps(vTemp, vTemp3);

				vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));

				vTemp = _mm_div_ps(vTemp, W);

				store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
				pOutputVector += OutputStride;
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3TransformNormal
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vec_float_t vResult = vmulq_lane_f32(M.r[0], VL, 0); // X
			vResult = vmlaq_lane_f32(vResult, M.r[1], VL, 1); // Y
			return vmlaq_lane_f32(vResult, M.r[2], vget_high_f32(V), 0); // Z
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
			vResult = _mm_mul_ps(vResult, M.r[0]);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vTemp = _mm_mul_ps(vTemp, M.r[1]);
			vResult = _mm_add_ps(vResult, vTemp);
			vTemp = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			vTemp = _mm_mul_ps(vTemp, M.r[2]);
			vResult = _mm_add_ps(vResult, vTemp);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable : 26015 26019, "PREfast noise: Esp:1307" )
#endif


		inline f32* LUNA_SIMD_CALLCONV Vector3TransformNormalStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 12);

			luassert(OutputStride >= 12);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 12) && (OutputStride == 12))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x3_t V = vld3q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 12 * 4;

						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmulq_lane_f32(V.val[0], r, 0); // Ax
						vec_float_t vResult1 = vmulq_lane_f32(V.val[0], r, 1); // Bx

						__prefetch(pInputVector);

						r = vget_high_f32(row0);
						vec_float_t vResult2 = vmulq_lane_f32(V.val[0], r, 0); // Cx

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(row1);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[1], r, 0); // Cx+Gy

						__prefetch(pInputVector + (g_cache_line_size * 3));

						r = vget_low_f32(row2);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[2], r, 0); // Ax+Ey+Iz
						vResult1 = vmlaq_lane_f32(vResult1, V.val[2], r, 1); // Bx+Fy+Jz

						__prefetch(pInputVector + (g_cache_line_size * 4));

						r = vget_high_f32(row2);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[2], r, 0); // Cx+Gy+Kz

						__prefetch(pInputVector + (g_cache_line_size * 5));

						V.val[0] = vResult0;
						V.val[1] = vResult1;
						V.val[2] = vResult2;

						vst3q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 12 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				float32x2_t VL = vld1_f32(reinterpret_cast<const float*>(pInputVector));
				float32x2_t zero = vdup_n_f32(0);
				float32x2_t VH = vld1_lane_f32(reinterpret_cast<const float*>(pInputVector) + 2, zero, 0);
				pInputVector += InputStride;

				vec_float_t vResult = vmulq_lane_f32(row0, VL, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, VL, 1); // Y
				vResult = vmlaq_lane_f32(vResult, row2, VH, 0); // Z

				VL = vget_low_f32(vResult);
				vst1_f32(reinterpret_cast<float*>(pOutputVector), VL);
				vst1q_lane_f32(reinterpret_cast<float*>(pOutputVector) + 2, vResult, 2);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if (InputStride == 12)
				{
					if (OutputStride == 12)
					{
						if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF))
						{
							// Packed input, aligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, row2);
								vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
								vec_float_t vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V1 = _mm_add_ps(vTemp, vTemp3);

								// Result 2
								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V2 = _mm_add_ps(vTemp, vTemp3);

								// Result 3
								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V3 = _mm_add_ps(vTemp, vTemp3);

								// Result 4
								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V4 = _mm_add_ps(vTemp, vTemp3);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), V1);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
						else
						{
							// Packed input, unaligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, row2);
								vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
								vec_float_t vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V1 = _mm_add_ps(vTemp, vTemp3);

								// Result 2
								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V2 = _mm_add_ps(vTemp, vTemp3);

								// Result 3
								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V3 = _mm_add_ps(vTemp, vTemp3);

								// Result 4
								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, row2);
								vTemp2 = _mm_mul_ps(Y, row1);
								vTemp3 = _mm_mul_ps(X, row0);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								V4 = _mm_add_ps(vTemp, vTemp3);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), V1);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
					}
					else
					{
						// Packed input, unpacked output
						for (usize j = 0; j < four; ++j)
						{
							__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
							__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
							pInputVector += 12 * 4;

							// Unpack the 4 vectors (.w components are junk)
							LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

							// Result 1
							vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Z, row2);
							vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
							vec_float_t vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 2
							Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 3
							Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 4
							Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, row2);
							vTemp2 = _mm_mul_ps(Y, row1);
							vTemp3 = _mm_mul_ps(X, row0);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 4;
						}
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				vec_float_t V = load_float3(reinterpret_cast<const f32*>(pInputVector));
				pInputVector += InputStride;

				vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
				vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
				vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

				vec_float_t vTemp = _mm_mul_ps(Z, row2);
				vec_float_t vTemp2 = _mm_mul_ps(Y, row1);
				vec_float_t vTemp3 = _mm_mul_ps(X, row0);
				vTemp = _mm_add_ps(vTemp, vTemp2);
				vTemp = _mm_add_ps(vTemp, vTemp3);

				store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
				pOutputVector += OutputStride;
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Project
		(
			vec_float_p1_t V,
			f32    ViewportX,
			f32    ViewportY,
			f32    ViewportWidth,
			f32    ViewportHeight,
			f32    ViewportMinZ,
			f32    ViewportMaxZ,
			mat_float_p1_t Projection,
			mat_float_p2_t View,
			mat_float_p2_t World
		)
		{
			const f32 HalfViewportWidth = ViewportWidth * 0.5f;
			const f32 HalfViewportHeight = ViewportHeight * 0.5f;

			vec_float_t Scale = vector_set(HalfViewportWidth, -HalfViewportHeight, ViewportMaxZ - ViewportMinZ, 0.0f);
			vec_float_t Offset = vector_set(ViewportX + HalfViewportWidth, ViewportY + HalfViewportHeight, ViewportMinZ, 0.0f);

			mat_float_t Transform = MatrixMultiply(World, View);
			Transform = MatrixMultiply(Transform, Projection);

			vec_float_t Result = Vector3TransformCoord(V, Transform);

			Result = VectorMultiplyAdd(Result, Scale, Offset);

			return Result;
		}

		//------------------------------------------------------------------------------

#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable : 26015 26019, "PREfast noise: Esp:1307" )
#endif


		inline f32* LUNA_SIMD_CALLCONV Vector3ProjectStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			f32           ViewportX,
			f32           ViewportY,
			f32           ViewportWidth,
			f32           ViewportHeight,
			f32           ViewportMinZ,
			f32           ViewportMaxZ,
			mat_float_p1_t     Projection,
			mat_float_p2_t     View,
			mat_float_p2_t     World
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 12);

			luassert(OutputStride >= 12);

#if defined(LUNA_PLATFORM_ARM_NEON)
			const f32 HalfViewportWidth = ViewportWidth * 0.5f;
			const f32 HalfViewportHeight = ViewportHeight * 0.5f;

			mat_float_t Transform = MatrixMultiply(World, View);
			Transform = MatrixMultiply(Transform, Projection);

			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 12) && (OutputStride == 12))
				{
					vec_float_t ScaleX = vdupq_n_f32(HalfViewportWidth);
					vec_float_t ScaleY = vdupq_n_f32(-HalfViewportHeight);
					vec_float_t ScaleZ = vdupq_n_f32(ViewportMaxZ - ViewportMinZ);

					vec_float_t OffsetX = vdupq_n_f32(ViewportX + HalfViewportWidth);
					vec_float_t OffsetY = vdupq_n_f32(ViewportY + HalfViewportHeight);
					vec_float_t OffsetZ = vdupq_n_f32(ViewportMinZ);

					for (usize j = 0; j < four; ++j)
					{
						float32x4x3_t V = vld3q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 12 * 4;

						float32x2_t r3 = vget_low_f32(Transform.r[3]);
						float32x2_t r = vget_low_f32(Transform.r[0]);
						vec_float_t vResult0 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Ax+M
						vec_float_t vResult1 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Bx+N

						__prefetch(pInputVector);

						r3 = vget_high_f32(Transform.r[3]);
						r = vget_high_f32(Transform.r[0]);
						vec_float_t vResult2 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), V.val[0], r, 0); // Cx+O
						vec_float_t W = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), V.val[0], r, 1); // Dx+P

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(Transform.r[1]);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy+N

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(Transform.r[1]);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[1], r, 0); // Cx+Gy+O
						W = vmlaq_lane_f32(W, V.val[1], r, 1); // Dx+Hy+P

						__prefetch(pInputVector + (g_cache_line_size * 3));

						r = vget_low_f32(Transform.r[2]);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[2], r, 0); // Ax+Ey+Iz+M
						vResult1 = vmlaq_lane_f32(vResult1, V.val[2], r, 1); // Bx+Fy+Jz+N

						__prefetch(pInputVector + (g_cache_line_size * 4));

						r = vget_high_f32(Transform.r[2]);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[2], r, 0); // Cx+Gy+Kz+O
						W = vmlaq_lane_f32(W, V.val[2], r, 1); // Dx+Hy+Lz+P

						__prefetch(pInputVector + (g_cache_line_size * 5));

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
						vResult0 = vdivq_f32(vResult0, W);
						vResult1 = vdivq_f32(vResult1, W);
						vResult2 = vdivq_f32(vResult2, W);
#else
						// 2 iterations of Newton-Raphson refinement of reciprocal
						float32x4_t Reciprocal = vrecpeq_f32(W);
						float32x4_t S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);
						S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);

						vResult0 = vmulq_f32(vResult0, Reciprocal);
						vResult1 = vmulq_f32(vResult1, Reciprocal);
						vResult2 = vmulq_f32(vResult2, Reciprocal);
#endif

						V.val[0] = vmlaq_f32(OffsetX, vResult0, ScaleX);
						V.val[1] = vmlaq_f32(OffsetY, vResult1, ScaleY);
						V.val[2] = vmlaq_f32(OffsetZ, vResult2, ScaleZ);

						vst3q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 12 * 4;

						i += 4;
					}
				}
			}

			if (i < VectorCount)
			{
				vec_float_t Scale = vector_set(HalfViewportWidth, -HalfViewportHeight, ViewportMaxZ - ViewportMinZ, 1.0f);
				vec_float_t Offset = vector_set(ViewportX + HalfViewportWidth, ViewportY + HalfViewportHeight, ViewportMinZ, 0.0f);

				for (; i < VectorCount; i++)
				{
					float32x2_t VL = vld1_f32(reinterpret_cast<const float*>(pInputVector));
					float32x2_t zero = vdup_n_f32(0);
					float32x2_t VH = vld1_lane_f32(reinterpret_cast<const float*>(pInputVector) + 2, zero, 0);
					pInputVector += InputStride;

					vec_float_t vResult = vmlaq_lane_f32(Transform.r[3], Transform.r[0], VL, 0); // X
					vResult = vmlaq_lane_f32(vResult, Transform.r[1], VL, 1); // Y
					vResult = vmlaq_lane_f32(vResult, Transform.r[2], VH, 0); // Z

					VH = vget_high_f32(vResult);
					vec_float_t W = vdupq_lane_f32(VH, 1);

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
					vResult = vdivq_f32(vResult, W);
#else
					// 2 iterations of Newton-Raphson refinement of reciprocal for W
					float32x4_t Reciprocal = vrecpeq_f32(W);
					float32x4_t S = vrecpsq_f32(Reciprocal, W);
					Reciprocal = vmulq_f32(S, Reciprocal);
					S = vrecpsq_f32(Reciprocal, W);
					Reciprocal = vmulq_f32(S, Reciprocal);

					vResult = vmulq_f32(vResult, Reciprocal);
#endif

					vResult = vmlaq_f32(Offset, vResult, Scale);

					VL = vget_low_f32(vResult);
					vst1_f32(reinterpret_cast<float*>(pOutputVector), VL);
					vst1q_lane_f32(reinterpret_cast<float*>(pOutputVector) + 2, vResult, 2);
					pOutputVector += OutputStride;
				}
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			const f32 HalfViewportWidth = ViewportWidth * 0.5f;
			const f32 HalfViewportHeight = ViewportHeight * 0.5f;

			vec_float_t Scale = vector_set(HalfViewportWidth, -HalfViewportHeight, ViewportMaxZ - ViewportMinZ, 1.0f);
			vec_float_t Offset = vector_set(ViewportX + HalfViewportWidth, ViewportY + HalfViewportHeight, ViewportMinZ, 0.0f);

			mat_float_t Transform = MatrixMultiply(World, View);
			Transform = MatrixMultiply(Transform, Projection);

			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if (InputStride == 12)
				{
					if (OutputStride == 12)
					{
						if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF))
						{
							// Packed input, aligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V1 = _mm_add_ps(vTemp, Offset);

								// Result 2
								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V2 = _mm_add_ps(vTemp, Offset);

								// Result 3
								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V3 = _mm_add_ps(vTemp, Offset);

								// Result 4
								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V4 = _mm_add_ps(vTemp, Offset);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), V1);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
						else
						{
							// Packed input, unaligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V1 = _mm_add_ps(vTemp, Offset);

								// Result 2
								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V2 = _mm_add_ps(vTemp, Offset);

								// Result 3
								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V3 = _mm_add_ps(vTemp, Offset);

								// Result 4
								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								vTemp = _mm_div_ps(vTemp, W);

								vTemp = _mm_mul_ps(vTemp, Scale);
								V4 = _mm_add_ps(vTemp, Offset);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), V1);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
					}
					else
					{
						// Packed input, unpacked output
						for (usize j = 0; j < four; ++j)
						{
							__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
							__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
							pInputVector += 12 * 4;

							// Unpack the 4 vectors (.w components are junk)
							LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

							// Result 1
							vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							vTemp = _mm_mul_ps(vTemp, Scale);
							vTemp = _mm_add_ps(vTemp, Offset);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 2
							Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							vTemp = _mm_mul_ps(vTemp, Scale);
							vTemp = _mm_add_ps(vTemp, Offset);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 3
							Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							vTemp = _mm_mul_ps(vTemp, Scale);
							vTemp = _mm_add_ps(vTemp, Offset);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 4
							Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							vTemp = _mm_mul_ps(vTemp, Scale);
							vTemp = _mm_add_ps(vTemp, Offset);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 4;
						}
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				vec_float_t V = load_float3(reinterpret_cast<const f32*>(pInputVector));
				pInputVector += InputStride;

				vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
				vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
				vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

				vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
				vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
				vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
				vTemp = _mm_add_ps(vTemp, Transform.r[3]);
				vTemp = _mm_add_ps(vTemp, vTemp2);
				vTemp = _mm_add_ps(vTemp, vTemp3);

				vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
				vTemp = _mm_div_ps(vTemp, W);

				vTemp = _mm_mul_ps(vTemp, Scale);
				vTemp = _mm_add_ps(vTemp, Offset);

				store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
				pOutputVector += OutputStride;
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector3Unproject
		(
			vec_float_p1_t V,
			f32     ViewportX,
			f32     ViewportY,
			f32     ViewportWidth,
			f32     ViewportHeight,
			f32     ViewportMinZ,
			f32     ViewportMaxZ,
			mat_float_p1_t Projection,
			mat_float_p2_t View,
			mat_float_p2_t World
		)
		{
			static const vec_f32_t D = { { { -1.0f, 1.0f, 0.0f, 0.0f } } };

			vec_float_t Scale = vector_set(ViewportWidth * 0.5f, -ViewportHeight * 0.5f, ViewportMaxZ - ViewportMinZ, 1.0f);
			Scale = VectorReciprocal(Scale);

			vec_float_t Offset = vector_set(-ViewportX, -ViewportY, -ViewportMinZ, 0.0f);
			Offset = VectorMultiplyAdd(Scale, Offset, D.v);

			mat_float_t Transform = MatrixMultiply(World, View);
			Transform = MatrixMultiply(Transform, Projection);
			Transform = MatrixInverse(nullptr, Transform);

			vec_float_t Result = VectorMultiplyAdd(V, Scale, Offset);

			return Vector3TransformCoord(Result, Transform);
		}

		//------------------------------------------------------------------------------

#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable : 26015 26019, "PREfast noise: Esp:1307" )
#endif


		inline f32* LUNA_SIMD_CALLCONV Vector3UnprojectStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			f32           ViewportX,
			f32           ViewportY,
			f32           ViewportWidth,
			f32           ViewportHeight,
			f32           ViewportMinZ,
			f32           ViewportMaxZ,
			mat_float_p1_t       Projection,
			mat_float_p2_t       View,
			mat_float_p2_t       World)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 12);

			luassert(OutputStride >= 12);

#if defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t Transform = MatrixMultiply(World, View);
			Transform = MatrixMultiply(Transform, Projection);
			Transform = MatrixInverse(nullptr, Transform);

			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			f32 sx = 1.f / (ViewportWidth * 0.5f);
			f32 sy = 1.f / (-ViewportHeight * 0.5f);
			f32 sz = 1.f / (ViewportMaxZ - ViewportMinZ);

			f32 ox = (-ViewportX * sx) - 1.f;
			f32 oy = (-ViewportY * sy) + 1.f;
			f32 oz = (-ViewportMinZ * sz);

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 12) && (OutputStride == 12))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x3_t V = vld3q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 12 * 4;

						vec_float_t ScaleX = vdupq_n_f32(sx);
						vec_float_t OffsetX = vdupq_n_f32(ox);
						vec_float_t VX = vmlaq_f32(OffsetX, ScaleX, V.val[0]);

						float32x2_t r3 = vget_low_f32(Transform.r[3]);
						float32x2_t r = vget_low_f32(Transform.r[0]);
						vec_float_t vResult0 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), VX, r, 0); // Ax+M
						vec_float_t vResult1 = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), VX, r, 1); // Bx+N

						__prefetch(pInputVector);

						r3 = vget_high_f32(Transform.r[3]);
						r = vget_high_f32(Transform.r[0]);
						vec_float_t vResult2 = vmlaq_lane_f32(vdupq_lane_f32(r3, 0), VX, r, 0); // Cx+O
						vec_float_t W = vmlaq_lane_f32(vdupq_lane_f32(r3, 1), VX, r, 1); // Dx+P

						__prefetch(pInputVector + g_cache_line_size);

						vec_float_t ScaleY = vdupq_n_f32(sy);
						vec_float_t OffsetY = vdupq_n_f32(oy);
						vec_float_t VY = vmlaq_f32(OffsetY, ScaleY, V.val[1]);

						r = vget_low_f32(Transform.r[1]);
						vResult0 = vmlaq_lane_f32(vResult0, VY, r, 0); // Ax+Ey+M
						vResult1 = vmlaq_lane_f32(vResult1, VY, r, 1); // Bx+Fy+N

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(Transform.r[1]);
						vResult2 = vmlaq_lane_f32(vResult2, VY, r, 0); // Cx+Gy+O
						W = vmlaq_lane_f32(W, VY, r, 1); // Dx+Hy+P

						__prefetch(pInputVector + (g_cache_line_size * 3));

						vec_float_t ScaleZ = vdupq_n_f32(sz);
						vec_float_t OffsetZ = vdupq_n_f32(oz);
						vec_float_t VZ = vmlaq_f32(OffsetZ, ScaleZ, V.val[2]);

						r = vget_low_f32(Transform.r[2]);
						vResult0 = vmlaq_lane_f32(vResult0, VZ, r, 0); // Ax+Ey+Iz+M
						vResult1 = vmlaq_lane_f32(vResult1, VZ, r, 1); // Bx+Fy+Jz+N

						__prefetch(pInputVector + (g_cache_line_size * 4));

						r = vget_high_f32(Transform.r[2]);
						vResult2 = vmlaq_lane_f32(vResult2, VZ, r, 0); // Cx+Gy+Kz+O
						W = vmlaq_lane_f32(W, VZ, r, 1); // Dx+Hy+Lz+P

						__prefetch(pInputVector + (g_cache_line_size * 5));

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
						V.val[0] = vdivq_f32(vResult0, W);
						V.val[1] = vdivq_f32(vResult1, W);
						V.val[2] = vdivq_f32(vResult2, W);
#else
						// 2 iterations of Newton-Raphson refinement of reciprocal
						float32x4_t Reciprocal = vrecpeq_f32(W);
						float32x4_t S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);
						S = vrecpsq_f32(Reciprocal, W);
						Reciprocal = vmulq_f32(S, Reciprocal);

						V.val[0] = vmulq_f32(vResult0, Reciprocal);
						V.val[1] = vmulq_f32(vResult1, Reciprocal);
						V.val[2] = vmulq_f32(vResult2, Reciprocal);
#endif

						vst3q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 12 * 4;

						i += 4;
					}
				}
			}

			if (i < VectorCount)
			{
				float32x2_t ScaleL = vcreate_f32(
					static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&sx))
					| (static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&sy)) << 32));
				float32x2_t ScaleH = vcreate_f32(static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&sz)));

				float32x2_t OffsetL = vcreate_f32(
					static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&ox))
					| (static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&oy)) << 32));
				float32x2_t OffsetH = vcreate_f32(static_cast<uint64_t>(*reinterpret_cast<const u32 *>(&oz)));

				for (; i < VectorCount; i++)
				{
					float32x2_t VL = vld1_f32(reinterpret_cast<const float*>(pInputVector));
					float32x2_t zero = vdup_n_f32(0);
					float32x2_t VH = vld1_lane_f32(reinterpret_cast<const float*>(pInputVector) + 2, zero, 0);
					pInputVector += InputStride;

					VL = vmla_f32(OffsetL, VL, ScaleL);
					VH = vmla_f32(OffsetH, VH, ScaleH);

					vec_float_t vResult = vmlaq_lane_f32(Transform.r[3], Transform.r[0], VL, 0); // X
					vResult = vmlaq_lane_f32(vResult, Transform.r[1], VL, 1); // Y
					vResult = vmlaq_lane_f32(vResult, Transform.r[2], VH, 0); // Z

					VH = vget_high_f32(vResult);
					vec_float_t W = vdupq_lane_f32(VH, 1);

#if defined(_M_ARM64) || defined(_M_HYBRID_X86_ARM64)
					vResult = vdivq_f32(vResult, W);
#else
					// 2 iterations of Newton-Raphson refinement of reciprocal for W
					float32x4_t Reciprocal = vrecpeq_f32(W);
					float32x4_t S = vrecpsq_f32(Reciprocal, W);
					Reciprocal = vmulq_f32(S, Reciprocal);
					S = vrecpsq_f32(Reciprocal, W);
					Reciprocal = vmulq_f32(S, Reciprocal);

					vResult = vmulq_f32(vResult, Reciprocal);
#endif

					VL = vget_low_f32(vResult);
					vst1_f32(reinterpret_cast<float*>(pOutputVector), VL);
					vst1q_lane_f32(reinterpret_cast<float*>(pOutputVector) + 2, vResult, 2);
					pOutputVector += OutputStride;
				}
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t D = { { { -1.0f, 1.0f, 0.0f, 0.0f } } };

			vec_float_t Scale = vector_set(ViewportWidth * 0.5f, -ViewportHeight * 0.5f, ViewportMaxZ - ViewportMinZ, 1.0f);
			Scale = VectorReciprocal(Scale);

			vec_float_t Offset = vector_set(-ViewportX, -ViewportY, -ViewportMinZ, 0.0f);
			Offset = _mm_mul_ps(Scale, Offset);
			Offset = _mm_add_ps(Offset, D);

			mat_float_t Transform = MatrixMultiply(World, View);
			Transform = MatrixMultiply(Transform, Projection);
			Transform = MatrixInverse(nullptr, Transform);

			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if (InputStride == 12)
				{
					if (OutputStride == 12)
					{
						if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF))
						{
							// Packed input, aligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								V1 = _mm_mul_ps(V1, Scale);
								V1 = _mm_add_ps(V1, Offset);

								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V1 = _mm_div_ps(vTemp, W);

								// Result 2
								V2 = _mm_mul_ps(V2, Scale);
								V2 = _mm_add_ps(V2, Offset);

								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V2 = _mm_div_ps(vTemp, W);

								// Result 3
								V3 = _mm_mul_ps(V3, Scale);
								V3 = _mm_add_ps(V3, Offset);

								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V3 = _mm_div_ps(vTemp, W);

								// Result 4
								V4 = _mm_mul_ps(V4, Scale);
								V4 = _mm_add_ps(V4, Offset);

								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V4 = _mm_div_ps(vTemp, W);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), V1);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
						else
						{
							// Packed input, unaligned & packed output
							for (usize j = 0; j < four; ++j)
							{
								__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
								__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
								__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
								pInputVector += 12 * 4;

								// Unpack the 4 vectors (.w components are junk)
								LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

								// Result 1
								V1 = _mm_mul_ps(V1, Scale);
								V1 = _mm_add_ps(V1, Offset);

								vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
								vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
								vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

								vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V1 = _mm_div_ps(vTemp, W);

								// Result 2
								V2 = _mm_mul_ps(V2, Scale);
								V2 = _mm_add_ps(V2, Offset);

								Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V2 = _mm_div_ps(vTemp, W);

								// Result 3
								V3 = _mm_mul_ps(V3, Scale);
								V3 = _mm_add_ps(V3, Offset);

								Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V3 = _mm_div_ps(vTemp, W);

								// Result 4
								V4 = _mm_mul_ps(V4, Scale);
								V4 = _mm_add_ps(V4, Offset);

								Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
								Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
								X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

								vTemp = _mm_mul_ps(Z, Transform.r[2]);
								vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
								vTemp3 = _mm_mul_ps(X, Transform.r[0]);
								vTemp = _mm_add_ps(vTemp, Transform.r[3]);
								vTemp = _mm_add_ps(vTemp, vTemp2);
								vTemp = _mm_add_ps(vTemp, vTemp3);

								W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
								V4 = _mm_div_ps(vTemp, W);

								// Pack and store the vectors
								LUNA_SIMD_3PACK4INTO3(vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), V1);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 16), vTemp);
								_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector + 32), V3);
								pOutputVector += 12 * 4;
								i += 4;
							}
						}
					}
					else
					{
						// Packed input, unpacked output
						for (usize j = 0; j < four; ++j)
						{
							__m128 V1 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
							__m128 L2 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 16));
							__m128 L3 = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector + 32));
							pInputVector += 12 * 4;

							// Unpack the 4 vectors (.w components are junk)
							LUNA_SIMD_3UNPACK3INTO4(V1, L2, L3);

							// Result 1
							V1 = _mm_mul_ps(V1, Scale);
							V1 = _mm_add_ps(V1, Offset);

							vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 2, 2, 2));
							vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 1, 1));
							vec_float_t X = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 0));

							vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 2
							V2 = _mm_mul_ps(V2, Scale);
							V2 = _mm_add_ps(V2, Offset);

							Z = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 3
							V3 = _mm_mul_ps(V3, Scale);
							V3 = _mm_add_ps(V3, Offset);

							Z = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							// Result 4
							V4 = _mm_mul_ps(V4, Scale);
							V4 = _mm_add_ps(V4, Offset);

							Z = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(2, 2, 2, 2));
							Y = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(1, 1, 1, 1));
							X = LUNA_SIMD_PERMUTE_PS(V4, _MM_SHUFFLE(0, 0, 0, 0));

							vTemp = _mm_mul_ps(Z, Transform.r[2]);
							vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
							vTemp3 = _mm_mul_ps(X, Transform.r[0]);
							vTemp = _mm_add_ps(vTemp, Transform.r[3]);
							vTemp = _mm_add_ps(vTemp, vTemp2);
							vTemp = _mm_add_ps(vTemp, vTemp3);

							W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
							vTemp = _mm_div_ps(vTemp, W);

							store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
							pOutputVector += OutputStride;

							i += 4;
						}
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				vec_float_t V = load_float3(reinterpret_cast<const f32*>(pInputVector));
				pInputVector += InputStride;

				V = _mm_mul_ps(V, Scale);
				V = _mm_add_ps(V, Offset);

				vec_float_t Z = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
				vec_float_t Y = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
				vec_float_t X = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));

				vec_float_t vTemp = _mm_mul_ps(Z, Transform.r[2]);
				vec_float_t vTemp2 = _mm_mul_ps(Y, Transform.r[1]);
				vec_float_t vTemp3 = _mm_mul_ps(X, Transform.r[0]);
				vTemp = _mm_add_ps(vTemp, Transform.r[3]);
				vTemp = _mm_add_ps(vTemp, vTemp2);
				vTemp = _mm_add_ps(vTemp, vTemp3);

				vec_float_t W = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(3, 3, 3, 3));
				vTemp = _mm_div_ps(vTemp, W);

				store_float3(reinterpret_cast<f32*>(pOutputVector), vTemp);
				pOutputVector += OutputStride;
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif

		/****************************************************************************
		 *
		 * 4D Vector
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4Equal
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			return ((_mm_movemask_ps(vTemp) == 0x0f) != 0);
#else
			return comparison_all_true(Vector4EqualR(V1, V2));
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector4EqualR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);

			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpeq_ps(V1, V2);
			int iTest = _mm_movemask_ps(vTemp);
			u32 CR = 0;
			if (iTest == 0xf)     // All equal?
			{
				CR = g_crmask_cr6true;
			}
			else if (iTest == 0)  // All not equal?
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4EqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return ((_mm_movemask_ps(_mm_castsi128_ps(vTemp)) == 0xf) != 0);
#else
			return comparison_all_true(Vector4EqualIntR(V1, V2));
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector4EqualIntR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);

			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			int iTest = _mm_movemask_ps(_mm_castsi128_ps(vTemp));
			u32 CR = 0;
			if (iTest == 0xf)     // All equal?
			{
				CR = g_crmask_cr6true;
			}
			else if (iTest == 0)  // All not equal?
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		inline bool LUNA_SIMD_CALLCONV Vector4NearEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t Epsilon
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t vDelta = vsubq_f32(V1, V2);
			uint32x4_t vResult = vacleq_f32(vDelta, Epsilon);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Get the difference
			vec_float_t vDelta = _mm_sub_ps(V1, V2);
			// Get the absolute value of the difference
			vec_float_t vTemp = _mm_setzero_ps();
			vTemp = _mm_sub_ps(vTemp, vDelta);
			vTemp = _mm_max_ps(vTemp, vDelta);
			vTemp = _mm_cmple_ps(vTemp, Epsilon);
			return ((_mm_movemask_ps(vTemp) == 0xf) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4NotEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) != 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpneq_ps(V1, V2);
			return ((_mm_movemask_ps(vTemp)) != 0);
#else
			return comparison_any_false(Vector4EqualR(V1, V2));
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4NotEqualInt
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vceqq_u32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) != 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i vTemp = _mm_cmpeq_epi32(_mm_castps_si128(V1), _mm_castps_si128(V2));
			return ((_mm_movemask_ps(_mm_castsi128_ps(vTemp)) != 0xF) != 0);
#else
			return comparison_any_false(Vector4EqualIntR(V1, V2));
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4Greater
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgtq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			return ((_mm_movemask_ps(vTemp) == 0x0f) != 0);
#else
			return comparison_all_true(Vector4GreaterR(V1, V2));
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector4GreaterR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgtq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);

			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			u32 CR = 0;
			vec_float_t vTemp = _mm_cmpgt_ps(V1, V2);
			int iTest = _mm_movemask_ps(vTemp);
			if (iTest == 0xf) {
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4GreaterOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgeq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			return ((_mm_movemask_ps(vTemp) == 0x0f) != 0);
#else
			return comparison_all_true(Vector4GreaterOrEqualR(V1, V2));
#endif
		}

		//------------------------------------------------------------------------------

		inline u32 LUNA_SIMD_CALLCONV Vector4GreaterOrEqualR
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcgeq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			u32 r = vget_lane_u32(vTemp.val[1], 1);

			u32 CR = 0;
			if (r == 0xFFFFFFFFU)
			{
				CR = g_crmask_cr6true;
			}
			else if (!r)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#elif defined(LUNA_PLATFORM_SSE2)
			u32 CR = 0;
			vec_float_t vTemp = _mm_cmpge_ps(V1, V2);
			int iTest = _mm_movemask_ps(vTemp);
			if (iTest == 0x0f)
			{
				CR = g_crmask_cr6true;
			}
			else if (!iTest)
			{
				CR = g_crmask_cr6false;
			}
			return CR;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4Less
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcltq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmplt_ps(V1, V2);
			return ((_mm_movemask_ps(vTemp) == 0x0f) != 0);
#else
			return comparison_all_true(Vector4GreaterR(V2, V1));
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4LessOrEqual
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t vResult = vcleq_f32(V1, V2);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp = _mm_cmple_ps(V1, V2);
			return ((_mm_movemask_ps(vTemp) == 0x0f) != 0);
#else
			return comparison_all_true(Vector4GreaterOrEqualR(V2, V1));
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4InBounds
		(
			vec_float_p1_t V,
			vec_float_p1_t Bounds
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test if less than or equal
			uint32x4_t ivTemp1 = vcleq_f32(V, Bounds);
			// Negate the bounds
			float32x4_t vTemp2 = vnegq_f32(Bounds);
			// Test if greater or equal (Reversed)
			uint32x4_t ivTemp2 = vcleq_f32(vTemp2, V);
			// Blend answers
			ivTemp1 = vandq_u32(ivTemp1, ivTemp2);
			// in bounds?
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(ivTemp1), vget_high_u8(ivTemp1));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test if less than or equal
			vec_float_t vTemp1 = _mm_cmple_ps(V, Bounds);
			// Negate the bounds
			vec_float_t vTemp2 = _mm_mul_ps(Bounds, g_NegativeOne);
			// Test if greater or equal (Reversed)
			vTemp2 = _mm_cmple_ps(vTemp2, V);
			// Blend answers
			vTemp1 = _mm_and_ps(vTemp1, vTemp2);
			// All in bounds?
			return ((_mm_movemask_ps(vTemp1) == 0x0f) != 0);
#else
			return comparison_all_in_bounds(Vector4InBoundsR(V, Bounds));
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4IsNaN
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Test against itself. NaN is always not equal
			uint32x4_t vTempNan = vceqq_f32(V, V);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTempNan), vget_high_u8(vTempNan));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			// If any are NaN, the mask is zero
			return (vget_lane_u32(vTemp.val[1], 1) != 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			// Test against itself. NaN is always not equal
			vec_float_t vTempNan = _mm_cmpneq_ps(V, V);
			// If any are NaN, the mask is non-zero
			return (_mm_movemask_ps(vTempNan) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV Vector4IsInfinite
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Mask off the sign bit
			uint32x4_t vTempInf = vandq_u32(V, g_AbsMask);
			// Compare to infinity
			vTempInf = vceqq_f32(vTempInf, g_Infinity);
			// If any are infinity, the signs are true.
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTempInf), vget_high_u8(vTempInf));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			return (vget_lane_u32(vTemp.val[1], 1) != 0);
#elif defined(LUNA_PLATFORM_SSE2)
			// Mask off the sign bit
			vec_float_t vTemp = _mm_and_ps(V, g_AbsMask);
			// Compare to infinity
			vTemp = _mm_cmpeq_ps(vTemp, g_Infinity);
			// If any are infinity, the signs are true.
			return (_mm_movemask_ps(vTemp) != 0);
#endif
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Dot
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t vTemp = vmulq_f32(V1, V2);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			return vcombine_f32(v1, v1);
#elif defined(LUNA_PLATFORM_SSE4)
			return _mm_dp_ps(V1, V2, 0xff);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vTemp = _mm_mul_ps(V1, V2);
			vTemp = _mm_hadd_ps(vTemp, vTemp);
			return _mm_hadd_ps(vTemp, vTemp);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp2 = V2;
			vec_float_t vTemp = _mm_mul_ps(V1, vTemp2);
			vTemp2 = _mm_shuffle_ps(vTemp2, vTemp, _MM_SHUFFLE(1, 0, 0, 0)); // Copy X to the Z position and Y to the W position
			vTemp2 = _mm_add_ps(vTemp2, vTemp);          // Add Z = X+Z; W = Y+W;
			vTemp = _mm_shuffle_ps(vTemp, vTemp2, _MM_SHUFFLE(0, 3, 0, 0));  // Copy W to the Z position
			vTemp = _mm_add_ps(vTemp, vTemp2);           // Add Z and W together
			return LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(2, 2, 2, 2));    // Splat Z and return
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Cross
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2,
			vec_float_p1_t V3
		)
		{
			// [ ((v2.z*v3.w-v2.w*v3.z)*v1.y)-((v2.y*v3.w-v2.w*v3.y)*v1.z)+((v2.y*v3.z-v2.z*v3.y)*v1.w),
			//   ((v2.w*v3.z-v2.z*v3.w)*v1.x)-((v2.w*v3.x-v2.x*v3.w)*v1.z)+((v2.z*v3.x-v2.x*v3.z)*v1.w),
			//   ((v2.y*v3.w-v2.w*v3.y)*v1.x)-((v2.x*v3.w-v2.w*v3.x)*v1.y)+((v2.x*v3.y-v2.y*v3.x)*v1.w),
			//   ((v2.z*v3.y-v2.y*v3.z)*v1.x)-((v2.z*v3.x-v2.x*v3.z)*v1.y)+((v2.y*v3.x-v2.x*v3.y)*v1.z) ]

#if defined(LUNA_PLATFORM_ARM_NEON)
			const float32x2_t select = vget_low_f32(g_MaskX);

			// Term1: V2zwyz * V3wzwy
			const float32x2_t v2xy = vget_low_f32(V2);
			const float32x2_t v2zw = vget_high_f32(V2);
			const float32x2_t v2yx = vrev64_f32(v2xy);
			const float32x2_t v2wz = vrev64_f32(v2zw);
			const float32x2_t v2yz = vbsl_f32(select, v2yx, v2wz);

			const float32x2_t v3zw = vget_high_f32(V3);
			const float32x2_t v3wz = vrev64_f32(v3zw);
			const float32x2_t v3xy = vget_low_f32(V3);
			const float32x2_t v3wy = vbsl_f32(select, v3wz, v3xy);

			float32x4_t vTemp1 = vcombine_f32(v2zw, v2yz);
			float32x4_t vTemp2 = vcombine_f32(v3wz, v3wy);
			vec_float_t vResult = vmulq_f32(vTemp1, vTemp2);

			// - V2wzwy * V3zwyz
			const float32x2_t v2wy = vbsl_f32(select, v2wz, v2xy);

			const float32x2_t v3yx = vrev64_f32(v3xy);
			const float32x2_t v3yz = vbsl_f32(select, v3yx, v3wz);

			vTemp1 = vcombine_f32(v2wz, v2wy);
			vTemp2 = vcombine_f32(v3zw, v3yz);
			vResult = vmlsq_f32(vResult, vTemp1, vTemp2);

			// term1 * V1yxxx
			const float32x2_t v1xy = vget_low_f32(V1);
			const float32x2_t v1yx = vrev64_f32(v1xy);

			vTemp1 = vcombine_f32(v1yx, vdup_lane_f32(v1yx, 1));
			vResult = vmulq_f32(vResult, vTemp1);

			// Term2: V2ywxz * V3wxwx
			const float32x2_t v2yw = vrev64_f32(v2wy);
			const float32x2_t v2xz = vbsl_f32(select, v2xy, v2wz);

			const float32x2_t v3wx = vbsl_f32(select, v3wz, v3yx);

			vTemp1 = vcombine_f32(v2yw, v2xz);
			vTemp2 = vcombine_f32(v3wx, v3wx);
			float32x4_t vTerm = vmulq_f32(vTemp1, vTemp2);

			// - V2wxwx * V3ywxz
			const float32x2_t v2wx = vbsl_f32(select, v2wz, v2yx);

			const float32x2_t v3yw = vrev64_f32(v3wy);
			const float32x2_t v3xz = vbsl_f32(select, v3xy, v3wz);

			vTemp1 = vcombine_f32(v2wx, v2wx);
			vTemp2 = vcombine_f32(v3yw, v3xz);
			vTerm = vmlsq_f32(vTerm, vTemp1, vTemp2);

			// vResult - term2 * V1zzyy
			const float32x2_t v1zw = vget_high_f32(V1);

			vTemp1 = vcombine_f32(vdup_lane_f32(v1zw, 0), vdup_lane_f32(v1yx, 0));
			vResult = vmlsq_f32(vResult, vTerm, vTemp1);

			// Term3: V2yzxy * V3zxyx
			const float32x2_t v3zx = vrev64_f32(v3xz);

			vTemp1 = vcombine_f32(v2yz, v2xy);
			vTemp2 = vcombine_f32(v3zx, v3yx);
			vTerm = vmulq_f32(vTemp1, vTemp2);

			// - V2zxyx * V3yzxy
			const float32x2_t v2zx = vrev64_f32(v2xz);

			vTemp1 = vcombine_f32(v2zx, v2yx);
			vTemp2 = vcombine_f32(v3yz, v3xy);
			vTerm = vmlsq_f32(vTerm, vTemp1, vTemp2);

			// vResult + term3 * V1wwwz
			const float32x2_t v1wz = vrev64_f32(v1zw);

			vTemp1 = vcombine_f32(vdup_lane_f32(v1wz, 0), v1wz);
			return vmlaq_f32(vResult, vTerm, vTemp1);
#elif defined(LUNA_PLATFORM_SSE2)
	// V2zwyz * V3wzwy
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 1, 3, 2));
			vec_float_t vTemp3 = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 3, 2, 3));
			vResult = _mm_mul_ps(vResult, vTemp3);
			// - V2wzwy * V3zwyz
			vec_float_t vTemp2 = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 3, 2, 3));
			vTemp3 = LUNA_SIMD_PERMUTE_PS(vTemp3, _MM_SHUFFLE(1, 3, 0, 1));
			vTemp2 = _mm_mul_ps(vTemp2, vTemp3);
			vResult = _mm_sub_ps(vResult, vTemp2);
			// term1 * V1yxxx
			vec_float_t vTemp1 = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 0, 0, 1));
			vResult = _mm_mul_ps(vResult, vTemp1);

			// V2ywxz * V3wxwx
			vTemp2 = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 0, 3, 1));
			vTemp3 = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 3, 0, 3));
			vTemp3 = _mm_mul_ps(vTemp3, vTemp2);
			// - V2wxwx * V3ywxz
			vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp2, _MM_SHUFFLE(2, 1, 2, 1));
			vTemp1 = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(2, 0, 3, 1));
			vTemp2 = _mm_mul_ps(vTemp2, vTemp1);
			vTemp3 = _mm_sub_ps(vTemp3, vTemp2);
			// vResult - temp * V1zzyy
			vTemp1 = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(1, 1, 2, 2));
			vTemp1 = _mm_mul_ps(vTemp1, vTemp3);
			vResult = _mm_sub_ps(vResult, vTemp1);

			// V2yzxy * V3zxyx
			vTemp2 = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(1, 0, 2, 1));
			vTemp3 = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(0, 1, 0, 2));
			vTemp3 = _mm_mul_ps(vTemp3, vTemp2);
			// - V2zxyx * V3yzxy
			vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp2, _MM_SHUFFLE(2, 0, 2, 1));
			vTemp1 = LUNA_SIMD_PERMUTE_PS(V3, _MM_SHUFFLE(1, 0, 2, 1));
			vTemp1 = _mm_mul_ps(vTemp1, vTemp2);
			vTemp3 = _mm_sub_ps(vTemp3, vTemp1);
			// vResult + term * V1wwwz
			vTemp1 = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 3, 3, 3));
			vTemp3 = _mm_mul_ps(vTemp3, vTemp1);
			vResult = _mm_add_ps(vResult, vTemp3);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4LengthSq
		(
			vec_float_p1_t V
		)
		{
			return Vector4Dot(V, V);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4ReciprocalLengthEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot4
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			// Reciprocal sqrt (estimate)
			v2 = vrsqrte_f32(v1);
			return vcombine_f32(v2, v2);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0xff);
			return _mm_rsqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_rsqrt_ps(vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and w
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
			// x+z, y+w
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// x+z,x+z,x+z,y+w
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
			// ??,??,y+w,y+w
			vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
			// ??,??,x+z+y+w,??
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// Splat the length
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
			// Get the reciprocal
			vLengthSq = _mm_rsqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4ReciprocalLength
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot4
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			// Reciprocal sqrt
			float32x2_t  S0 = vrsqrte_f32(v1);
			float32x2_t  P0 = vmul_f32(v1, S0);
			float32x2_t  R0 = vrsqrts_f32(P0, S0);
			float32x2_t  S1 = vmul_f32(S0, R0);
			float32x2_t  P1 = vmul_f32(v1, S1);
			float32x2_t  R1 = vrsqrts_f32(P1, S1);
			float32x2_t Result = vmul_f32(S1, R1);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0xff);
			vec_float_t vLengthSq = _mm_sqrt_ps(vTemp);
			return _mm_div_ps(g_One, vLengthSq);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			vLengthSq = _mm_div_ps(g_One, vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and w
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
			// x+z, y+w
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// x+z,x+z,x+z,y+w
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
			// ??,??,y+w,y+w
			vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
			// ??,??,x+z+y+w,??
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// Splat the length
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
			// Get the reciprocal
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			// Accurate!
			vLengthSq = _mm_div_ps(g_One, vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4LengthEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot4
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			const float32x2_t zero = vdup_n_f32(0);
			uint32x2_t VEqualsZero = vceq_f32(v1, zero);
			// Sqrt (estimate)
			float32x2_t Result = vrsqrte_f32(v1);
			Result = vmul_f32(v1, Result);
			Result = vbsl_f32(VEqualsZero, zero, Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0xff);
			return _mm_sqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and w
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
			// x+z, y+w
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// x+z,x+z,x+z,y+w
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
			// ??,??,y+w,y+w
			vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
			// ??,??,x+z+y+w,??
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// Splat the length
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
			// Get the length
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Length
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot4
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			const float32x2_t zero = vdup_n_f32(0);
			uint32x2_t VEqualsZero = vceq_f32(v1, zero);
			// Sqrt
			float32x2_t S0 = vrsqrte_f32(v1);
			float32x2_t P0 = vmul_f32(v1, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(v1, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			float32x2_t Result = vmul_f32(S1, R1);
			Result = vmul_f32(v1, Result);
			Result = vbsl_f32(VEqualsZero, zero, Result);
			return vcombine_f32(Result, Result);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0xff);
			return _mm_sqrt_ps(vTemp);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and w
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
			// x+z, y+w
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// x+z,x+z,x+z,y+w
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
			// ??,??,y+w,y+w
			vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
			// ??,??,x+z+y+w,??
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// Splat the length
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
			// Get the length
			vLengthSq = _mm_sqrt_ps(vLengthSq);
			return vLengthSq;
#endif
		}

		//------------------------------------------------------------------------------
		// Vector4NormalizeEst uses a reciprocal estimate and
		// returns QNaN on zero and infinite vectors.

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4NormalizeEst
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot4
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			// Reciprocal sqrt (estimate)
			v2 = vrsqrte_f32(v1);
			// normalize
			return vmulq_f32(V, vcombine_f32(v2, v2));
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vTemp = _mm_dp_ps(V, V, 0xff);
			vec_float_t vResult = _mm_rsqrt_ps(vTemp);
			return _mm_mul_ps(vResult, V);
#elif defined(LUNA_PLATFORM_SSE3)
			vec_float_t vDot = _mm_mul_ps(V, V);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_hadd_ps(vDot, vDot);
			vDot = _mm_rsqrt_ps(vDot);
			vDot = _mm_mul_ps(vDot, V);
			return vDot;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and w
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
			// x+z, y+w
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// x+z,x+z,x+z,y+w
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
			// ??,??,y+w,y+w
			vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
			// ??,??,x+z+y+w,??
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// Splat the length
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
			// Get the reciprocal
			vec_float_t vResult = _mm_rsqrt_ps(vLengthSq);
			// Reciprocal mul to perform the normalization
			vResult = _mm_mul_ps(vResult, V);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Normalize
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Dot4
			float32x4_t vTemp = vmulq_f32(V, V);
			float32x2_t v1 = vget_low_f32(vTemp);
			float32x2_t v2 = vget_high_f32(vTemp);
			v1 = vadd_f32(v1, v2);
			v1 = vpadd_f32(v1, v1);
			uint32x2_t VEqualsZero = vceq_f32(v1, vdup_n_f32(0));
			uint32x2_t VEqualsInf = vceq_f32(v1, vget_low_f32(g_Infinity));
			// Reciprocal sqrt (2 iterations of Newton-Raphson)
			float32x2_t S0 = vrsqrte_f32(v1);
			float32x2_t P0 = vmul_f32(v1, S0);
			float32x2_t R0 = vrsqrts_f32(P0, S0);
			float32x2_t S1 = vmul_f32(S0, R0);
			float32x2_t P1 = vmul_f32(v1, S1);
			float32x2_t R1 = vrsqrts_f32(P1, S1);
			v2 = vmul_f32(S1, R1);
			// normalize
			vec_float_t vResult = vmulq_f32(V, vcombine_f32(v2, v2));
			vResult = vbslq_f32(vcombine_f32(VEqualsZero, VEqualsZero), vdupq_n_f32(0), vResult);
			return vbslq_f32(vcombine_f32(VEqualsInf, VEqualsInf), g_QNaN, vResult);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float_t vLengthSq = _mm_dp_ps(V, V, 0xff);
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Divide to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE3)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			vLengthSq = _mm_hadd_ps(vLengthSq, vLengthSq);
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Divide to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y,z and w
			vec_float_t vLengthSq = _mm_mul_ps(V, V);
			// vTemp has z and w
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(3, 2, 3, 2));
			// x+z, y+w
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// x+z,x+z,x+z,y+w
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(1, 0, 0, 0));
			// ??,??,y+w,y+w
			vTemp = _mm_shuffle_ps(vTemp, vLengthSq, _MM_SHUFFLE(3, 3, 0, 0));
			// ??,??,x+z+y+w,??
			vLengthSq = _mm_add_ps(vLengthSq, vTemp);
			// Splat the length
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 2, 2, 2));
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Create zero with a single instruction
			vec_float_t vZeroMask = _mm_setzero_ps();
			// Test for a divide by zero (Must be FP to detect -0.0)
			vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Divide to perform the normalization
			vResult = _mm_div_ps(V, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vZeroMask);
			// Select qnan or result based on infinite length
			vec_float_t vTemp1 = _mm_andnot_ps(vLengthSq, g_QNaN);
			vec_float_t vTemp2 = _mm_and_ps(vResult, vLengthSq);
			vResult = _mm_or_ps(vTemp1, vTemp2);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4ClampLength
		(
			vec_float_p1_t V,
			f32    LengthMin,
			f32    LengthMax
		)
		{
			vec_float_t ClampMax = vector_replicate(LengthMax);
			vec_float_t ClampMin = vector_replicate(LengthMin);

			return Vector4ClampLengthV(V, ClampMin, ClampMax);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4ClampLengthV
		(
			vec_float_p1_t V,
			vec_float_p1_t LengthMin,
			vec_float_p1_t LengthMax
		)
		{
			luassert((VectorGetY(LengthMin) == VectorGetX(LengthMin)) && (VectorGetZ(LengthMin) == VectorGetX(LengthMin)) && (VectorGetW(LengthMin) == VectorGetX(LengthMin)));
			luassert((VectorGetY(LengthMax) == VectorGetX(LengthMax)) && (VectorGetZ(LengthMax) == VectorGetX(LengthMax)) && (VectorGetW(LengthMax) == VectorGetX(LengthMax)));
			luassert(Vector4GreaterOrEqual(LengthMin, vector_zero()));
			luassert(Vector4GreaterOrEqual(LengthMax, vector_zero()));
			luassert(Vector4GreaterOrEqual(LengthMax, LengthMin));

			vec_float_t LengthSq = Vector4LengthSq(V);

			const vec_float_t Zero = vector_zero();

			vec_float_t RcpLength = VectorReciprocalSqrt(LengthSq);

			vec_float_t InfiniteLength = VectorEqualInt(LengthSq, g_Infinity.v);
			vec_float_t ZeroLength = VectorEqual(LengthSq, Zero);

			vec_float_t Normal = VectorMultiply(V, RcpLength);

			vec_float_t length = VectorMultiply(LengthSq, RcpLength);

			vec_float_t Select = VectorEqualInt(InfiniteLength, ZeroLength);
			length = VectorSelect(LengthSq, length, Select);
			Normal = VectorSelect(LengthSq, Normal, Select);

			vec_float_t ControlMax = VectorGreater(length, LengthMax);
			vec_float_t ControlMin = VectorLess(length, LengthMin);

			vec_float_t ClampLength = VectorSelect(length, LengthMax, ControlMax);
			ClampLength = VectorSelect(ClampLength, LengthMin, ControlMin);

			vec_float_t Result = VectorMultiply(Normal, ClampLength);

			// Preserve the original vector (with no precision loss) if the length falls within the given range
			vec_float_t Control = VectorEqualInt(ControlMax, ControlMin);
			Result = VectorSelect(Result, V, Control);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Reflect
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal
		)
		{
			// Result = Incident - (2 * dot(Incident, Normal)) * Normal

			vec_float_t Result = Vector4Dot(Incident, Normal);
			Result = VectorAdd(Result, Result);
			Result = VectorNegativeMultiplySubtract(Result, Normal, Incident);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Refract
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal,
			f32    RefractionIndex
		)
		{
			vec_float_t Index = vector_replicate(RefractionIndex);
			return Vector4RefractV(Incident, Normal, Index);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4RefractV
		(
			vec_float_p1_t Incident,
			vec_float_p1_t Normal,
			vec_float_p1_t RefractionIndex
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t IDotN = Vector4Dot(Incident, Normal);

			// R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
			float32x4_t R = vmlsq_f32(g_One, IDotN, IDotN);
			R = vmulq_f32(R, RefractionIndex);
			R = vmlsq_f32(g_One, R, RefractionIndex);

			uint32x4_t vResult = vcleq_f32(R, g_Zero);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vResult), vget_high_u8(vResult));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			if (vget_lane_u32(vTemp.val[1], 1) == 0xFFFFFFFFU)
			{
				// Total internal reflection
				vResult = g_Zero;
			}
			else
			{
				// Sqrt(R)
				float32x4_t S0 = vrsqrteq_f32(R);
				float32x4_t P0 = vmulq_f32(R, S0);
				float32x4_t R0 = vrsqrtsq_f32(P0, S0);
				float32x4_t S1 = vmulq_f32(S0, R0);
				float32x4_t P1 = vmulq_f32(R, S1);
				float32x4_t R1 = vrsqrtsq_f32(P1, S1);
				float32x4_t S2 = vmulq_f32(S1, R1);
				R = vmulq_f32(R, S2);
				// R = RefractionIndex * IDotN + sqrt(R)
				R = vmlaq_f32(R, RefractionIndex, IDotN);
				// Result = RefractionIndex * Incident - Normal * R
				vResult = vmulq_f32(RefractionIndex, Incident);
				vResult = vmlsq_f32(vResult, R, Normal);
			}
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t IDotN = Vector4Dot(Incident, Normal);

			// R = 1.0f - RefractionIndex * RefractionIndex * (1.0f - IDotN * IDotN)
			vec_float_t R = _mm_mul_ps(IDotN, IDotN);
			R = _mm_sub_ps(g_One, R);
			R = _mm_mul_ps(R, RefractionIndex);
			R = _mm_mul_ps(R, RefractionIndex);
			R = _mm_sub_ps(g_One, R);

			vec_float_t vResult = _mm_cmple_ps(R, g_Zero);
			if (_mm_movemask_ps(vResult) == 0x0f)
			{
				// Total internal reflection
				vResult = g_Zero;
			}
			else
			{
				// R = RefractionIndex * IDotN + sqrt(R)
				R = _mm_sqrt_ps(R);
				vResult = _mm_mul_ps(RefractionIndex, IDotN);
				R = _mm_add_ps(R, vResult);
				// Result = RefractionIndex * Incident - Normal * R
				vResult = _mm_mul_ps(RefractionIndex, Incident);
				R = _mm_mul_ps(R, Normal);
				vResult = _mm_sub_ps(vResult, R);
			}
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Orthogonal
		(
			vec_float_p1_t V
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t Negate = { { { 1.f, 1.f, -1.f, -1.f } } };

			float32x4_t Result = vcombine_f32(vget_high_f32(V), vget_low_f32(V));
			return vmulq_f32(Result, Negate);
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t FlipZW = { { { 1.0f, 1.0f, -1.0f, -1.0f } } };
			vec_float_t vResult = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 0, 3, 2));
			vResult = _mm_mul_ps(vResult, FlipZW);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4AngleBetweenNormalsEst
		(
			vec_float_p1_t N1,
			vec_float_p1_t N2
		)
		{
			vec_float_t Result = Vector4Dot(N1, N2);
			Result = VectorClamp(Result, g_NegativeOne.v, g_One.v);
			Result = VectorACosEst(Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4AngleBetweenNormals
		(
			vec_float_p1_t N1,
			vec_float_p1_t N2
		)
		{
			vec_float_t Result = Vector4Dot(N1, N2);
			Result = VectorClamp(Result, g_NegativeOne.v, g_One.v);
			Result = VectorACos(Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4AngleBetweenVectors
		(
			vec_float_p1_t V1,
			vec_float_p1_t V2
		)
		{
			vec_float_t L1 = Vector4ReciprocalLength(V1);
			vec_float_t L2 = Vector4ReciprocalLength(V2);

			vec_float_t dot = Vector4Dot(V1, V2);

			L1 = VectorMultiply(L1, L2);

			vec_float_t CosAngle = VectorMultiply(dot, L1);
			CosAngle = VectorClamp(CosAngle, g_NegativeOne.v, g_One.v);

			return VectorACos(CosAngle);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV Vector4Transform
		(
			vec_float_p1_t V,
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vec_float_t vResult = vmulq_lane_f32(M.r[0], VL, 0); // X
			vResult = vmlaq_lane_f32(vResult, M.r[1], VL, 1); // Y
			float32x2_t VH = vget_high_f32(V);
			vResult = vmlaq_lane_f32(vResult, M.r[2], VH, 0); // Z
			return vmlaq_lane_f32(vResult, M.r[3], VH, 1); // W
#elif defined(LUNA_PLATFORM_SSE2)
			// Splat x,y,z and w
			vec_float_t vTempX = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
			vec_float_t vTempY = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vec_float_t vTempZ = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			vec_float_t vTempW = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));
			// Mul by the matrix
			vTempX = _mm_mul_ps(vTempX, M.r[0]);
			vTempY = _mm_mul_ps(vTempY, M.r[1]);
			vTempZ = _mm_mul_ps(vTempZ, M.r[2]);
			vTempW = _mm_mul_ps(vTempW, M.r[3]);
			// Add them all together
			vTempX = _mm_add_ps(vTempX, vTempY);
			vTempZ = _mm_add_ps(vTempZ, vTempW);
			vTempX = _mm_add_ps(vTempX, vTempZ);
			return vTempX;
#endif
		}

		//------------------------------------------------------------------------------

		inline f32* LUNA_SIMD_CALLCONV Vector4TransformStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          VectorCount,
			mat_float_p1_t       M
		)
		{
			luassert(pOutputStream != nullptr);
			luassert(pInputStream != nullptr);

			luassert(InputStride >= 16);

			luassert(OutputStride >= 16);

#if defined(LUNA_PLATFORM_ARM_NEON)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];
			const vec_float_t row3 = M.r[3];

			usize i = 0;
			usize four = VectorCount >> 2;
			if (four > 0)
			{
				if ((InputStride == 16) && (OutputStride == 16))
				{
					for (usize j = 0; j < four; ++j)
					{
						float32x4x4_t V = vld4q_f32(reinterpret_cast<const float*>(pInputVector));
						pInputVector += 16 * 4;

						float32x2_t r = vget_low_f32(row0);
						vec_float_t vResult0 = vmulq_lane_f32(V.val[0], r, 0); // Ax
						vec_float_t vResult1 = vmulq_lane_f32(V.val[0], r, 1); // Bx

						__prefetch(pInputVector);

						r = vget_high_f32(row0);
						vec_float_t vResult2 = vmulq_lane_f32(V.val[0], r, 0); // Cx
						vec_float_t vResult3 = vmulq_lane_f32(V.val[0], r, 1); // Dx

						__prefetch(pInputVector + g_cache_line_size);

						r = vget_low_f32(row1);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[1], r, 0); // Ax+Ey
						vResult1 = vmlaq_lane_f32(vResult1, V.val[1], r, 1); // Bx+Fy

						__prefetch(pInputVector + (g_cache_line_size * 2));

						r = vget_high_f32(row1);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[1], r, 0); // Cx+Gy
						vResult3 = vmlaq_lane_f32(vResult3, V.val[1], r, 1); // Dx+Hy

						__prefetch(pInputVector + (g_cache_line_size * 3));

						r = vget_low_f32(row2);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[2], r, 0); // Ax+Ey+Iz
						vResult1 = vmlaq_lane_f32(vResult1, V.val[2], r, 1); // Bx+Fy+Jz

						__prefetch(pInputVector + (g_cache_line_size * 4));

						r = vget_high_f32(row2);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[2], r, 0); // Cx+Gy+Kz
						vResult3 = vmlaq_lane_f32(vResult3, V.val[2], r, 1); // Dx+Hy+Lz

						__prefetch(pInputVector + (g_cache_line_size * 5));

						r = vget_low_f32(row3);
						vResult0 = vmlaq_lane_f32(vResult0, V.val[3], r, 0); // Ax+Ey+Iz+Mw
						vResult1 = vmlaq_lane_f32(vResult1, V.val[3], r, 1); // Bx+Fy+Jz+Nw

						__prefetch(pInputVector + (g_cache_line_size * 6));

						r = vget_high_f32(row3);
						vResult2 = vmlaq_lane_f32(vResult2, V.val[3], r, 0); // Cx+Gy+Kz+Ow
						vResult3 = vmlaq_lane_f32(vResult3, V.val[3], r, 1); // Dx+Hy+Lz+Pw

						__prefetch(pInputVector + (g_cache_line_size * 7));

						V.val[0] = vResult0;
						V.val[1] = vResult1;
						V.val[2] = vResult2;
						V.val[3] = vResult3;

						vst4q_f32(reinterpret_cast<float*>(pOutputVector), V);
						pOutputVector += 16 * 4;

						i += 4;
					}
				}
			}

			for (; i < VectorCount; i++)
			{
				vec_float_t V = vld1q_f32(reinterpret_cast<const float*>(pInputVector));
				pInputVector += InputStride;

				float32x2_t VL = vget_low_f32(V);
				vec_float_t vResult = vmulq_lane_f32(row0, VL, 0); // X
				vResult = vmlaq_lane_f32(vResult, row1, VL, 1); // Y
				float32x2_t VH = vget_high_f32(V);
				vResult = vmlaq_lane_f32(vResult, row2, VH, 0); // Z
				vResult = vmlaq_lane_f32(vResult, row3, VH, 1); // W

				vst1q_f32(reinterpret_cast<float*>(pOutputVector), vResult);
				pOutputVector += OutputStride;
			}

			return pOutputStream;
#elif defined(LUNA_PLATFORM_SSE2)
			auto pInputVector = reinterpret_cast<const uint8_t*>(pInputStream);
			auto pOutputVector = reinterpret_cast<uint8_t*>(pOutputStream);

			const vec_float_t row0 = M.r[0];
			const vec_float_t row1 = M.r[1];
			const vec_float_t row2 = M.r[2];
			const vec_float_t row3 = M.r[3];

			if (!(reinterpret_cast<uintptr_t>(pOutputStream) & 0xF) && !(OutputStride & 0xF))
			{
				if (!(reinterpret_cast<uintptr_t>(pInputStream) & 0xF) && !(InputStride & 0xF))
				{
					// Aligned input, aligned output
					for (usize i = 0; i < VectorCount; i++)
					{
						__m128 V = _mm_load_ps(reinterpret_cast<const float*>(pInputVector));
						pInputVector += InputStride;

						vec_float_t vTempX = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
						vec_float_t vTempY = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
						vec_float_t vTempZ = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
						vec_float_t vTempW = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));

						vTempX = _mm_mul_ps(vTempX, row0);
						vTempY = _mm_mul_ps(vTempY, row1);
						vTempZ = _mm_mul_ps(vTempZ, row2);
						vTempW = _mm_mul_ps(vTempW, row3);

						vTempX = _mm_add_ps(vTempX, vTempY);
						vTempZ = _mm_add_ps(vTempZ, vTempW);
						vTempX = _mm_add_ps(vTempX, vTempZ);

						LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTempX);
						pOutputVector += OutputStride;
					}
				}
				else
				{
					// Unaligned input, aligned output
					for (usize i = 0; i < VectorCount; i++)
					{
						__m128 V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
						pInputVector += InputStride;

						vec_float_t vTempX = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
						vec_float_t vTempY = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
						vec_float_t vTempZ = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
						vec_float_t vTempW = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));

						vTempX = _mm_mul_ps(vTempX, row0);
						vTempY = _mm_mul_ps(vTempY, row1);
						vTempZ = _mm_mul_ps(vTempZ, row2);
						vTempW = _mm_mul_ps(vTempW, row3);

						vTempX = _mm_add_ps(vTempX, vTempY);
						vTempZ = _mm_add_ps(vTempZ, vTempW);
						vTempX = _mm_add_ps(vTempX, vTempZ);

						LUNA_SIMD_STREAM_PS(reinterpret_cast<float*>(pOutputVector), vTempX);
						pOutputVector += OutputStride;
					}
				}
			}
			else
			{
				if (!(reinterpret_cast<uintptr_t>(pInputStream) & 0xF) && !(InputStride & 0xF))
				{
					// Aligned input, unaligned output
					for (usize i = 0; i < VectorCount; i++)
					{
						__m128 V = _mm_load_ps(reinterpret_cast<const float*>(pInputVector));
						pInputVector += InputStride;

						vec_float_t vTempX = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
						vec_float_t vTempY = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
						vec_float_t vTempZ = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
						vec_float_t vTempW = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));

						vTempX = _mm_mul_ps(vTempX, row0);
						vTempY = _mm_mul_ps(vTempY, row1);
						vTempZ = _mm_mul_ps(vTempZ, row2);
						vTempW = _mm_mul_ps(vTempW, row3);

						vTempX = _mm_add_ps(vTempX, vTempY);
						vTempZ = _mm_add_ps(vTempZ, vTempW);
						vTempX = _mm_add_ps(vTempX, vTempZ);

						_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTempX);
						pOutputVector += OutputStride;
					}
				}
				else
				{
					// Unaligned input, unaligned output
					for (usize i = 0; i < VectorCount; i++)
					{
						__m128 V = _mm_loadu_ps(reinterpret_cast<const float*>(pInputVector));
						pInputVector += InputStride;

						vec_float_t vTempX = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(0, 0, 0, 0));
						vec_float_t vTempY = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
						vec_float_t vTempZ = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
						vec_float_t vTempW = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(3, 3, 3, 3));

						vTempX = _mm_mul_ps(vTempX, row0);
						vTempY = _mm_mul_ps(vTempY, row1);
						vTempZ = _mm_mul_ps(vTempZ, row2);
						vTempW = _mm_mul_ps(vTempW, row3);

						vTempX = _mm_add_ps(vTempX, vTempY);
						vTempZ = _mm_add_ps(vTempZ, vTempW);
						vTempX = _mm_add_ps(vTempX, vTempZ);

						_mm_storeu_ps(reinterpret_cast<float*>(pOutputVector), vTempX);
						pOutputVector += OutputStride;
					}
				}
			}

			LUNA_SIMD_SFENCE();

			return pOutputStream;
#endif
		}

	}
}

#endif // LUNA_SIMD_ENABLED