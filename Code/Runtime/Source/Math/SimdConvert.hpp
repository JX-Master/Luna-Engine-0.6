// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file SimdConvert.hpp
* @author JxMaster
* @date 2019/1/4
* @brief Multi-platform SIMD Library modified from Microsoft's DirectX Math.
* See: https://github.com/Microsoft/DirectXMath
 */
#pragma once

#include "Simd.hpp"
#include "../../Assert.hpp"

#if defined(LUNA_SIMD_ENABLED)

namespace Luna
{
	namespace Simd
	{
		inline vec_float_t LUNA_SIMD_CALLCONV convert_vector_int_to_float
		(
			vec_float_p1_t    VInt,
			u32     DivExponent
		)
		{
			luassert(DivExponent < 32);
#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 fScale = 1.0f / (float)(1U << DivExponent);
			float32x4_t vResult = vcvtq_f32_s32(VInt);
			return vmulq_n_f32(vResult, fScale);
#else // LUNA_PLATFORM_SSE2
			// Convert to floats
			vec_float_t vResult = _mm_cvtepi32_ps(_mm_castps_si128(VInt));
			// Convert DivExponent into 1.0f/(1<<DivExponent)
			u32 uScale = 0x3F800000U - (DivExponent << 23);
			// Splat the scalar value
			__m128i vScale = _mm_set1_epi32(static_cast<int>(uScale));
			vResult = _mm_mul_ps(vResult, _mm_castsi128_ps(vScale));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV convert_vector_float_to_int
		(
			vec_float_p1_t    VFloat,
			u32     MulExponent
		)
		{
			luassert(MulExponent < 32);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t vResult = vmulq_n_f32(VFloat, (float)(1U << MulExponent));
			// In case of positive overflow, detect it
			uint32x4_t vOverflow = vcgtq_f32(vResult, g_MaxInt);
			// Float to int conversion
			int32x4_t vResulti = vcvtq_s32_f32(vResult);
			// If there was positive overflow, set to 0x7FFFFFFF
			vResult = vandq_u32(vOverflow, g_AbsMask);
			vOverflow = vbicq_u32(vResulti, vOverflow);
			vOverflow = vorrq_u32(vOverflow, vResult);
			return vOverflow;
#else // LUNA_PLATFORM_SSE2
			vec_float_t vResult = _mm_set_ps1(static_cast<f32>(1U << MulExponent));
			vResult = _mm_mul_ps(vResult, VFloat);
			// In case of positive overflow, detect it
			vec_float_t vOverflow = _mm_cmpgt_ps(vResult, g_MaxInt);
			// Float to int conversion
			__m128i vResulti = _mm_cvttps_epi32(vResult);
			// If there was positive overflow, set to 0x7FFFFFFF
			vResult = _mm_and_ps(vOverflow, g_AbsMask);
			vOverflow = _mm_andnot_ps(vOverflow, _mm_castsi128_ps(vResulti));
			vOverflow = _mm_or_ps(vOverflow, vResult);
			return vOverflow;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV convert_vector_uint_to_float
		(
			vec_float_p1_t     VUInt,
			u32      DivExponent
		)
		{
			luassert(DivExponent < 32);
#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 fScale = 1.0f / (float)(1U << DivExponent);
			float32x4_t vResult = vcvtq_f32_u32(VUInt);
			return vmulq_n_f32(vResult, fScale);
#else // LUNA_PLATFORM_SSE2
			// For the values that are higher than 0x7FFFFFFF, a fixup is needed
			// Determine which ones need the fix.
			vec_float_t vMask = _mm_and_ps(VUInt, g_NegativeZero);
			// Force all values positive
			vec_float_t vResult = _mm_xor_ps(VUInt, vMask);
			// Convert to floats
			vResult = _mm_cvtepi32_ps(_mm_castps_si128(vResult));
			// Convert 0x80000000 -> 0xFFFFFFFF
			__m128i iMask = _mm_srai_epi32(_mm_castps_si128(vMask), 31);
			// For only the ones that are too big, add the fixup
			vMask = _mm_and_ps(_mm_castsi128_ps(iMask), g_FixUnsigned);
			vResult = _mm_add_ps(vResult, vMask);
			// Convert DivExponent into 1.0f/(1<<DivExponent)
			u32 uScale = 0x3F800000U - (DivExponent << 23);
			// Splat
			iMask = _mm_set1_epi32(static_cast<int>(uScale));
			vResult = _mm_mul_ps(vResult, _mm_castsi128_ps(iMask));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV convert_vector_float_to_uint
		(
			vec_float_p1_t     VFloat,
			u32      MulExponent
		)
		{
			luassert(MulExponent < 32);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t vResult = vmulq_n_f32(VFloat, (float)(1U << MulExponent));
			// In case of overflow, detect it
			uint32x4_t vOverflow = vcgtq_f32(vResult, g_MaxUInt);
			// Float to int conversion
			uint32x4_t vResulti = vcvtq_u32_f32(vResult);
			// If there was overflow, set to 0xFFFFFFFFU
			vResult = vbicq_u32(vResulti, vOverflow);
			vOverflow = vorrq_u32(vOverflow, vResult);
			return vOverflow;
#else // LUNA_PLATFORM_SSE2
			vec_float_t vResult = _mm_set_ps1(static_cast<f32>(1U << MulExponent));
			vResult = _mm_mul_ps(vResult, VFloat);
			// clamp to >=0
			vResult = _mm_max_ps(vResult, g_Zero);
			// Any numbers that are too big, set to 0xFFFFFFFFU
			vec_float_t vOverflow = _mm_cmpgt_ps(vResult, g_MaxUInt);
			vec_float_t vValue = g_UnsignedFix;
			// Too large for a signed integer?
			vec_float_t vMask = _mm_cmpge_ps(vResult, vValue);
			// Zero for number's lower than 0x80000000, 32768.0f*65536.0f otherwise
			vValue = _mm_and_ps(vValue, vMask);
			// Perform fixup only on numbers too large (Keeps low bit precision)
			vResult = _mm_sub_ps(vResult, vValue);
			__m128i vResulti = _mm_cvttps_epi32(vResult);
			// Convert from signed to unsigned only if greater than 0x80000000
			vMask = _mm_and_ps(vMask, g_NegativeZero);
			vResult = _mm_xor_ps(_mm_castsi128_ps(vResulti), vMask);
			// On those that are too large, set to 0xFFFFFFFF
			vResult = _mm_or_ps(vResult, vOverflow);
			return vResult;
#endif
		}

		// #pragma warning(pop)

		/****************************************************************************
		*
		* Vector and matrix load operations
		*
		****************************************************************************/

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint(const u32* pSource)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t zero = vdupq_n_u32(0);
			return vld1q_lane_u32(pSource, zero, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_load_ss(reinterpret_cast<const float*>(pSource));
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int(const i32* pSource)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x4_t zero = vdupq_n_u32(0);
			return vld1q_lane_u32(pSource, zero, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_load_ss(reinterpret_cast<const float*>(pSource));
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float(const f32* pSource)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t zero = vdupq_n_f32(0);
			return vld1q_lane_f32(pSource, zero, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_load_ss(pSource);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint2
		(
			const u32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t x = vld1_u32(pSource);
			uint32x2_t zero = vdup_n_u32(0);
			return vcombine_u32(x, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pSource));
			__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pSource + 1));
			return _mm_unpacklo_ps(x, y);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint2a
		(
			const u32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t x = vld1_u32_ex(pSource, 64);
			uint32x2_t zero = vdup_n_u32(0);
			return vcombine_u32(x, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int2
		(
			const i32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t x = vld1_u32(pSource);
			uint32x2_t zero = vdup_n_u32(0);
			return vcombine_u32(x, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pSource));
			__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pSource + 1));
			return _mm_unpacklo_ps(x, y);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int2a
		(
			const i32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t x = vld1_u32_ex(pSource, 64);
			uint32x2_t zero = vdup_n_u32(0);
			return vcombine_u32(x, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float2
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t x = vld1_f32(reinterpret_cast<const float*>(pSource));
			float32x2_t zero = vdup_n_f32(0);
			return vcombine_f32(x, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 x = _mm_load_ss(pSource);
			__m128 y = _mm_load_ss(pSource + 1);
			return _mm_unpacklo_ps(x, y);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float2a
		(
			const f32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t x = vld1_f32_ex(reinterpret_cast<const float*>(pSource), 64);
			float32x2_t zero = vdup_n_f32(0);
			return vcombine_f32(x, zero);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint3
		(
			const u32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t x = vld1_u32(pSource);
			uint32x2_t zero = vdup_n_u32(0);
			uint32x2_t y = vld1_lane_u32(pSource + 2, zero, 0);
			return vcombine_u32(x, y);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pSource));
			__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pSource + 1));
			__m128 z = _mm_load_ss(reinterpret_cast<const float*>(pSource + 2));
			__m128 xy = _mm_unpacklo_ps(x, y);
			return _mm_movelh_ps(xy, z);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint3a
		(
			const u32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Reads an extra integer which is zero's
			uint32x4_t V = vld1q_u32_ex(pSource, 128);
			return vsetq_lane_u32(0, V, 3);
#elif defined(LUNA_PLATFORM_SSE2)
			// Reads an extra integer which is zero's
			__m128i V = _mm_load_si128(reinterpret_cast<const __m128i*>(pSource));
			V = _mm_and_si128(V, g_Mask3);
			return _mm_castsi128_ps(V);
#endif
		}
		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int3
		(
			const i32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t x = vld1_u32(pSource);
			uint32x2_t zero = vdup_n_u32(0);
			uint32x2_t y = vld1_lane_u32(pSource + 2, zero, 0);
			return vcombine_u32(x, y);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 x = _mm_load_ss(reinterpret_cast<const float*>(pSource));
			__m128 y = _mm_load_ss(reinterpret_cast<const float*>(pSource + 1));
			__m128 z = _mm_load_ss(reinterpret_cast<const float*>(pSource + 2));
			__m128 xy = _mm_unpacklo_ps(x, y);
			return _mm_movelh_ps(xy, z);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int3a
		(
			const i32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Reads an extra integer which is zero's
			uint32x4_t V = vld1q_u32_ex(pSource, 128);
			return vsetq_lane_u32(0, V, 3);
#elif defined(LUNA_PLATFORM_SSE2)
			// Reads an extra integer which is zero's
			__m128i V = _mm_load_si128(reinterpret_cast<const __m128i*>(pSource));
			V = _mm_and_si128(V, g_Mask3);
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float3
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t x = vld1_f32(reinterpret_cast<const float*>(pSource));
			float32x2_t zero = vdup_n_f32(0);
			float32x2_t y = vld1_lane_f32(reinterpret_cast<const float*>(pSource) + 2, zero, 0);
			return vcombine_f32(x, y);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 x = _mm_load_ss(pSource);
			__m128 y = _mm_load_ss(pSource + 1);
			__m128 z = _mm_load_ss(pSource + 2);
			__m128 xy = _mm_unpacklo_ps(x, y);
			return _mm_movelh_ps(xy, z);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float3a
		(
			const f32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Reads an extra f32 which is zero's
			float32x4_t V = vld1q_f32_ex(reinterpret_cast<const float*>(pSource), 128);
			return vsetq_lane_f32(0, V, 3);
#elif defined(LUNA_PLATFORM_SSE2)
			// Reads an extra f32 which is zero's
			__m128 V = _mm_load_ps(pSource);
			return _mm_and_ps(V, g_Mask3);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint4
		(
			const u32* pSource
		)
		{
			luassert(pSource);

#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_u32(pSource);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_uint4a
		(
			const u32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_u32_ex(pSource, 128);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_load_si128(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int4
		(
			const i32* pSource
		)
		{
			luassert(pSource);

#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_u32(pSource);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_int4a
		(
			const i32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_u32_ex(pSource, 128);
#elif defined(LUNA_PLATFORM_SSE2)
			__m128i V = _mm_load_si128(reinterpret_cast<const __m128i*>(pSource));
			return _mm_castsi128_ps(V);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float4
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_f32(reinterpret_cast<const float*>(pSource));
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_loadu_ps(pSource);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV load_float4a
		(
			const f32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			return vld1q_f32_ex(reinterpret_cast<const float*>(pSource), 128);
#elif defined(LUNA_PLATFORM_SSE2)
			return _mm_load_ps(pSource);
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float3x3
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t v0 = vld1q_f32(pSource);
			float32x4_t v1 = vld1q_f32(pSource + 4);
			float32x2_t v2 = vcreate_f32(static_cast<uint64_t>(*reinterpret_cast<const u32*>(pSource + 8)));
			float32x4_t T = vextq_f32(v0, v1, 3);

			mat_float_t M;
			M.r[0] = vandq_u32(v0, g_Mask3);
			M.r[1] = vandq_u32(T, g_Mask3);
			M.r[2] = vcombine_f32(vget_high_f32(v1), v2);
			M.r[3] = g_IdentityR3;
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 Z = _mm_setzero_ps();

			__m128 V1 = _mm_loadu_ps(pSource);
			__m128 V2 = _mm_loadu_ps(pSource + 4);
			__m128 V3 = _mm_load_ss(pSource + 8);

			__m128 T1 = _mm_unpackhi_ps(V1, Z);
			__m128 T2 = _mm_unpacklo_ps(V2, Z);
			__m128 T3 = _mm_shuffle_ps(V3, T2, _MM_SHUFFLE(0, 1, 0, 0));
			__m128 T4 = _mm_movehl_ps(T2, T3);
			__m128 T5 = _mm_movehl_ps(Z, T1);

			mat_float_t M;
			M.r[0] = _mm_movelh_ps(V1, T1);
			M.r[1] = _mm_add_ps(T4, T5);
			M.r[2] = _mm_shuffle_ps(V2, V3, _MM_SHUFFLE(1, 0, 3, 2));
			M.r[3] = g_IdentityR3;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float4x3
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t v0 = vld1q_f32(pSource);
			float32x4_t v1 = vld1q_f32(pSource + 4);
			float32x4_t v2 = vld1q_f32(pSource + 8);

			float32x4_t T1 = vextq_f32(v0, v1, 3);
			float32x4_t T2 = vcombine_f32(vget_high_f32(v1), vget_low_f32(v2));
			float32x4_t T3 = vextq_f32(v2, v2, 1);

			mat_float_t M;
			M.r[0] = vandq_u32(v0, g_Mask3);
			M.r[1] = vandq_u32(T1, g_Mask3);
			M.r[2] = vandq_u32(T2, g_Mask3);
			M.r[3] = vsetq_lane_f32(1.f, T3, 3);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			// Use unaligned load instructions to 
			// load the 12 floats
			// vTemp1 = x1,y1,z1,x2
			vec_float_t vTemp1 = _mm_loadu_ps(pSource);
			// vTemp2 = y2,z2,x3,y3
			vec_float_t vTemp2 = _mm_loadu_ps(pSource + 4);
			// vTemp4 = z3,x4,y4,z4
			vec_float_t vTemp4 = _mm_loadu_ps(pSource + 8);
			// vTemp3 = x3,y3,z3,z3
			vec_float_t vTemp3 = _mm_shuffle_ps(vTemp2, vTemp4, _MM_SHUFFLE(0, 0, 3, 2));
			// vTemp2 = y2,z2,x2,x2
			vTemp2 = _mm_shuffle_ps(vTemp2, vTemp1, _MM_SHUFFLE(3, 3, 1, 0));
			// vTemp2 = x2,y2,z2,z2
			vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp2, _MM_SHUFFLE(1, 1, 0, 2));
			// vTemp1 = x1,y1,z1,0
			vTemp1 = _mm_and_ps(vTemp1, g_Mask3);
			// vTemp2 = x2,y2,z2,0
			vTemp2 = _mm_and_ps(vTemp2, g_Mask3);
			// vTemp3 = x3,y3,z3,0
			vTemp3 = _mm_and_ps(vTemp3, g_Mask3);
			// vTemp4i = x4,y4,z4,0
			__m128i vTemp4i = _mm_srli_si128(_mm_castps_si128(vTemp4), 32 / 8);
			// vTemp4i = x4,y4,z4,1.0f
			vTemp4i = _mm_or_si128(vTemp4i, g_IdentityR3);
			mat_float_t M(vTemp1,
				vTemp2,
				vTemp3,
				_mm_castsi128_ps(vTemp4i));
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float4x3a
		(
			const f32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t v0 = vld1q_f32_ex(pSource, 128);
			float32x4_t v1 = vld1q_f32_ex(pSource + 4, 128);
			float32x4_t v2 = vld1q_f32_ex(pSource + 8, 128);

			float32x4_t T1 = vextq_f32(v0, v1, 3);
			float32x4_t T2 = vcombine_f32(vget_high_f32(v1), vget_low_f32(v2));
			float32x4_t T3 = vextq_f32(v2, v2, 1);

			mat_float_t M;
			M.r[0] = vandq_u32(v0, g_Mask3);
			M.r[1] = vandq_u32(T1, g_Mask3);
			M.r[2] = vandq_u32(T2, g_Mask3);
			M.r[3] = vsetq_lane_f32(1.f, T3, 3);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			// Use aligned load instructions to 
			// load the 12 floats
			// vTemp1 = x1,y1,z1,x2
			vec_float_t vTemp1 = _mm_load_ps(pSource);
			// vTemp2 = y2,z2,x3,y3
			vec_float_t vTemp2 = _mm_load_ps(pSource + 4);
			// vTemp4 = z3,x4,y4,z4
			vec_float_t vTemp4 = _mm_load_ps(pSource + 8);
			// vTemp3 = x3,y3,z3,z3
			vec_float_t vTemp3 = _mm_shuffle_ps(vTemp2, vTemp4, _MM_SHUFFLE(0, 0, 3, 2));
			// vTemp2 = y2,z2,x2,x2
			vTemp2 = _mm_shuffle_ps(vTemp2, vTemp1, _MM_SHUFFLE(3, 3, 1, 0));
			// vTemp2 = x2,y2,z2,z2
			vTemp2 = LUNA_SIMD_PERMUTE_PS(vTemp2, _MM_SHUFFLE(1, 1, 0, 2));
			// vTemp1 = x1,y1,z1,0
			vTemp1 = _mm_and_ps(vTemp1, g_Mask3);
			// vTemp2 = x2,y2,z2,0
			vTemp2 = _mm_and_ps(vTemp2, g_Mask3);
			// vTemp3 = x3,y3,z3,0
			vTemp3 = _mm_and_ps(vTemp3, g_Mask3);
			// vTemp4i = x4,y4,z4,0
			__m128i vTemp4i = _mm_srli_si128(_mm_castps_si128(vTemp4), 32 / 8);
			// vTemp4i = x4,y4,z4,1.0f
			vTemp4i = _mm_or_si128(vTemp4i, g_IdentityR3);
			mat_float_t M(vTemp1,
				vTemp2,
				vTemp3,
				_mm_castsi128_ps(vTemp4i));
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float3x4
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2x4_t vTemp0 = vld4_f32(pSource);
			float32x4_t vTemp1 = vld1q_f32(pSource + 8);

			float32x2_t l = vget_low_f32(vTemp1);
			float32x4_t T0 = vcombine_f32(vTemp0.val[0], l);
			float32x2_t rl = vrev64_f32(l);
			float32x4_t T1 = vcombine_f32(vTemp0.val[1], rl);

			float32x2_t h = vget_high_f32(vTemp1);
			float32x4_t T2 = vcombine_f32(vTemp0.val[2], h);
			float32x2_t rh = vrev64_f32(h);
			float32x4_t T3 = vcombine_f32(vTemp0.val[3], rh);

			mat_float_t M = {};
			M.r[0] = vandq_u32(T0, g_Mask3);
			M.r[1] = vandq_u32(T1, g_Mask3);
			M.r[2] = vandq_u32(T2, g_Mask3);
			M.r[3] = vsetq_lane_f32(1.f, T3, 3);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			M.r[0] = _mm_loadu_ps(pSource);
			M.r[1] = _mm_loadu_ps(pSource + 4);
			M.r[2] = _mm_loadu_ps(pSource + 8);
			M.r[3] = g_IdentityR3;

			// x.x,x.y,y.x,y.y
			vec_float_t vTemp1 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(1, 0, 1, 0));
			// x.z,x.w,y.z,y.w
			vec_float_t vTemp3 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(3, 2, 3, 2));
			// z.x,z.y,w.x,w.y
			vec_float_t vTemp2 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(1, 0, 1, 0));
			// z.z,z.w,w.z,w.w
			vec_float_t vTemp4 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(3, 2, 3, 2));
			mat_float_t mResult;

			// x.x,y.x,z.x,w.x
			mResult.r[0] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));
			// x.y,y.y,z.y,w.y
			mResult.r[1] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));
			// x.z,y.z,z.z,w.z
			mResult.r[2] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));
			// x.w,y.w,z.w,w.w
			mResult.r[3] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(3, 1, 3, 1));
			return mResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float3x4a
		(
			const f32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2x4_t vTemp0 = vld4_f32_ex(pSource, 128);
			float32x4_t vTemp1 = vld1q_f32_ex(pSource + 8, 128);

			float32x2_t l = vget_low_f32(vTemp1);
			float32x4_t T0 = vcombine_f32(vTemp0.val[0], l);
			float32x2_t rl = vrev64_f32(l);
			float32x4_t T1 = vcombine_f32(vTemp0.val[1], rl);

			float32x2_t h = vget_high_f32(vTemp1);
			float32x4_t T2 = vcombine_f32(vTemp0.val[2], h);
			float32x2_t rh = vrev64_f32(h);
			float32x4_t T3 = vcombine_f32(vTemp0.val[3], rh);

			mat_float_t M = {};
			M.r[0] = vandq_u32(T0, g_Mask3);
			M.r[1] = vandq_u32(T1, g_Mask3);
			M.r[2] = vandq_u32(T2, g_Mask3);
			M.r[3] = vsetq_lane_f32(1.f, T3, 3);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			M.r[0] = _mm_load_ps(pSource);
			M.r[1] = _mm_load_ps(pSource + 4);
			M.r[2] = _mm_load_ps(pSource + 8);
			M.r[3] = g_IdentityR3;

			// x.x,x.y,y.x,y.y
			vec_float_t vTemp1 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(1, 0, 1, 0));
			// x.z,x.w,y.z,y.w
			vec_float_t vTemp3 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(3, 2, 3, 2));
			// z.x,z.y,w.x,w.y
			vec_float_t vTemp2 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(1, 0, 1, 0));
			// z.z,z.w,w.z,w.w
			vec_float_t vTemp4 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(3, 2, 3, 2));
			mat_float_t mResult;

			// x.x,y.x,z.x,w.x
			mResult.r[0] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));
			// x.y,y.y,z.y,w.y
			mResult.r[1] = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));
			// x.z,y.z,z.z,w.z
			mResult.r[2] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));
			// x.w,y.w,z.w,w.w
			mResult.r[3] = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(3, 1, 3, 1));
			return mResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float4x4
		(
			const f32* pSource
		)
		{
			luassert(pSource);
#if defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t M;
			M.r[0] = vld1q_f32(reinterpret_cast<const float*>(pSource));
			M.r[1] = vld1q_f32(reinterpret_cast<const float*>(pSource + 4));
			M.r[2] = vld1q_f32(reinterpret_cast<const float*>(pSource + 8));
			M.r[3] = vld1q_f32(reinterpret_cast<const float*>(pSource + 12));
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			M.r[0] = _mm_loadu_ps(pSource);
			M.r[1] = _mm_loadu_ps(pSource + 4);
			M.r[2] = _mm_loadu_ps(pSource + 8);
			M.r[3] = _mm_loadu_ps(pSource + 12);
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV load_float4x4a
		(
			const f32* pSource
		)
		{
			luassert(pSource);
			luassert((reinterpret_cast<uintptr_t>(pSource) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t M;
			M.r[0] = vld1q_f32_ex(reinterpret_cast<const float*>(pSource), 128);
			M.r[1] = vld1q_f32_ex(reinterpret_cast<const float*>(pSource + 4), 128);
			M.r[2] = vld1q_f32_ex(reinterpret_cast<const float*>(pSource + 8), 128);
			M.r[3] = vld1q_f32_ex(reinterpret_cast<const float*>(pSource + 12), 128);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			M.r[0] = _mm_load_ps(pSource);
			M.r[1] = _mm_load_ps(pSource + 4);
			M.r[2] = _mm_load_ps(pSource + 8);
			M.r[3] = _mm_load_ps(pSource + 12);
			return M;
#endif
		}

		/****************************************************************************
		 *
		 * Vector and matrix store operations
		 *
		 ****************************************************************************/

		inline void LUNA_SIMD_CALLCONV store_uint
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_u32(pDestination, *reinterpret_cast<const uint32x4_t*>(&V), 0);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ss(reinterpret_cast<float*>(pDestination), V);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_u32(pDestination, *reinterpret_cast<const uint32x4_t*>(&V), 0);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ss(reinterpret_cast<float*>(pDestination), V);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float
		(
			f32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_lane_f32(pDestination, V, 0);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ss(pDestination, V);
#endif
		}
		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int2
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32(pDestination, VL);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[0]), V);
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[1]), T);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int2a
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32_ex(pDestination, VL, 64);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_uint2
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32(pDestination, VL);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[0]), V);
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[1]), T);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_uint2a
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32_ex(pDestination, VL, 64);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float2
		(
			f32* pDestination,
			vec_float_p1_t  V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vst1_f32(reinterpret_cast<float*>(pDestination), VL);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			_mm_store_ss(&pDestination[0], V);
			_mm_store_ss(&pDestination[1], T);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float2a
		(
			f32*   pDestination,
			vec_float_p1_t     V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vst1_f32_ex(reinterpret_cast<float*>(pDestination), VL, 64);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int3
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32(pDestination, VL);
			vst1q_lane_u32(pDestination + 2, *reinterpret_cast<const uint32x4_t*>(&V), 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T1 = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vec_float_t T2 = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_store_ss(reinterpret_cast<float*>(pDestination), V);
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[1]), T1);
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[2]), T2);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int3a
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32_ex(pDestination, VL, 64);
			vst1q_lane_u32(pDestination + 2, *reinterpret_cast<const uint32x4_t*>(&V), 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[2]), T);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_uint3
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32(pDestination, VL);
			vst1q_lane_u32(pDestination + 2, *reinterpret_cast<const uint32x4_t*>(&V), 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T1 = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vec_float_t T2 = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_store_ss(reinterpret_cast<float*>(pDestination), V);
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[1]), T1);
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[2]), T2);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_uint3a
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			uint32x2_t VL = vget_low_u32(V);
			vst1_u32_ex(pDestination, VL, 64);
			vst1q_lane_u32(pDestination + 2, *reinterpret_cast<const uint32x4_t*>(&V), 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
			_mm_store_ss(reinterpret_cast<float*>(&pDestination[2]), T);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float3
		(
			f32* pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vst1_f32(reinterpret_cast<float*>(pDestination), VL);
			vst1q_lane_f32(reinterpret_cast<float*>(pDestination) + 2, V, 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T1 = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(1, 1, 1, 1));
			vec_float_t T2 = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_store_ss(&pDestination[0], V);
			_mm_store_ss(&pDestination[1], T1);
			_mm_store_ss(&pDestination[2], T2);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float3a
		(
			f32*   pDestination,
			vec_float_p1_t     V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(V);
			vst1_f32_ex(reinterpret_cast<float*>(pDestination), VL, 64);
			vst1q_lane_f32(reinterpret_cast<float*>(pDestination) + 2, V, 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t T = LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_storel_epi64(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
			_mm_store_ss(&pDestination[2], T);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int4
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_u32(pDestination, V);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storeu_si128(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_int4a
		(
			i32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_u32_ex(pDestination, V, 128);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_si128(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_uint4
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_u32(pDestination, V);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storeu_si128(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_uint4a
		(
			u32*    pDestination,
			vec_float_p1_t V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_u32_ex(pDestination, V, 128);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_si128(reinterpret_cast<__m128i*>(pDestination), _mm_castps_si128(V));
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float4
		(
			f32 *pDestination,
			vec_float_p1_t  V
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_f32(reinterpret_cast<float*>(pDestination), V);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storeu_ps(&pDestination[0], V);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float4a
		(
			f32*   pDestination,
			vec_float_p1_t     V
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_f32_ex(reinterpret_cast<float*>(pDestination), V, 128);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ps(&pDestination[0], V);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float3x3
		(
			f32*	pDestination,
			mat_float_p1_t	M
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t T1 = vextq_f32(M.r[0], M.r[1], 1);
			float32x4_t T2 = vbslq_f32(g_Mask3, M.r[0], T1);
			vst1q_f32(pDestination, T2);

			T1 = vextq_f32(M.r[1], M.r[1], 1);
			T2 = vcombine_f32(vget_low_f32(T1), vget_low_f32(M.r[2]));
			vst1q_f32(pDestination + 4, T2);

			vst1q_lane_f32(pDestination + 8, M.r[2], 2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp1 = M.r[0];
			vec_float_t vTemp2 = M.r[1];
			vec_float_t vTemp3 = M.r[2];
			vec_float_t vWork = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(0, 0, 2, 2));
			vTemp1 = _mm_shuffle_ps(vTemp1, vWork, _MM_SHUFFLE(2, 0, 1, 0));
			_mm_storeu_ps(pDestination, vTemp1);
			vTemp2 = _mm_shuffle_ps(vTemp2, vTemp3, _MM_SHUFFLE(1, 0, 2, 1));
			_mm_storeu_ps(pDestination + 4, vTemp2);
			vTemp3 = LUNA_SIMD_PERMUTE_PS(vTemp3, _MM_SHUFFLE(2, 2, 2, 2));
			_mm_store_ss(pDestination + 8, vTemp3);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float4x3
		(
			f32* pDestination,
			mat_float_p1_t M
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t T1 = vextq_f32(M.r[0], M.r[1], 1);
			float32x4_t T2 = vbslq_f32(g_Mask3, M.r[0], T1);
			vst1q_f32(pDestination, T2);

			T1 = vextq_f32(M.r[1], M.r[1], 1);
			T2 = vcombine_f32(vget_low_f32(T1), vget_low_f32(M.r[2]));
			vst1q_f32(pDestination + 4, T2);

			T1 = vdupq_lane_f32(vget_high_f32(M.r[2]), 0);
			T2 = vextq_f32(T1, M.r[3], 3);
			vst1q_f32(pDestination + 8, T2);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp1 = M.r[0];
			vec_float_t vTemp2 = M.r[1];
			vec_float_t vTemp3 = M.r[2];
			vec_float_t vTemp4 = M.r[3];
			vec_float_t vTemp2x = _mm_shuffle_ps(vTemp2, vTemp3, _MM_SHUFFLE(1, 0, 2, 1));
			vTemp2 = _mm_shuffle_ps(vTemp2, vTemp1, _MM_SHUFFLE(2, 2, 0, 0));
			vTemp1 = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(0, 2, 1, 0));
			vTemp3 = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(0, 0, 2, 2));
			vTemp3 = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 1, 2, 0));
			_mm_storeu_ps(pDestination, vTemp1);
			_mm_storeu_ps(pDestination + 4, vTemp2x);
			_mm_storeu_ps(pDestination + 8, vTemp3);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float4x3a
		(
			f32*	pDestination,
			mat_float_p1_t		M
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4_t T1 = vextq_f32(M.r[0], M.r[1], 1);
			float32x4_t T2 = vbslq_f32(g_Mask3, M.r[0], T1);
			vst1q_f32_ex(pDestination, T2, 128);

			T1 = vextq_f32(M.r[1], M.r[1], 1);
			T2 = vcombine_f32(vget_low_f32(T1), vget_low_f32(M.r[2]));
			vst1q_f32_ex(pDestination + 4, T2, 128);

			T1 = vdupq_lane_f32(vget_high_f32(M.r[2]), 0);
			T2 = vextq_f32(T1, M.r[3], 3);
			vst1q_f32_ex(pDestination + 8, T2, 128);
#elif defined(LUNA_PLATFORM_SSE2)
			// x1,y1,z1,w1
			vec_float_t vTemp1 = M.r[0];
			// x2,y2,z2,w2
			vec_float_t vTemp2 = M.r[1];
			// x3,y3,z3,w3
			vec_float_t vTemp3 = M.r[2];
			// x4,y4,z4,w4
			vec_float_t vTemp4 = M.r[3];
			// z1,z1,x2,y2
			vec_float_t vTemp = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(1, 0, 2, 2));
			// y2,z2,x3,y3 (Final)
			vTemp2 = _mm_shuffle_ps(vTemp2, vTemp3, _MM_SHUFFLE(1, 0, 2, 1));
			// x1,y1,z1,x2 (Final)
			vTemp1 = _mm_shuffle_ps(vTemp1, vTemp, _MM_SHUFFLE(2, 0, 1, 0));
			// z3,z3,x4,x4
			vTemp3 = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(0, 0, 2, 2));
			// z3,x4,y4,z4 (Final)
			vTemp3 = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 1, 2, 0));
			// Store in 3 operations
			_mm_store_ps(pDestination, vTemp1);
			_mm_store_ps(pDestination + 4, vTemp2);
			_mm_store_ps(pDestination + 8, vTemp3);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float3x4
		(
			f32* pDestination,
			mat_float_p1_t M
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4x2_t P0 = vzipq_f32(M.r[0], M.r[2]);
			float32x4x2_t P1 = vzipq_f32(M.r[1], M.r[3]);

			float32x4x2_t T0 = vzipq_f32(P0.val[0], P1.val[0]);
			float32x4x2_t T1 = vzipq_f32(P0.val[1], P1.val[1]);

			vst1q_f32(pDestination, T0.val[0]);
			vst1q_f32(pDestination + 4, T0.val[1]);
			vst1q_f32(pDestination + 8, T1.val[0]);
#elif defined(LUNA_PLATFORM_SSE2)
			// x.x,x.y,y.x,y.y
			vec_float_t vTemp1 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(1, 0, 1, 0));
			// x.z,x.w,y.z,y.w
			vec_float_t vTemp3 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(3, 2, 3, 2));
			// z.x,z.y,w.x,w.y
			vec_float_t vTemp2 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(1, 0, 1, 0));
			// z.z,z.w,w.z,w.w
			vec_float_t vTemp4 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(3, 2, 3, 2));

			// x.x,y.x,z.x,w.x
			vec_float_t r0 = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));
			// x.y,y.y,z.y,w.y
			vec_float_t r1 = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));
			// x.z,y.z,z.z,w.z
			vec_float_t r2 = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));

			_mm_storeu_ps(pDestination, r0);
			_mm_storeu_ps(pDestination + 4, r1);
			_mm_storeu_ps(pDestination + 8, r2);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float3x4a
		(
			f32* pDestination,
			mat_float_p1_t M
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4x2_t P0 = vzipq_f32(M.r[0], M.r[2]);
			float32x4x2_t P1 = vzipq_f32(M.r[1], M.r[3]);

			float32x4x2_t T0 = vzipq_f32(P0.val[0], P1.val[0]);
			float32x4x2_t T1 = vzipq_f32(P0.val[1], P1.val[1]);

			vst1q_f32_ex(pDestination, T0.val[0], 128);
			vst1q_f32_ex(pDestination + 4, T0.val[1], 128);
			vst1q_f32_ex(pDestination + 8, T1.val[0], 128);
#elif defined(LUNA_PLATFORM_SSE2)
			// x.x,x.y,y.x,y.y
			vec_float_t vTemp1 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(1, 0, 1, 0));
			// x.z,x.w,y.z,y.w
			vec_float_t vTemp3 = _mm_shuffle_ps(M.r[0], M.r[1], _MM_SHUFFLE(3, 2, 3, 2));
			// z.x,z.y,w.x,w.y
			vec_float_t vTemp2 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(1, 0, 1, 0));
			// z.z,z.w,w.z,w.w
			vec_float_t vTemp4 = _mm_shuffle_ps(M.r[2], M.r[3], _MM_SHUFFLE(3, 2, 3, 2));

			// x.x,y.x,z.x,w.x
			vec_float_t r0 = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(2, 0, 2, 0));
			// x.y,y.y,z.y,w.y
			vec_float_t r1 = _mm_shuffle_ps(vTemp1, vTemp2, _MM_SHUFFLE(3, 1, 3, 1));
			// x.z,y.z,z.z,w.z
			vec_float_t r2 = _mm_shuffle_ps(vTemp3, vTemp4, _MM_SHUFFLE(2, 0, 2, 0));

			_mm_store_ps(pDestination, r0);
			_mm_store_ps(pDestination + 4, r1);
			_mm_store_ps(pDestination + 8, r2);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float4x4
		(
			f32* pDestination,
			mat_float_p1_t M
		)
		{
			luassert(pDestination);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_f32(reinterpret_cast<float*>(pDestination), M.r[0]);
			vst1q_f32(reinterpret_cast<float*>(pDestination + 4), M.r[1]);
			vst1q_f32(reinterpret_cast<float*>(pDestination + 8), M.r[2]);
			vst1q_f32(reinterpret_cast<float*>(pDestination + 12), M.r[3]);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_storeu_ps(pDestination, M.r[0]);
			_mm_storeu_ps(pDestination + 4, M.r[1]);
			_mm_storeu_ps(pDestination + 8, M.r[2]);
			_mm_storeu_ps(pDestination + 12, M.r[3]);
#endif
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV store_float4x4a
		(
			f32*	pDestination,
			mat_float_p1_t		M
		)
		{
			luassert(pDestination);
			luassert((reinterpret_cast<uintptr_t>(pDestination) & 0xF) == 0);
#if defined(LUNA_PLATFORM_ARM_NEON)
			vst1q_f32_ex(reinterpret_cast<float*>(pDestination), M.r[0], 128);
			vst1q_f32_ex(reinterpret_cast<float*>(pDestination + 4), M.r[1], 128);
			vst1q_f32_ex(reinterpret_cast<float*>(pDestination + 8), M.r[2], 128);
			vst1q_f32_ex(reinterpret_cast<float*>(pDestination + 12), M.r[3], 128);
#elif defined(LUNA_PLATFORM_SSE2)
			_mm_store_ps(pDestination, M.r[0]);
			_mm_store_ps(pDestination + 4, M.r[1]);
			_mm_store_ps(pDestination + 8, M.r[2]);
			_mm_store_ps(pDestination + 12, M.r[3]);
#endif
		}
	}
}

#endif // LUNA_SIMD_ENABLED.