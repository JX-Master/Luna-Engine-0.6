// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file SimdMatrix.hpp
* @author JxMaster
* @date 2019/1/4
* @brief Multi-platform SIMD Library modified from Microsoft's DirectX Math.
* See: https://github.com/Microsoft/DirectXMath
 */
#pragma once

#include "SimdVector.hpp"

#if defined(LUNA_SIMD_ENABLED)

namespace Luna
{
	namespace Simd
	{

		/****************************************************************************
		 *
		 * Matrix
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		 // Return true if any entry in the matrix is NaN
		inline bool LUNA_SIMD_CALLCONV MatrixIsNaN
		(
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Load in registers
			vec_float_t vX = M.r[0];
			vec_float_t vY = M.r[1];
			vec_float_t vZ = M.r[2];
			vec_float_t vW = M.r[3];
			// Test themselves to check for NaN
			vX = vmvnq_u32(vceqq_f32(vX, vX));
			vY = vmvnq_u32(vceqq_f32(vY, vY));
			vZ = vmvnq_u32(vceqq_f32(vZ, vZ));
			vW = vmvnq_u32(vceqq_f32(vW, vW));
			// Or all the results
			vX = vorrq_u32(vX, vZ);
			vY = vorrq_u32(vY, vW);
			vX = vorrq_u32(vX, vY);
			// If any tested true, return true
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vX), vget_high_u8(vX));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			uint32_t r = vget_lane_u32(vTemp.val[1], 1);
			return (r != 0);
#elif defined(LUNA_PLATFORM_SSE2)
			// Load in registers
			vec_float_t vX = M.r[0];
			vec_float_t vY = M.r[1];
			vec_float_t vZ = M.r[2];
			vec_float_t vW = M.r[3];
			// Test themselves to check for NaN
			vX = _mm_cmpneq_ps(vX, vX);
			vY = _mm_cmpneq_ps(vY, vY);
			vZ = _mm_cmpneq_ps(vZ, vZ);
			vW = _mm_cmpneq_ps(vW, vW);
			// Or all the results
			vX = _mm_or_ps(vX, vZ);
			vY = _mm_or_ps(vY, vW);
			vX = _mm_or_ps(vX, vY);
			// If any tested true, return true
			return (_mm_movemask_ps(vX) != 0);
#else
#endif
		}

		//------------------------------------------------------------------------------

		// Return true if any entry in the matrix is +/-INF
		inline bool LUNA_SIMD_CALLCONV MatrixIsInfinite
		(
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Mask off the sign bits
			vec_float_t vTemp1 = vandq_u32(M.r[0], g_AbsMask);
			vec_float_t vTemp2 = vandq_u32(M.r[1], g_AbsMask);
			vec_float_t vTemp3 = vandq_u32(M.r[2], g_AbsMask);
			vec_float_t vTemp4 = vandq_u32(M.r[3], g_AbsMask);
			// Compare to infinity
			vTemp1 = vceqq_f32(vTemp1, g_Infinity);
			vTemp2 = vceqq_f32(vTemp2, g_Infinity);
			vTemp3 = vceqq_f32(vTemp3, g_Infinity);
			vTemp4 = vceqq_f32(vTemp4, g_Infinity);
			// Or the answers together
			vTemp1 = vorrq_u32(vTemp1, vTemp2);
			vTemp3 = vorrq_u32(vTemp3, vTemp4);
			vTemp1 = vorrq_u32(vTemp1, vTemp3);
			// If any are infinity, the signs are true.
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTemp1), vget_high_u8(vTemp1));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			uint32_t r = vget_lane_u32(vTemp.val[1], 1);
			return (r != 0);
#elif defined(LUNA_PLATFORM_SSE2)
			// Mask off the sign bits
			vec_float_t vTemp1 = _mm_and_ps(M.r[0], g_AbsMask);
			vec_float_t vTemp2 = _mm_and_ps(M.r[1], g_AbsMask);
			vec_float_t vTemp3 = _mm_and_ps(M.r[2], g_AbsMask);
			vec_float_t vTemp4 = _mm_and_ps(M.r[3], g_AbsMask);
			// Compare to infinity
			vTemp1 = _mm_cmpeq_ps(vTemp1, g_Infinity);
			vTemp2 = _mm_cmpeq_ps(vTemp2, g_Infinity);
			vTemp3 = _mm_cmpeq_ps(vTemp3, g_Infinity);
			vTemp4 = _mm_cmpeq_ps(vTemp4, g_Infinity);
			// Or the answers together
			vTemp1 = _mm_or_ps(vTemp1, vTemp2);
			vTemp3 = _mm_or_ps(vTemp3, vTemp4);
			vTemp1 = _mm_or_ps(vTemp1, vTemp3);
			// If any are infinity, the signs are true.
			return (_mm_movemask_ps(vTemp1) != 0);
#endif
		}

		//------------------------------------------------------------------------------

		// Return true if the Matrix is equal to identity
		inline bool LUNA_SIMD_CALLCONV MatrixIsIdentity
		(
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float_t vTemp1 = vceqq_f32(M.r[0], g_IdentityR0);
			vec_float_t vTemp2 = vceqq_f32(M.r[1], g_IdentityR1);
			vec_float_t vTemp3 = vceqq_f32(M.r[2], g_IdentityR2);
			vec_float_t vTemp4 = vceqq_f32(M.r[3], g_IdentityR3);
			vTemp1 = vandq_u32(vTemp1, vTemp2);
			vTemp3 = vandq_u32(vTemp3, vTemp4);
			vTemp1 = vandq_u32(vTemp1, vTemp3);
			int8x8x2_t vTemp = vzip_u8(vget_low_u8(vTemp1), vget_high_u8(vTemp1));
			vTemp = vzip_u16(vTemp.val[0], vTemp.val[1]);
			uint32_t r = vget_lane_u32(vTemp.val[1], 1);
			return (r == 0xFFFFFFFFU);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vTemp1 = _mm_cmpeq_ps(M.r[0], g_IdentityR0);
			vec_float_t vTemp2 = _mm_cmpeq_ps(M.r[1], g_IdentityR1);
			vec_float_t vTemp3 = _mm_cmpeq_ps(M.r[2], g_IdentityR2);
			vec_float_t vTemp4 = _mm_cmpeq_ps(M.r[3], g_IdentityR3);
			vTemp1 = _mm_and_ps(vTemp1, vTemp2);
			vTemp3 = _mm_and_ps(vTemp3, vTemp4);
			vTemp1 = _mm_and_ps(vTemp1, vTemp3);
			return (_mm_movemask_ps(vTemp1) == 0x0f);
#endif
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------
		// Perform a 4x4 matrix multiply by a 4x4 matrix
		inline mat_float_t LUNA_SIMD_CALLCONV MatrixMultiply
		(
			mat_float_p1_t M1,
			mat_float_p2_t M2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t mResult;
			float32x2_t VL = vget_low_f32(M1.r[0]);
			float32x2_t VH = vget_high_f32(M1.r[0]);
			// Perform the operation on the first row
			vec_float_t vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vec_float_t vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vec_float_t vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vec_float_t vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			mResult.r[0] = vaddq_f32(vZ, vW);
			// Repeat for the other 3 rows
			VL = vget_low_f32(M1.r[1]);
			VH = vget_high_f32(M1.r[1]);
			vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			mResult.r[1] = vaddq_f32(vZ, vW);
			VL = vget_low_f32(M1.r[2]);
			VH = vget_high_f32(M1.r[2]);
			vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			mResult.r[2] = vaddq_f32(vZ, vW);
			VL = vget_low_f32(M1.r[3]);
			VH = vget_high_f32(M1.r[3]);
			vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			mResult.r[3] = vaddq_f32(vZ, vW);
			return mResult;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t mResult;
			// Splat the component X,Y,Z then W
#if defined(LUNA_PLATFORM_AVX)
			vec_float_t vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 0);
			vec_float_t vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 1);
			vec_float_t vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 2);
			vec_float_t vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 3);
#else
	// Use vW to hold the original row
			vec_float_t vW = M1.r[0];
			vec_float_t vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vec_float_t vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vec_float_t vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			// Perform the operation on the first row
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			// Perform a binary add to reduce cumulative errors
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			mResult.r[0] = vX;
			// Repeat for the other 3 rows
#if defined(LUNA_PLATFORM_AVX)
			vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 0);
			vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 1);
			vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 2);
			vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 3);
#else
			vW = M1.r[1];
			vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			mResult.r[1] = vX;
#if defined(LUNA_PLATFORM_AVX)
			vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 0);
			vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 1);
			vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 2);
			vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 3);
#else
			vW = M1.r[2];
			vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			mResult.r[2] = vX;
#if defined(LUNA_PLATFORM_AVX)
			vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 0);
			vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 1);
			vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 2);
			vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 3);
#else
			vW = M1.r[3];
			vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			mResult.r[3] = vX;
			return mResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixMultiplyTranspose
		(
			mat_float_p1_t M1,
			mat_float_p2_t M2
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x2_t VL = vget_low_f32(M1.r[0]);
			float32x2_t VH = vget_high_f32(M1.r[0]);
			// Perform the operation on the first row
			vec_float_t vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vec_float_t vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vec_float_t vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vec_float_t vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			float32x4_t r0 = vaddq_f32(vZ, vW);
			// Repeat for the other 3 rows
			VL = vget_low_f32(M1.r[1]);
			VH = vget_high_f32(M1.r[1]);
			vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			float32x4_t r1 = vaddq_f32(vZ, vW);
			VL = vget_low_f32(M1.r[2]);
			VH = vget_high_f32(M1.r[2]);
			vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			float32x4_t r2 = vaddq_f32(vZ, vW);
			VL = vget_low_f32(M1.r[3]);
			VH = vget_high_f32(M1.r[3]);
			vX = vmulq_lane_f32(M2.r[0], VL, 0);
			vY = vmulq_lane_f32(M2.r[1], VL, 1);
			vZ = vmlaq_lane_f32(vX, M2.r[2], VH, 0);
			vW = vmlaq_lane_f32(vY, M2.r[3], VH, 1);
			float32x4_t r3 = vaddq_f32(vZ, vW);

			// Transpose result
			float32x4x2_t P0 = vzipq_f32(r0, r2);
			float32x4x2_t P1 = vzipq_f32(r1, r3);

			float32x4x2_t T0 = vzipq_f32(P0.val[0], P1.val[0]);
			float32x4x2_t T1 = vzipq_f32(P0.val[1], P1.val[1]);

			mat_float_t mResult;
			mResult.r[0] = T0.val[0];
			mResult.r[1] = T0.val[1];
			mResult.r[2] = T1.val[0];
			mResult.r[3] = T1.val[1];
			return mResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Splat the component X,Y,Z then W
#if defined(LUNA_PLATFORM_AVX)
			vec_float_t vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 0);
			vec_float_t vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 1);
			vec_float_t vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 2);
			vec_float_t vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[0]) + 3);
#else
	// Use vW to hold the original row
			vec_float_t vW = M1.r[0];
			vec_float_t vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vec_float_t vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vec_float_t vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			// Perform the operation on the first row
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			// Perform a binary add to reduce cumulative errors
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			vec_float_t r0 = vX;
			// Repeat for the other 3 rows
#if defined(LUNA_PLATFORM_AVX)
			vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 0);
			vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 1);
			vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 2);
			vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[1]) + 3);
#else
			vW = M1.r[1];
			vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			vec_float_t r1 = vX;
#if defined(LUNA_PLATFORM_AVX)
			vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 0);
			vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 1);
			vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 2);
			vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[2]) + 3);
#else
			vW = M1.r[2];
			vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			vec_float_t r2 = vX;
#if defined(LUNA_PLATFORM_AVX)
			vX = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 0);
			vY = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 1);
			vZ = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 2);
			vW = _mm_broadcast_ss(reinterpret_cast<const f32*>(&M1.r[3]) + 3);
#else
			vW = M1.r[3];
			vX = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(0, 0, 0, 0));
			vY = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(1, 1, 1, 1));
			vZ = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(2, 2, 2, 2));
			vW = LUNA_SIMD_PERMUTE_PS(vW, _MM_SHUFFLE(3, 3, 3, 3));
#endif
			vX = _mm_mul_ps(vX, M2.r[0]);
			vY = _mm_mul_ps(vY, M2.r[1]);
			vZ = _mm_mul_ps(vZ, M2.r[2]);
			vW = _mm_mul_ps(vW, M2.r[3]);
			vX = _mm_add_ps(vX, vZ);
			vY = _mm_add_ps(vY, vW);
			vX = _mm_add_ps(vX, vY);
			vec_float_t r3 = vX;

			// x.x,x.y,y.x,y.y
			vec_float_t vTemp1 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(1, 0, 1, 0));
			// x.z,x.w,y.z,y.w
			vec_float_t vTemp3 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(3, 2, 3, 2));
			// z.x,z.y,w.x,w.y
			vec_float_t vTemp2 = _mm_shuffle_ps(r2, r3, _MM_SHUFFLE(1, 0, 1, 0));
			// z.z,z.w,w.z,w.w
			vec_float_t vTemp4 = _mm_shuffle_ps(r2, r3, _MM_SHUFFLE(3, 2, 3, 2));

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

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixTranspose
		(
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			float32x4x2_t P0 = vzipq_f32(M.r[0], M.r[2]);
			float32x4x2_t P1 = vzipq_f32(M.r[1], M.r[3]);

			float32x4x2_t T0 = vzipq_f32(P0.val[0], P1.val[0]);
			float32x4x2_t T1 = vzipq_f32(P0.val[1], P1.val[1]);

			mat_float_t mResult;
			mResult.r[0] = T0.val[0];
			mResult.r[1] = T0.val[1];
			mResult.r[2] = T1.val[0];
			mResult.r[3] = T1.val[1];
			return mResult;
#elif defined(LUNA_PLATFORM_SSE2)
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
		// Return the inverse and the determinant of a 4x4 matrix

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixInverse
		(
			vec_float_t* pDeterminant,
			mat_float_p1_t  M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)

			mat_float_t MT = MatrixTranspose(M);

			vec_float_t V0[4], V1[4];
			V0[0] = VectorSwizzle<g_swizzle_x, g_swizzle_x, g_swizzle_y, g_swizzle_y>(MT.r[2]);
			V1[0] = VectorSwizzle<g_swizzle_z, g_swizzle_w, g_swizzle_z, g_swizzle_w>(MT.r[3]);
			V0[1] = VectorSwizzle<g_swizzle_x, g_swizzle_x, g_swizzle_y, g_swizzle_y>(MT.r[0]);
			V1[1] = VectorSwizzle<g_swizzle_z, g_swizzle_w, g_swizzle_z, g_swizzle_w>(MT.r[1]);
			V0[2] = VectorPermute<g_permute_0x, g_permute_0z, g_permute_1x, g_permute_1z>(MT.r[2], MT.r[0]);
			V1[2] = VectorPermute<g_permute_0y, g_permute_0w, g_permute_1y, g_permute_1w>(MT.r[3], MT.r[1]);

			vec_float_t D0 = VectorMultiply(V0[0], V1[0]);
			vec_float_t D1 = VectorMultiply(V0[1], V1[1]);
			vec_float_t D2 = VectorMultiply(V0[2], V1[2]);

			V0[0] = VectorSwizzle<g_swizzle_z, g_swizzle_w, g_swizzle_z, g_swizzle_w>(MT.r[2]);
			V1[0] = VectorSwizzle<g_swizzle_x, g_swizzle_x, g_swizzle_y, g_swizzle_y>(MT.r[3]);
			V0[1] = VectorSwizzle<g_swizzle_z, g_swizzle_w, g_swizzle_z, g_swizzle_w>(MT.r[0]);
			V1[1] = VectorSwizzle<g_swizzle_x, g_swizzle_x, g_swizzle_y, g_swizzle_y>(MT.r[1]);
			V0[2] = VectorPermute<g_permute_0y, g_permute_0w, g_permute_1y, g_permute_1w>(MT.r[2], MT.r[0]);
			V1[2] = VectorPermute<g_permute_0x, g_permute_0z, g_permute_1x, g_permute_1z>(MT.r[3], MT.r[1]);

			D0 = VectorNegativeMultiplySubtract(V0[0], V1[0], D0);
			D1 = VectorNegativeMultiplySubtract(V0[1], V1[1], D1);
			D2 = VectorNegativeMultiplySubtract(V0[2], V1[2], D2);

			V0[0] = VectorSwizzle<g_swizzle_y, g_swizzle_z, g_swizzle_x, g_swizzle_y>(MT.r[1]);
			V1[0] = VectorPermute<g_permute_1y, g_permute_0y, g_permute_0w, g_permute_0x>(D0, D2);
			V0[1] = VectorSwizzle<g_swizzle_z, g_swizzle_x, g_swizzle_y, g_swizzle_x>(MT.r[0]);
			V1[1] = VectorPermute<g_permute_0w, g_permute_1y, g_permute_0y, g_permute_0z>(D0, D2);
			V0[2] = VectorSwizzle<g_swizzle_y, g_swizzle_z, g_swizzle_x, g_swizzle_y>(MT.r[3]);
			V1[2] = VectorPermute<g_permute_1w, g_permute_0y, g_permute_0w, g_permute_0x>(D1, D2);
			V0[3] = VectorSwizzle<g_swizzle_z, g_swizzle_x, g_swizzle_y, g_swizzle_x>(MT.r[2]);
			V1[3] = VectorPermute<g_permute_0w, g_permute_1w, g_permute_0y, g_permute_0z>(D1, D2);

			vec_float_t C0 = VectorMultiply(V0[0], V1[0]);
			vec_float_t C2 = VectorMultiply(V0[1], V1[1]);
			vec_float_t C4 = VectorMultiply(V0[2], V1[2]);
			vec_float_t C6 = VectorMultiply(V0[3], V1[3]);

			V0[0] = VectorSwizzle<g_swizzle_z, g_swizzle_w, g_swizzle_y, g_swizzle_z>(MT.r[1]);
			V1[0] = VectorPermute<g_permute_0w, g_permute_0x, g_permute_0y, g_permute_1x>(D0, D2);
			V0[1] = VectorSwizzle<g_swizzle_w, g_swizzle_z, g_swizzle_w, g_swizzle_y>(MT.r[0]);
			V1[1] = VectorPermute<g_permute_0z, g_permute_0y, g_permute_1x, g_permute_0x>(D0, D2);
			V0[2] = VectorSwizzle<g_swizzle_z, g_swizzle_w, g_swizzle_y, g_swizzle_z>(MT.r[3]);
			V1[2] = VectorPermute<g_permute_0w, g_permute_0x, g_permute_0y, g_permute_1z>(D1, D2);
			V0[3] = VectorSwizzle<g_swizzle_w, g_swizzle_z, g_swizzle_w, g_swizzle_y>(MT.r[2]);
			V1[3] = VectorPermute<g_permute_0z, g_permute_0y, g_permute_1z, g_permute_0x>(D1, D2);

			C0 = VectorNegativeMultiplySubtract(V0[0], V1[0], C0);
			C2 = VectorNegativeMultiplySubtract(V0[1], V1[1], C2);
			C4 = VectorNegativeMultiplySubtract(V0[2], V1[2], C4);
			C6 = VectorNegativeMultiplySubtract(V0[3], V1[3], C6);

			V0[0] = VectorSwizzle<g_swizzle_w, g_swizzle_x, g_swizzle_w, g_swizzle_x>(MT.r[1]);
			V1[0] = VectorPermute<g_permute_0z, g_permute_1y, g_permute_1x, g_permute_0z>(D0, D2);
			V0[1] = VectorSwizzle<g_swizzle_y, g_swizzle_w, g_swizzle_x, g_swizzle_z>(MT.r[0]);
			V1[1] = VectorPermute<g_permute_1y, g_permute_0x, g_permute_0w, g_permute_1x>(D0, D2);
			V0[2] = VectorSwizzle<g_swizzle_w, g_swizzle_x, g_swizzle_w, g_swizzle_x>(MT.r[3]);
			V1[2] = VectorPermute<g_permute_0z, g_permute_1w, g_permute_1z, g_permute_0z>(D1, D2);
			V0[3] = VectorSwizzle<g_swizzle_y, g_swizzle_w, g_swizzle_x, g_swizzle_z>(MT.r[2]);
			V1[3] = VectorPermute<g_permute_1w, g_permute_0x, g_permute_0w, g_permute_1z>(D1, D2);

			vec_float_t C1 = VectorNegativeMultiplySubtract(V0[0], V1[0], C0);
			C0 = VectorMultiplyAdd(V0[0], V1[0], C0);
			vec_float_t C3 = VectorMultiplyAdd(V0[1], V1[1], C2);
			C2 = VectorNegativeMultiplySubtract(V0[1], V1[1], C2);
			vec_float_t C5 = VectorNegativeMultiplySubtract(V0[2], V1[2], C4);
			C4 = VectorMultiplyAdd(V0[2], V1[2], C4);
			vec_float_t C7 = VectorMultiplyAdd(V0[3], V1[3], C6);
			C6 = VectorNegativeMultiplySubtract(V0[3], V1[3], C6);

			mat_float_t R;
			R.r[0] = VectorSelect(C0, C1, g_Select0101.v);
			R.r[1] = VectorSelect(C2, C3, g_Select0101.v);
			R.r[2] = VectorSelect(C4, C5, g_Select0101.v);
			R.r[3] = VectorSelect(C6, C7, g_Select0101.v);

			vec_float_t Determinant = Vector4Dot(R.r[0], MT.r[0]);

			if (pDeterminant != nullptr)
				*pDeterminant = Determinant;

			vec_float_t Reciprocal = VectorReciprocal(Determinant);

			mat_float_t Result;
			Result.r[0] = VectorMultiply(R.r[0], Reciprocal);
			Result.r[1] = VectorMultiply(R.r[1], Reciprocal);
			Result.r[2] = VectorMultiply(R.r[2], Reciprocal);
			Result.r[3] = VectorMultiply(R.r[3], Reciprocal);
			return Result;

#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t MT = MatrixTranspose(M);
			vec_float_t V00 = LUNA_SIMD_PERMUTE_PS(MT.r[2], _MM_SHUFFLE(1, 1, 0, 0));
			vec_float_t V10 = LUNA_SIMD_PERMUTE_PS(MT.r[3], _MM_SHUFFLE(3, 2, 3, 2));
			vec_float_t V01 = LUNA_SIMD_PERMUTE_PS(MT.r[0], _MM_SHUFFLE(1, 1, 0, 0));
			vec_float_t V11 = LUNA_SIMD_PERMUTE_PS(MT.r[1], _MM_SHUFFLE(3, 2, 3, 2));
			vec_float_t V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(2, 0, 2, 0));
			vec_float_t V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(3, 1, 3, 1));

			vec_float_t D0 = _mm_mul_ps(V00, V10);
			vec_float_t D1 = _mm_mul_ps(V01, V11);
			vec_float_t D2 = _mm_mul_ps(V02, V12);

			V00 = LUNA_SIMD_PERMUTE_PS(MT.r[2], _MM_SHUFFLE(3, 2, 3, 2));
			V10 = LUNA_SIMD_PERMUTE_PS(MT.r[3], _MM_SHUFFLE(1, 1, 0, 0));
			V01 = LUNA_SIMD_PERMUTE_PS(MT.r[0], _MM_SHUFFLE(3, 2, 3, 2));
			V11 = LUNA_SIMD_PERMUTE_PS(MT.r[1], _MM_SHUFFLE(1, 1, 0, 0));
			V02 = _mm_shuffle_ps(MT.r[2], MT.r[0], _MM_SHUFFLE(3, 1, 3, 1));
			V12 = _mm_shuffle_ps(MT.r[3], MT.r[1], _MM_SHUFFLE(2, 0, 2, 0));

			V00 = _mm_mul_ps(V00, V10);
			V01 = _mm_mul_ps(V01, V11);
			V02 = _mm_mul_ps(V02, V12);
			D0 = _mm_sub_ps(D0, V00);
			D1 = _mm_sub_ps(D1, V01);
			D2 = _mm_sub_ps(D2, V02);
			// V11 = D0Y,D0W,D2Y,D2Y
			V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
			V00 = LUNA_SIMD_PERMUTE_PS(MT.r[1], _MM_SHUFFLE(1, 0, 2, 1));
			V10 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(0, 3, 0, 2));
			V01 = LUNA_SIMD_PERMUTE_PS(MT.r[0], _MM_SHUFFLE(0, 1, 0, 2));
			V11 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(2, 1, 2, 1));
			// V13 = D1Y,D1W,D2W,D2W
			vec_float_t V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
			V02 = LUNA_SIMD_PERMUTE_PS(MT.r[3], _MM_SHUFFLE(1, 0, 2, 1));
			V12 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(0, 3, 0, 2));
			vec_float_t V03 = LUNA_SIMD_PERMUTE_PS(MT.r[2], _MM_SHUFFLE(0, 1, 0, 2));
			V13 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(2, 1, 2, 1));

			vec_float_t C0 = _mm_mul_ps(V00, V10);
			vec_float_t C2 = _mm_mul_ps(V01, V11);
			vec_float_t C4 = _mm_mul_ps(V02, V12);
			vec_float_t C6 = _mm_mul_ps(V03, V13);

			// V11 = D0X,D0Y,D2X,D2X
			V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
			V00 = LUNA_SIMD_PERMUTE_PS(MT.r[1], _MM_SHUFFLE(2, 1, 3, 2));
			V10 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(2, 1, 0, 3));
			V01 = LUNA_SIMD_PERMUTE_PS(MT.r[0], _MM_SHUFFLE(1, 3, 2, 3));
			V11 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(0, 2, 1, 2));
			// V13 = D1X,D1Y,D2Z,D2Z
			V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
			V02 = LUNA_SIMD_PERMUTE_PS(MT.r[3], _MM_SHUFFLE(2, 1, 3, 2));
			V12 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(2, 1, 0, 3));
			V03 = LUNA_SIMD_PERMUTE_PS(MT.r[2], _MM_SHUFFLE(1, 3, 2, 3));
			V13 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(0, 2, 1, 2));

			V00 = _mm_mul_ps(V00, V10);
			V01 = _mm_mul_ps(V01, V11);
			V02 = _mm_mul_ps(V02, V12);
			V03 = _mm_mul_ps(V03, V13);
			C0 = _mm_sub_ps(C0, V00);
			C2 = _mm_sub_ps(C2, V01);
			C4 = _mm_sub_ps(C4, V02);
			C6 = _mm_sub_ps(C6, V03);

			V00 = LUNA_SIMD_PERMUTE_PS(MT.r[1], _MM_SHUFFLE(0, 3, 0, 3));
			// V10 = D0Z,D0Z,D2X,D2Y
			V10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
			V10 = LUNA_SIMD_PERMUTE_PS(V10, _MM_SHUFFLE(0, 2, 3, 0));
			V01 = LUNA_SIMD_PERMUTE_PS(MT.r[0], _MM_SHUFFLE(2, 0, 3, 1));
			// V11 = D0X,D0W,D2X,D2Y
			V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
			V11 = LUNA_SIMD_PERMUTE_PS(V11, _MM_SHUFFLE(2, 1, 0, 3));
			V02 = LUNA_SIMD_PERMUTE_PS(MT.r[3], _MM_SHUFFLE(0, 3, 0, 3));
			// V12 = D1Z,D1Z,D2Z,D2W
			V12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
			V12 = LUNA_SIMD_PERMUTE_PS(V12, _MM_SHUFFLE(0, 2, 3, 0));
			V03 = LUNA_SIMD_PERMUTE_PS(MT.r[2], _MM_SHUFFLE(2, 0, 3, 1));
			// V13 = D1X,D1W,D2Z,D2W
			V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
			V13 = LUNA_SIMD_PERMUTE_PS(V13, _MM_SHUFFLE(2, 1, 0, 3));

			V00 = _mm_mul_ps(V00, V10);
			V01 = _mm_mul_ps(V01, V11);
			V02 = _mm_mul_ps(V02, V12);
			V03 = _mm_mul_ps(V03, V13);
			vec_float_t C1 = _mm_sub_ps(C0, V00);
			C0 = _mm_add_ps(C0, V00);
			vec_float_t C3 = _mm_add_ps(C2, V01);
			C2 = _mm_sub_ps(C2, V01);
			vec_float_t C5 = _mm_sub_ps(C4, V02);
			C4 = _mm_add_ps(C4, V02);
			vec_float_t C7 = _mm_add_ps(C6, V03);
			C6 = _mm_sub_ps(C6, V03);

			C0 = _mm_shuffle_ps(C0, C1, _MM_SHUFFLE(3, 1, 2, 0));
			C2 = _mm_shuffle_ps(C2, C3, _MM_SHUFFLE(3, 1, 2, 0));
			C4 = _mm_shuffle_ps(C4, C5, _MM_SHUFFLE(3, 1, 2, 0));
			C6 = _mm_shuffle_ps(C6, C7, _MM_SHUFFLE(3, 1, 2, 0));
			C0 = LUNA_SIMD_PERMUTE_PS(C0, _MM_SHUFFLE(3, 1, 2, 0));
			C2 = LUNA_SIMD_PERMUTE_PS(C2, _MM_SHUFFLE(3, 1, 2, 0));
			C4 = LUNA_SIMD_PERMUTE_PS(C4, _MM_SHUFFLE(3, 1, 2, 0));
			C6 = LUNA_SIMD_PERMUTE_PS(C6, _MM_SHUFFLE(3, 1, 2, 0));
			// Get the determinate
			vec_float_t vTemp = Vector4Dot(C0, MT.r[0]);
			if (pDeterminant != nullptr)
				*pDeterminant = vTemp;
			vTemp = _mm_div_ps(g_One, vTemp);
			mat_float_t mResult;
			mResult.r[0] = _mm_mul_ps(C0, vTemp);
			mResult.r[1] = _mm_mul_ps(C2, vTemp);
			mResult.r[2] = _mm_mul_ps(C4, vTemp);
			mResult.r[3] = _mm_mul_ps(C6, vTemp);
			return mResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV MatrixDeterminant
		(
			mat_float_p1_t M
		)
		{
			static const vec_f32_t Sign = { { { 1.0f, -1.0f, 1.0f, -1.0f } } };

			vec_float_t V0 = VectorSwizzle<g_swizzle_y, g_swizzle_x, g_swizzle_x, g_swizzle_x>(M.r[2]);
			vec_float_t V1 = VectorSwizzle<g_swizzle_z, g_swizzle_z, g_swizzle_y, g_swizzle_y>(M.r[3]);
			vec_float_t V2 = VectorSwizzle<g_swizzle_y, g_swizzle_x, g_swizzle_x, g_swizzle_x>(M.r[2]);
			vec_float_t V3 = VectorSwizzle<g_swizzle_w, g_swizzle_w, g_swizzle_w, g_swizzle_z>(M.r[3]);
			vec_float_t V4 = VectorSwizzle<g_swizzle_z, g_swizzle_z, g_swizzle_y, g_swizzle_y>(M.r[2]);
			vec_float_t V5 = VectorSwizzle<g_swizzle_w, g_swizzle_w, g_swizzle_w, g_swizzle_z>(M.r[3]);

			vec_float_t P0 = VectorMultiply(V0, V1);
			vec_float_t P1 = VectorMultiply(V2, V3);
			vec_float_t P2 = VectorMultiply(V4, V5);

			V0 = VectorSwizzle<g_swizzle_z, g_swizzle_z, g_swizzle_y, g_swizzle_y>(M.r[2]);
			V1 = VectorSwizzle<g_swizzle_y, g_swizzle_x, g_swizzle_x, g_swizzle_x>(M.r[3]);
			V2 = VectorSwizzle<g_swizzle_w, g_swizzle_w, g_swizzle_w, g_swizzle_z>(M.r[2]);
			V3 = VectorSwizzle<g_swizzle_y, g_swizzle_x, g_swizzle_x, g_swizzle_x>(M.r[3]);
			V4 = VectorSwizzle<g_swizzle_w, g_swizzle_w, g_swizzle_w, g_swizzle_z>(M.r[2]);
			V5 = VectorSwizzle<g_swizzle_z, g_swizzle_z, g_swizzle_y, g_swizzle_y>(M.r[3]);

			P0 = VectorNegativeMultiplySubtract(V0, V1, P0);
			P1 = VectorNegativeMultiplySubtract(V2, V3, P1);
			P2 = VectorNegativeMultiplySubtract(V4, V5, P2);

			V0 = VectorSwizzle<g_swizzle_w, g_swizzle_w, g_swizzle_w, g_swizzle_z>(M.r[1]);
			V1 = VectorSwizzle<g_swizzle_z, g_swizzle_z, g_swizzle_y, g_swizzle_y>(M.r[1]);
			V2 = VectorSwizzle<g_swizzle_y, g_swizzle_x, g_swizzle_x, g_swizzle_x>(M.r[1]);

			vec_float_t S = VectorMultiply(M.r[0], Sign.v);
			vec_float_t R = VectorMultiply(V0, P0);
			R = VectorNegativeMultiplySubtract(V1, P1, R);
			R = VectorMultiplyAdd(V2, P2, R);

			return Vector4Dot(S, R);
		}

#define LUNA_SIMD_3RANKDECOMPOSE(a, b, c, x, y, z)      \
    if((x) < (y))                   \
    {                               \
        if((y) < (z))               \
        {                           \
            (a) = 2;                \
            (b) = 1;                \
            (c) = 0;                \
        }                           \
        else                        \
        {                           \
            (a) = 1;                \
                                    \
            if((x) < (z))           \
            {                       \
                (b) = 2;            \
                (c) = 0;            \
            }                       \
            else                    \
            {                       \
                (b) = 0;            \
                (c) = 2;            \
            }                       \
        }                           \
    }                               \
    else                            \
    {                               \
        if((x) < (z))               \
        {                           \
            (a) = 2;                \
            (b) = 0;                \
            (c) = 1;                \
        }                           \
        else                        \
        {                           \
            (a) = 0;                \
                                    \
            if((y) < (z))           \
            {                       \
                (b) = 2;            \
                (c) = 1;            \
            }                       \
            else                    \
            {                       \
                (b) = 1;            \
                (c) = 2;            \
            }                       \
        }                           \
    }

#define LUNA_SIMD_3_DECOMP_EPSILON 0.0001f


		inline bool LUNA_SIMD_CALLCONV MatrixDecompose
		(
			vec_float_t *outScale,
			vec_float_t *outRotQuat,
			vec_float_t *outTrans,
			mat_float_p1_t M
		)
		{
			static const vec_float_t *pvCanonicalBasis[3] = {
				&g_IdentityR0.v,
				&g_IdentityR1.v,
				&g_IdentityR2.v
			};

			luassert(outScale != nullptr);
			luassert(outRotQuat != nullptr);
			luassert(outTrans != nullptr);

			// Get the translation
			outTrans[0] = M.r[3];

			vec_float_t *ppvBasis[3];
			mat_float_t matTemp;
			ppvBasis[0] = &matTemp.r[0];
			ppvBasis[1] = &matTemp.r[1];
			ppvBasis[2] = &matTemp.r[2];

			matTemp.r[0] = M.r[0];
			matTemp.r[1] = M.r[1];
			matTemp.r[2] = M.r[2];
			matTemp.r[3] = g_IdentityR3.v;

			auto pfScales = reinterpret_cast<f32 *>(outScale);

			usize a, b, c;
			VectorGetXPtr(&pfScales[0], Vector3Length(ppvBasis[0][0]));
			VectorGetXPtr(&pfScales[1], Vector3Length(ppvBasis[1][0]));
			VectorGetXPtr(&pfScales[2], Vector3Length(ppvBasis[2][0]));
			pfScales[3] = 0.f;

			LUNA_SIMD_3RANKDECOMPOSE(a, b, c, pfScales[0], pfScales[1], pfScales[2])

				if (pfScales[a] < LUNA_SIMD_3_DECOMP_EPSILON)
				{
					ppvBasis[a][0] = pvCanonicalBasis[a][0];
				}
			ppvBasis[a][0] = Vector3Normalize(ppvBasis[a][0]);

			if (pfScales[b] < LUNA_SIMD_3_DECOMP_EPSILON)
			{
				usize aa, bb, cc;
				f32 fAbsX, fAbsY, fAbsZ;

				fAbsX = fabsf(VectorGetX(ppvBasis[a][0]));
				fAbsY = fabsf(VectorGetY(ppvBasis[a][0]));
				fAbsZ = fabsf(VectorGetZ(ppvBasis[a][0]));

				LUNA_SIMD_3RANKDECOMPOSE(aa, bb, cc, fAbsX, fAbsY, fAbsZ)

					ppvBasis[b][0] = Vector3Cross(ppvBasis[a][0], pvCanonicalBasis[cc][0]);
			}

			ppvBasis[b][0] = Vector3Normalize(ppvBasis[b][0]);

			if (pfScales[c] < LUNA_SIMD_3_DECOMP_EPSILON)
			{
				ppvBasis[c][0] = Vector3Cross(ppvBasis[a][0], ppvBasis[b][0]);
			}

			ppvBasis[c][0] = Vector3Normalize(ppvBasis[c][0]);

			f32 fDet = VectorGetX(MatrixDeterminant(matTemp));

			// use Kramer's rule to check for handedness of coordinate system
			if (fDet < 0.0f)
			{
				// switch coordinate system by negating the scale and inverting the basis vector on the x-axis
				pfScales[a] = -pfScales[a];
				ppvBasis[a][0] = VectorNegate(ppvBasis[a][0]);

				fDet = -fDet;
			}

			fDet -= 1.0f;
			fDet *= fDet;

			if (LUNA_SIMD_3_DECOMP_EPSILON < fDet)
			{
				// Non-SRT matrix encountered
				return false;
			}

			// generate the Quaternion from the matrix
			outRotQuat[0] = QuaternionRotationMatrix(matTemp);
			return true;
		}

#undef LUNA_SIMD_3_DECOMP_EPSILON
#undef LUNA_SIMD_3RANKDECOMPOSE

		//------------------------------------------------------------------------------
		// Transformation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixIdentity()
		{
			mat_float_t M;
			M.r[0] = g_IdentityR0.v;
			M.r[1] = g_IdentityR1.v;
			M.r[2] = g_IdentityR2.v;
			M.r[3] = g_IdentityR3.v;
			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixSet
		(
			f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33
		)
		{
			mat_float_t M;
			M.r[0] = vector_set(m00, m01, m02, m03);
			M.r[1] = vector_set(m10, m11, m12, m13);
			M.r[2] = vector_set(m20, m21, m22, m23);
			M.r[3] = vector_set(m30, m31, m32, m33);
			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixTranslation
		(
			f32 OffsetX,
			f32 OffsetY,
			f32 OffsetZ
		)
		{
#if defined(LUNA_PLATFORM_SSE2) || defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t M;
			M.r[0] = g_IdentityR0.v;
			M.r[1] = g_IdentityR1.v;
			M.r[2] = g_IdentityR2.v;
			M.r[3] = vector_set(OffsetX, OffsetY, OffsetZ, 1.f);
			return M;
#endif
		}


		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixTranslationFromVector
		(
			vec_float_p1_t Offset
		)
		{
#if defined(LUNA_PLATFORM_SSE2) || defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t M;
			M.r[0] = g_IdentityR0.v;
			M.r[1] = g_IdentityR1.v;
			M.r[2] = g_IdentityR2.v;
			M.r[3] = VectorSelect(g_IdentityR3.v, Offset, g_Select1110.v);
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixScaling
		(
			f32 ScaleX,
			f32 ScaleY,
			f32 ScaleZ
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			const vec_float_t Zero = vdupq_n_f32(0);
			mat_float_t M;
			M.r[0] = vsetq_lane_f32(ScaleX, Zero, 0);
			M.r[1] = vsetq_lane_f32(ScaleY, Zero, 1);
			M.r[2] = vsetq_lane_f32(ScaleZ, Zero, 2);
			M.r[3] = g_IdentityR3.v;
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			M.r[0] = _mm_set_ps(0, 0, 0, ScaleX);
			M.r[1] = _mm_set_ps(0, 0, ScaleY, 0);
			M.r[2] = _mm_set_ps(0, ScaleZ, 0, 0);
			M.r[3] = g_IdentityR3.v;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixScalingFromVector
		(
			vec_float_p1_t Scale
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			mat_float_t M;
			M.r[0] = vandq_u32(Scale, g_MaskX);
			M.r[1] = vandq_u32(Scale, g_MaskY);
			M.r[2] = vandq_u32(Scale, g_MaskZ);
			M.r[3] = g_IdentityR3.v;
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			M.r[0] = _mm_and_ps(Scale, g_MaskX);
			M.r[1] = _mm_and_ps(Scale, g_MaskY);
			M.r[2] = _mm_and_ps(Scale, g_MaskZ);
			M.r[3] = g_IdentityR3.v;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationX
		(
			f32 Angle
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			f32    fSinAngle;
			f32    fCosAngle;
			ScalarSinCos(&fSinAngle, &fCosAngle, Angle);

			const vec_float_t Zero = vdupq_n_f32(0);

			vec_float_t T1 = vsetq_lane_f32(fCosAngle, Zero, 1);
			T1 = vsetq_lane_f32(fSinAngle, T1, 2);

			vec_float_t T2 = vsetq_lane_f32(-fSinAngle, Zero, 1);
			T2 = vsetq_lane_f32(fCosAngle, T2, 2);

			mat_float_t M;
			M.r[0] = g_IdentityR0.v;
			M.r[1] = T1;
			M.r[2] = T2;
			M.r[3] = g_IdentityR3.v;
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			f32    SinAngle;
			f32    CosAngle;
			ScalarSinCos(&SinAngle, &CosAngle, Angle);

			vec_float_t vSin = _mm_set_ss(SinAngle);
			vec_float_t vCos = _mm_set_ss(CosAngle);
			// x = 0,y = cos,z = sin, w = 0
			vCos = _mm_shuffle_ps(vCos, vSin, _MM_SHUFFLE(3, 0, 0, 3));
			mat_float_t M;
			M.r[0] = g_IdentityR0;
			M.r[1] = vCos;
			// x = 0,y = sin,z = cos, w = 0
			vCos = LUNA_SIMD_PERMUTE_PS(vCos, _MM_SHUFFLE(3, 1, 2, 0));
			// x = 0,y = -sin,z = cos, w = 0
			vCos = _mm_mul_ps(vCos, g_NegateY);
			M.r[2] = vCos;
			M.r[3] = g_IdentityR3;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationY
		(
			f32 Angle
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			f32    fSinAngle;
			f32    fCosAngle;
			ScalarSinCos(&fSinAngle, &fCosAngle, Angle);

			const vec_float_t Zero = vdupq_n_f32(0);

			vec_float_t T0 = vsetq_lane_f32(fCosAngle, Zero, 0);
			T0 = vsetq_lane_f32(-fSinAngle, T0, 2);

			vec_float_t T2 = vsetq_lane_f32(fSinAngle, Zero, 0);
			T2 = vsetq_lane_f32(fCosAngle, T2, 2);

			mat_float_t M;
			M.r[0] = T0;
			M.r[1] = g_IdentityR1.v;
			M.r[2] = T2;
			M.r[3] = g_IdentityR3.v;
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			f32    SinAngle;
			f32    CosAngle;
			ScalarSinCos(&SinAngle, &CosAngle, Angle);

			vec_float_t vSin = _mm_set_ss(SinAngle);
			vec_float_t vCos = _mm_set_ss(CosAngle);
			// x = sin,y = 0,z = cos, w = 0
			vSin = _mm_shuffle_ps(vSin, vCos, _MM_SHUFFLE(3, 0, 3, 0));
			mat_float_t M;
			M.r[2] = vSin;
			M.r[1] = g_IdentityR1;
			// x = cos,y = 0,z = sin, w = 0
			vSin = LUNA_SIMD_PERMUTE_PS(vSin, _MM_SHUFFLE(3, 0, 1, 2));
			// x = cos,y = 0,z = -sin, w = 0
			vSin = _mm_mul_ps(vSin, g_NegateZ);
			M.r[0] = vSin;
			M.r[3] = g_IdentityR3;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationZ
		(
			f32 Angle
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			f32    fSinAngle;
			f32    fCosAngle;
			ScalarSinCos(&fSinAngle, &fCosAngle, Angle);

			const vec_float_t Zero = vdupq_n_f32(0);

			vec_float_t T0 = vsetq_lane_f32(fCosAngle, Zero, 0);
			T0 = vsetq_lane_f32(fSinAngle, T0, 1);

			vec_float_t T1 = vsetq_lane_f32(-fSinAngle, Zero, 0);
			T1 = vsetq_lane_f32(fCosAngle, T1, 1);

			mat_float_t M;
			M.r[0] = T0;
			M.r[1] = T1;
			M.r[2] = g_IdentityR2.v;
			M.r[3] = g_IdentityR3.v;
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			f32    SinAngle;
			f32    CosAngle;
			ScalarSinCos(&SinAngle, &CosAngle, Angle);

			vec_float_t vSin = _mm_set_ss(SinAngle);
			vec_float_t vCos = _mm_set_ss(CosAngle);
			// x = cos,y = sin,z = 0, w = 0
			vCos = _mm_unpacklo_ps(vCos, vSin);
			mat_float_t M;
			M.r[0] = vCos;
			// x = sin,y = cos,z = 0, w = 0
			vCos = LUNA_SIMD_PERMUTE_PS(vCos, _MM_SHUFFLE(3, 2, 0, 1));
			// x = cos,y = -sin,z = 0, w = 0
			vCos = _mm_mul_ps(vCos, g_NegateX);
			M.r[1] = vCos;
			M.r[2] = g_IdentityR2;
			M.r[3] = g_IdentityR3;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationRollPitchYaw
		(
			f32 Pitch,
			f32 Yaw,
			f32 Roll
		)
		{
			vec_float_t Angles = vector_set(Pitch, Yaw, Roll, 0.0f);
			return MatrixRotationRollPitchYawFromVector(Angles);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationRollPitchYawFromVector
		(
			vec_float_p1_t Angles // <Pitch, Yaw, Roll, undefined>
		)
		{
			vec_float_t Q = QuaternionRotationRollPitchYawFromVector(Angles);
			return MatrixRotationQuaternion(Q);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationNormal
		(
			vec_float_p1_t NormalAxis,
			f32     Angle
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)

			f32    fSinAngle;
			f32    fCosAngle;
			ScalarSinCos(&fSinAngle, &fCosAngle, Angle);

			vec_float_t A = vector_set(fSinAngle, fCosAngle, 1.0f - fCosAngle, 0.0f);

			vec_float_t C2 = vector_splat_z(A);
			vec_float_t C1 = vector_splat_y(A);
			vec_float_t C0 = vector_splat_x(A);

			vec_float_t N0 = VectorSwizzle<g_swizzle_y, g_swizzle_z, g_swizzle_x, g_swizzle_w>(NormalAxis);
			vec_float_t N1 = VectorSwizzle<g_swizzle_z, g_swizzle_x, g_swizzle_y, g_swizzle_w>(NormalAxis);

			vec_float_t V0 = VectorMultiply(C2, N0);
			V0 = VectorMultiply(V0, N1);

			vec_float_t R0 = VectorMultiply(C2, NormalAxis);
			R0 = VectorMultiplyAdd(R0, NormalAxis, C1);

			vec_float_t R1 = VectorMultiplyAdd(C0, NormalAxis, V0);
			vec_float_t R2 = VectorNegativeMultiplySubtract(C0, NormalAxis, V0);

			V0 = VectorSelect(A, R0, g_Select1110.v);
			vec_float_t V1 = VectorPermute<g_permute_0z, g_permute_1y, g_permute_1z, g_permute_0x>(R1, R2);
			vec_float_t V2 = VectorPermute<g_permute_0y, g_permute_1x, g_permute_0y, g_permute_1x>(R1, R2);

			mat_float_t M;
			M.r[0] = VectorPermute<g_permute_0x, g_permute_1x, g_permute_1y, g_permute_0w>(V0, V1);
			M.r[1] = VectorPermute<g_permute_1z, g_permute_0y, g_permute_1w, g_permute_0w>(V0, V1);
			M.r[2] = VectorPermute<g_permute_1x, g_permute_1y, g_permute_0z, g_permute_0w>(V0, V2);
			M.r[3] = g_IdentityR3.v;
			return M;

#elif defined(LUNA_PLATFORM_SSE2)
			f32    fSinAngle;
			f32    fCosAngle;
			ScalarSinCos(&fSinAngle, &fCosAngle, Angle);

			vec_float_t C2 = _mm_set_ps1(1.0f - fCosAngle);
			vec_float_t C1 = _mm_set_ps1(fCosAngle);
			vec_float_t C0 = _mm_set_ps1(fSinAngle);

			vec_float_t N0 = LUNA_SIMD_PERMUTE_PS(NormalAxis, _MM_SHUFFLE(3, 0, 2, 1));
			vec_float_t N1 = LUNA_SIMD_PERMUTE_PS(NormalAxis, _MM_SHUFFLE(3, 1, 0, 2));

			vec_float_t V0 = _mm_mul_ps(C2, N0);
			V0 = _mm_mul_ps(V0, N1);

			vec_float_t R0 = _mm_mul_ps(C2, NormalAxis);
			R0 = _mm_mul_ps(R0, NormalAxis);
			R0 = _mm_add_ps(R0, C1);

			vec_float_t R1 = _mm_mul_ps(C0, NormalAxis);
			R1 = _mm_add_ps(R1, V0);
			vec_float_t R2 = _mm_mul_ps(C0, NormalAxis);
			R2 = _mm_sub_ps(V0, R2);

			V0 = _mm_and_ps(R0, g_Mask3);
			vec_float_t V1 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(2, 1, 2, 0));
			V1 = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(0, 3, 2, 1));
			vec_float_t V2 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(0, 0, 1, 1));
			V2 = LUNA_SIMD_PERMUTE_PS(V2, _MM_SHUFFLE(2, 0, 2, 0));

			R2 = _mm_shuffle_ps(V0, V1, _MM_SHUFFLE(1, 0, 3, 0));
			R2 = LUNA_SIMD_PERMUTE_PS(R2, _MM_SHUFFLE(1, 3, 2, 0));

			mat_float_t M;
			M.r[0] = R2;

			R2 = _mm_shuffle_ps(V0, V1, _MM_SHUFFLE(3, 2, 3, 1));
			R2 = LUNA_SIMD_PERMUTE_PS(R2, _MM_SHUFFLE(1, 3, 0, 2));
			M.r[1] = R2;

			V2 = _mm_shuffle_ps(V2, V0, _MM_SHUFFLE(3, 2, 1, 0));
			M.r[2] = V2;
			M.r[3] = g_IdentityR3.v;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationAxis
		(
			vec_float_p1_t Axis,
			f32     Angle
		)
		{
			luassert(!Vector3Equal(Axis, vector_zero()));
			luassert(!Vector3IsInfinite(Axis));

			vec_float_t Normal = Vector3Normalize(Axis);
			return MatrixRotationNormal(Normal, Angle);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixRotationQuaternion
		(
			vec_float_p1_t Quaternion
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)

			static const vec_f32_t Constant1110 = { { { 1.0f, 1.0f, 1.0f, 0.0f } } };

			vec_float_t Q0 = VectorAdd(Quaternion, Quaternion);
			vec_float_t Q1 = VectorMultiply(Quaternion, Q0);

			vec_float_t V0 = VectorPermute<g_permute_0y, g_permute_0x, g_permute_0x, g_permute_1w>(Q1, Constant1110.v);
			vec_float_t V1 = VectorPermute<g_permute_0z, g_permute_0z, g_permute_0y, g_permute_1w>(Q1, Constant1110.v);
			vec_float_t R0 = VectorSubtract(Constant1110, V0);
			R0 = VectorSubtract(R0, V1);

			V0 = VectorSwizzle<g_swizzle_x, g_swizzle_x, g_swizzle_y, g_swizzle_w>(Quaternion);
			V1 = VectorSwizzle<g_swizzle_z, g_swizzle_y, g_swizzle_z, g_swizzle_w>(Q0);
			V0 = VectorMultiply(V0, V1);

			V1 = vector_splat_w(Quaternion);
			vec_float_t V2 = VectorSwizzle<g_swizzle_y, g_swizzle_z, g_swizzle_x, g_swizzle_w>(Q0);
			V1 = VectorMultiply(V1, V2);

			vec_float_t R1 = VectorAdd(V0, V1);
			vec_float_t R2 = VectorSubtract(V0, V1);

			V0 = VectorPermute<g_permute_0y, g_permute_1x, g_permute_1y, g_permute_0z>(R1, R2);
			V1 = VectorPermute<g_permute_0x, g_permute_1z, g_permute_0x, g_permute_1z>(R1, R2);

			mat_float_t M;
			M.r[0] = VectorPermute<g_permute_0x, g_permute_1x, g_permute_1y, g_permute_0w>(R0, V0);
			M.r[1] = VectorPermute<g_permute_1z, g_permute_0y, g_permute_1w, g_permute_0w>(R0, V0);
			M.r[2] = VectorPermute<g_permute_1x, g_permute_1y, g_permute_0z, g_permute_0w>(R0, V1);
			M.r[3] = g_IdentityR3.v;
			return M;

#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t  Constant1110 = { { { 1.0f, 1.0f, 1.0f, 0.0f } } };

			vec_float_t Q0 = _mm_add_ps(Quaternion, Quaternion);
			vec_float_t Q1 = _mm_mul_ps(Quaternion, Q0);

			vec_float_t V0 = LUNA_SIMD_PERMUTE_PS(Q1, _MM_SHUFFLE(3, 0, 0, 1));
			V0 = _mm_and_ps(V0, g_Mask3);
			vec_float_t V1 = LUNA_SIMD_PERMUTE_PS(Q1, _MM_SHUFFLE(3, 1, 2, 2));
			V1 = _mm_and_ps(V1, g_Mask3);
			vec_float_t R0 = _mm_sub_ps(Constant1110, V0);
			R0 = _mm_sub_ps(R0, V1);

			V0 = LUNA_SIMD_PERMUTE_PS(Quaternion, _MM_SHUFFLE(3, 1, 0, 0));
			V1 = LUNA_SIMD_PERMUTE_PS(Q0, _MM_SHUFFLE(3, 2, 1, 2));
			V0 = _mm_mul_ps(V0, V1);

			V1 = LUNA_SIMD_PERMUTE_PS(Quaternion, _MM_SHUFFLE(3, 3, 3, 3));
			vec_float_t V2 = LUNA_SIMD_PERMUTE_PS(Q0, _MM_SHUFFLE(3, 0, 2, 1));
			V1 = _mm_mul_ps(V1, V2);

			vec_float_t R1 = _mm_add_ps(V0, V1);
			vec_float_t R2 = _mm_sub_ps(V0, V1);

			V0 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(1, 0, 2, 1));
			V0 = LUNA_SIMD_PERMUTE_PS(V0, _MM_SHUFFLE(1, 3, 2, 0));
			V1 = _mm_shuffle_ps(R1, R2, _MM_SHUFFLE(2, 2, 0, 0));
			V1 = LUNA_SIMD_PERMUTE_PS(V1, _MM_SHUFFLE(2, 0, 2, 0));

			Q1 = _mm_shuffle_ps(R0, V0, _MM_SHUFFLE(1, 0, 3, 0));
			Q1 = LUNA_SIMD_PERMUTE_PS(Q1, _MM_SHUFFLE(1, 3, 2, 0));

			mat_float_t M;
			M.r[0] = Q1;

			Q1 = _mm_shuffle_ps(R0, V0, _MM_SHUFFLE(3, 2, 3, 1));
			Q1 = LUNA_SIMD_PERMUTE_PS(Q1, _MM_SHUFFLE(1, 3, 0, 2));
			M.r[1] = Q1;

			Q1 = _mm_shuffle_ps(V1, R0, _MM_SHUFFLE(3, 2, 1, 0));
			M.r[2] = Q1;
			M.r[3] = g_IdentityR3;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixTransformation2D
		(
			vec_float_p1_t ScalingOrigin,
			f32     ScalingOrientation,
			vec_float_p1_t Scaling,
			vec_float_p1_t RotationOrigin,
			f32     Rotation,
			vec_float_p2_t Translation
		)
		{
			// M = Inverse(MScalingOrigin) * Transpose(MScalingOrientation) * MScaling * MScalingOrientation *
			//         MScalingOrigin * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

			vec_float_t VScalingOrigin = VectorSelect(g_Select1100.v, ScalingOrigin, g_Select1100.v);
			vec_float_t NegScalingOrigin = VectorNegate(VScalingOrigin);

			mat_float_t MScalingOriginI = MatrixTranslationFromVector(NegScalingOrigin);
			mat_float_t MScalingOrientation = MatrixRotationZ(ScalingOrientation);
			mat_float_t MScalingOrientationT = MatrixTranspose(MScalingOrientation);
			vec_float_t VScaling = VectorSelect(g_One.v, Scaling, g_Select1100.v);
			mat_float_t MScaling = MatrixScalingFromVector(VScaling);
			vec_float_t VRotationOrigin = VectorSelect(g_Select1100.v, RotationOrigin, g_Select1100.v);
			mat_float_t MRotation = MatrixRotationZ(Rotation);
			vec_float_t VTranslation = VectorSelect(g_Select1100.v, Translation, g_Select1100.v);

			mat_float_t M = MatrixMultiply(MScalingOriginI, MScalingOrientationT);
			M = MatrixMultiply(M, MScaling);
			M = MatrixMultiply(M, MScalingOrientation);
			M.r[3] = VectorAdd(M.r[3], VScalingOrigin);
			M.r[3] = VectorSubtract(M.r[3], VRotationOrigin);
			M = MatrixMultiply(M, MRotation);
			M.r[3] = VectorAdd(M.r[3], VRotationOrigin);
			M.r[3] = VectorAdd(M.r[3], VTranslation);

			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixTransformation
		(
			vec_float_p1_t ScalingOrigin,
			vec_float_p1_t ScalingOrientationQuaternion,
			vec_float_p1_t Scaling,
			vec_float_p2_t RotationOrigin,
			vec_float_p3_t RotationQuaternion,
			vec_float_p3_t Translation
		)
		{
			// M = Inverse(MScalingOrigin) * Transpose(MScalingOrientation) * MScaling * MScalingOrientation *
			//         MScalingOrigin * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

			vec_float_t VScalingOrigin = VectorSelect(g_Select1110.v, ScalingOrigin, g_Select1110.v);
			vec_float_t NegScalingOrigin = VectorNegate(ScalingOrigin);

			mat_float_t MScalingOriginI = MatrixTranslationFromVector(NegScalingOrigin);
			mat_float_t MScalingOrientation = MatrixRotationQuaternion(ScalingOrientationQuaternion);
			mat_float_t MScalingOrientationT = MatrixTranspose(MScalingOrientation);
			mat_float_t MScaling = MatrixScalingFromVector(Scaling);
			vec_float_t VRotationOrigin = VectorSelect(g_Select1110.v, RotationOrigin, g_Select1110.v);
			mat_float_t MRotation = MatrixRotationQuaternion(RotationQuaternion);
			vec_float_t VTranslation = VectorSelect(g_Select1110.v, Translation, g_Select1110.v);

			mat_float_t M;
			M = MatrixMultiply(MScalingOriginI, MScalingOrientationT);
			M = MatrixMultiply(M, MScaling);
			M = MatrixMultiply(M, MScalingOrientation);
			M.r[3] = VectorAdd(M.r[3], VScalingOrigin);
			M.r[3] = VectorSubtract(M.r[3], VRotationOrigin);
			M = MatrixMultiply(M, MRotation);
			M.r[3] = VectorAdd(M.r[3], VRotationOrigin);
			M.r[3] = VectorAdd(M.r[3], VTranslation);
			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixAffineTransformation2D
		(
			vec_float_p1_t Scaling,
			vec_float_p1_t RotationOrigin,
			f32     Rotation,
			vec_float_p1_t Translation
		)
		{
			// M = MScaling * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

			vec_float_t VScaling = VectorSelect(g_One.v, Scaling, g_Select1100.v);
			mat_float_t MScaling = MatrixScalingFromVector(VScaling);
			vec_float_t VRotationOrigin = VectorSelect(g_Select1100.v, RotationOrigin, g_Select1100.v);
			mat_float_t MRotation = MatrixRotationZ(Rotation);
			vec_float_t VTranslation = VectorSelect(g_Select1100.v, Translation, g_Select1100.v);

			mat_float_t M;
			M = MScaling;
			M.r[3] = VectorSubtract(M.r[3], VRotationOrigin);
			M = MatrixMultiply(M, MRotation);
			M.r[3] = VectorAdd(M.r[3], VRotationOrigin);
			M.r[3] = VectorAdd(M.r[3], VTranslation);
			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixAffineTransformation
		(
			vec_float_p1_t Scaling,
			vec_float_p1_t RotationOrigin,
			vec_float_p1_t RotationQuaternion,
			vec_float_p2_t Translation
		)
		{
			// M = MScaling * Inverse(MRotationOrigin) * MRotation * MRotationOrigin * MTranslation;

			mat_float_t MScaling = MatrixScalingFromVector(Scaling);
			vec_float_t VRotationOrigin = VectorSelect(g_Select1110.v, RotationOrigin, g_Select1110.v);
			mat_float_t MRotation = MatrixRotationQuaternion(RotationQuaternion);
			vec_float_t VTranslation = VectorSelect(g_Select1110.v, Translation, g_Select1110.v);

			mat_float_t M;
			M = MScaling;
			M.r[3] = VectorSubtract(M.r[3], VRotationOrigin);
			M = MatrixMultiply(M, MRotation);
			M.r[3] = VectorAdd(M.r[3], VRotationOrigin);
			M.r[3] = VectorAdd(M.r[3], VTranslation);
			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixReflect
		(
			vec_float_p1_t ReflectionPlane
		)
		{
			luassert(!Vector3Equal(ReflectionPlane, vector_zero()));
			luassert(!PlaneIsInfinite(ReflectionPlane));

			static const vec_f32_t NegativeTwo = { { { -2.0f, -2.0f, -2.0f, 0.0f } } };

			vec_float_t P = PlaneNormalize(ReflectionPlane);
			vec_float_t S = VectorMultiply(P, NegativeTwo);

			vec_float_t A = vector_splat_x(P);
			vec_float_t B = vector_splat_y(P);
			vec_float_t C = vector_splat_z(P);
			vec_float_t D = vector_splat_w(P);

			mat_float_t M;
			M.r[0] = VectorMultiplyAdd(A, S, g_IdentityR0.v);
			M.r[1] = VectorMultiplyAdd(B, S, g_IdentityR1.v);
			M.r[2] = VectorMultiplyAdd(C, S, g_IdentityR2.v);
			M.r[3] = VectorMultiplyAdd(D, S, g_IdentityR3.v);
			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixShadow
		(
			vec_float_p1_t ShadowPlane,
			vec_float_p1_t LightPosition
		)
		{
			static const vec_u32_t Select0001 = { { { g_select_0, g_select_0, g_select_0, g_select_1 } } };

			luassert(!Vector3Equal(ShadowPlane, vector_zero()));
			luassert(!PlaneIsInfinite(ShadowPlane));

			vec_float_t P = PlaneNormalize(ShadowPlane);
			vec_float_t dot = PlaneDot(P, LightPosition);
			P = VectorNegate(P);
			vec_float_t D = vector_splat_w(P);
			vec_float_t C = vector_splat_z(P);
			vec_float_t B = vector_splat_y(P);
			vec_float_t A = vector_splat_x(P);
			dot = VectorSelect(Select0001.v, dot, Select0001.v);

			mat_float_t M;
			M.r[3] = VectorMultiplyAdd(D, LightPosition, dot);
			dot = VectorRotateLeft(dot, 1);
			M.r[2] = VectorMultiplyAdd(C, LightPosition, dot);
			dot = VectorRotateLeft(dot, 1);
			M.r[1] = VectorMultiplyAdd(B, LightPosition, dot);
			dot = VectorRotateLeft(dot, 1);
			M.r[0] = VectorMultiplyAdd(A, LightPosition, dot);
			return M;
		}

		//------------------------------------------------------------------------------
		// View and projection initialization operations
		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixLookAtLH
		(
			vec_float_p1_t EyePosition,
			vec_float_p1_t FocusPosition,
			vec_float_p1_t UpDirection
		)
		{
			vec_float_t EyeDirection = VectorSubtract(FocusPosition, EyePosition);
			return MatrixLookToLH(EyePosition, EyeDirection, UpDirection);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixLookAtRH
		(
			vec_float_p1_t EyePosition,
			vec_float_p1_t FocusPosition,
			vec_float_p1_t UpDirection
		)
		{
			vec_float_t NegEyeDirection = VectorSubtract(EyePosition, FocusPosition);
			return MatrixLookToLH(EyePosition, NegEyeDirection, UpDirection);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixLookToLH
		(
			vec_float_p1_t EyePosition,
			vec_float_p1_t EyeDirection,
			vec_float_p1_t UpDirection
		)
		{
			luassert(!Vector3Equal(EyeDirection, vector_zero()));
			luassert(!Vector3IsInfinite(EyeDirection));
			luassert(!Vector3Equal(UpDirection, vector_zero()));
			luassert(!Vector3IsInfinite(UpDirection));

			vec_float_t R2 = Vector3Normalize(EyeDirection);

			vec_float_t R0 = Vector3Cross(UpDirection, R2);
			R0 = Vector3Normalize(R0);

			vec_float_t R1 = Vector3Cross(R2, R0);

			vec_float_t NegEyePosition = VectorNegate(EyePosition);

			vec_float_t D0 = Vector3Dot(R0, NegEyePosition);
			vec_float_t D1 = Vector3Dot(R1, NegEyePosition);
			vec_float_t D2 = Vector3Dot(R2, NegEyePosition);

			mat_float_t M;
			M.r[0] = VectorSelect(D0, R0, g_Select1110.v);
			M.r[1] = VectorSelect(D1, R1, g_Select1110.v);
			M.r[2] = VectorSelect(D2, R2, g_Select1110.v);
			M.r[3] = g_IdentityR3.v;

			M = MatrixTranspose(M);

			return M;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixLookToRH
		(
			vec_float_p1_t EyePosition,
			vec_float_p1_t EyeDirection,
			vec_float_p1_t UpDirection
		)
		{
			vec_float_t NegEyeDirection = VectorNegate(EyeDirection);
			return MatrixLookToLH(EyePosition, NegEyeDirection, UpDirection);
		}

		//------------------------------------------------------------------------------

#ifdef _PREFAST_
#pragma prefast(push)
#pragma prefast(disable:28931, "PREfast noise: Esp:1266")
#endif

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixPerspectiveLH
		(
			f32 ViewWidth,
			f32 ViewHeight,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(NearZ > 0.f && FarZ > 0.f);
			luassert(!ScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 TwoNearZ = NearZ + NearZ;
			f32 fRange = FarZ / (FarZ - NearZ);
			const vec_float_t Zero = vdupq_n_f32(0);
			mat_float_t M;
			M.r[0] = vsetq_lane_f32(TwoNearZ / ViewWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(TwoNearZ / ViewHeight, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, g_IdentityR3.v, 2);
			M.r[3] = vsetq_lane_f32(-fRange * NearZ, Zero, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 TwoNearZ = NearZ + NearZ;
			f32 fRange = FarZ / (FarZ - NearZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				TwoNearZ / ViewWidth,
				TwoNearZ / ViewHeight,
				fRange,
				-fRange * NearZ
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// TwoNearZ / ViewWidth,0,0,0
			M.r[0] = vTemp;
			// 0,TwoNearZ / ViewHeight,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// x=fRange,y=-fRange * NearZ,0,1.0f
			vValues = _mm_shuffle_ps(vValues, g_IdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
			// 0,0,fRange,1.0f
			vTemp = _mm_setzero_ps();
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
			M.r[2] = vTemp;
			// 0,0,-fRange * NearZ,0
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
			M.r[3] = vTemp;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixPerspectiveRH
		(
			f32 ViewWidth,
			f32 ViewHeight,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(NearZ > 0.f && FarZ > 0.f);
			luassert(!ScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 TwoNearZ = NearZ + NearZ;
			f32 fRange = FarZ / (NearZ - FarZ);
			const vec_float_t Zero = vdupq_n_f32(0);

			mat_float_t M;
			M.r[0] = vsetq_lane_f32(TwoNearZ / ViewWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(TwoNearZ / ViewHeight, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, g_NegIdentityR3.v, 2);
			M.r[3] = vsetq_lane_f32(fRange * NearZ, Zero, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 TwoNearZ = NearZ + NearZ;
			f32 fRange = FarZ / (NearZ - FarZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				TwoNearZ / ViewWidth,
				TwoNearZ / ViewHeight,
				fRange,
				fRange * NearZ
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// TwoNearZ / ViewWidth,0,0,0
			M.r[0] = vTemp;
			// 0,TwoNearZ / ViewHeight,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// x=fRange,y=-fRange * NearZ,0,-1.0f
			vValues = _mm_shuffle_ps(vValues, g_NegIdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
			// 0,0,fRange,-1.0f
			vTemp = _mm_setzero_ps();
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
			M.r[2] = vTemp;
			// 0,0,-fRange * NearZ,0
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
			M.r[3] = vTemp;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixPerspectiveFovLH
		(
			f32 FovAngleY,
			f32 AspectRatio,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(NearZ > 0.f && FarZ > 0.f);
			luassert(!ScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
			luassert(!ScalarNearEqual(AspectRatio, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32    SinFov;
			f32    CosFov;
			ScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

			f32 fRange = FarZ / (FarZ - NearZ);
			f32 Height = CosFov / SinFov;
			f32 Width = Height / AspectRatio;
			const vec_float_t Zero = vdupq_n_f32(0);

			mat_float_t M;
			M.r[0] = vsetq_lane_f32(Width, Zero, 0);
			M.r[1] = vsetq_lane_f32(Height, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, g_IdentityR3.v, 2);
			M.r[3] = vsetq_lane_f32(-fRange * NearZ, Zero, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			f32    SinFov;
			f32    CosFov;
			ScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

			f32 fRange = FarZ / (FarZ - NearZ);
			// Note: This is recorded on the stack
			f32 Height = CosFov / SinFov;
			vec_float_t rMem = {
				Height / AspectRatio,
				Height,
				fRange,
				-fRange * NearZ
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// CosFov / SinFov,0,0,0
			mat_float_t M;
			M.r[0] = vTemp;
			// 0,Height / AspectRatio,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// x=fRange,y=-fRange * NearZ,0,1.0f
			vTemp = _mm_setzero_ps();
			vValues = _mm_shuffle_ps(vValues, g_IdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
			// 0,0,fRange,1.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
			M.r[2] = vTemp;
			// 0,0,-fRange * NearZ,0.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
			M.r[3] = vTemp;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixPerspectiveFovRH
		(
			f32 FovAngleY,
			f32 AspectRatio,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(NearZ > 0.f && FarZ > 0.f);
			luassert(!ScalarNearEqual(FovAngleY, 0.0f, 0.00001f * 2.0f));
			luassert(!ScalarNearEqual(AspectRatio, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32    SinFov;
			f32    CosFov;
			ScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
			f32 fRange = FarZ / (NearZ - FarZ);
			f32 Height = CosFov / SinFov;
			f32 Width = Height / AspectRatio;
			const vec_float_t Zero = vdupq_n_f32(0);

			mat_float_t M;
			M.r[0] = vsetq_lane_f32(Width, Zero, 0);
			M.r[1] = vsetq_lane_f32(Height, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, g_NegIdentityR3.v, 2);
			M.r[3] = vsetq_lane_f32(fRange * NearZ, Zero, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			f32    SinFov;
			f32    CosFov;
			ScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);
			f32 fRange = FarZ / (NearZ - FarZ);
			// Note: This is recorded on the stack
			f32 Height = CosFov / SinFov;
			vec_float_t rMem = {
				Height / AspectRatio,
				Height,
				fRange,
				fRange * NearZ
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// CosFov / SinFov,0,0,0
			mat_float_t M;
			M.r[0] = vTemp;
			// 0,Height / AspectRatio,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// x=fRange,y=-fRange * NearZ,0,-1.0f
			vTemp = _mm_setzero_ps();
			vValues = _mm_shuffle_ps(vValues, g_NegIdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
			// 0,0,fRange,-1.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
			M.r[2] = vTemp;
			// 0,0,fRange * NearZ,0.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
			M.r[3] = vTemp;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixPerspectiveOffCenterLH
		(
			f32 ViewLeft,
			f32 ViewRight,
			f32 ViewBottom,
			f32 ViewTop,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(NearZ > 0.f && FarZ > 0.f);
			luassert(!ScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
			luassert(!ScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 TwoNearZ = NearZ + NearZ;
			f32 ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = FarZ / (FarZ - NearZ);
			const vec_float_t Zero = vdupq_n_f32(0);

			mat_float_t M;
			M.r[0] = vsetq_lane_f32(TwoNearZ * ReciprocalWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(TwoNearZ * ReciprocalHeight, Zero, 1);
			M.r[2] = vector_set(-(ViewLeft + ViewRight) * ReciprocalWidth,
				-(ViewTop + ViewBottom) * ReciprocalHeight,
				fRange,
				1.0f);
			M.r[3] = vsetq_lane_f32(-fRange * NearZ, Zero, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 TwoNearZ = NearZ + NearZ;
			f32 ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = FarZ / (FarZ - NearZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				TwoNearZ*ReciprocalWidth,
				TwoNearZ*ReciprocalHeight,
				-fRange * NearZ,
				0
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// TwoNearZ*ReciprocalWidth,0,0,0
			M.r[0] = vTemp;
			// 0,TwoNearZ*ReciprocalHeight,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// 0,0,fRange,1.0f
			M.r[2] = vector_set(-(ViewLeft + ViewRight) * ReciprocalWidth,
				-(ViewTop + ViewBottom) * ReciprocalHeight,
				fRange,
				1.0f);
			// 0,0,-fRange * NearZ,0.0f
			vValues = _mm_and_ps(vValues, g_MaskZ);
			M.r[3] = vValues;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixPerspectiveOffCenterRH
		(
			f32 ViewLeft,
			f32 ViewRight,
			f32 ViewBottom,
			f32 ViewTop,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(NearZ > 0.f && FarZ > 0.f);
			luassert(!ScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
			luassert(!ScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 TwoNearZ = NearZ + NearZ;
			f32 ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = FarZ / (NearZ - FarZ);
			const vec_float_t Zero = vdupq_n_f32(0);

			mat_float_t M;
			M.r[0] = vsetq_lane_f32(TwoNearZ * ReciprocalWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(TwoNearZ * ReciprocalHeight, Zero, 1);
			M.r[2] = vector_set((ViewLeft + ViewRight) * ReciprocalWidth,
				(ViewTop + ViewBottom) * ReciprocalHeight,
				fRange,
				-1.0f);
			M.r[3] = vsetq_lane_f32(fRange * NearZ, Zero, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 TwoNearZ = NearZ + NearZ;
			f32 ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = FarZ / (NearZ - FarZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				TwoNearZ*ReciprocalWidth,
				TwoNearZ*ReciprocalHeight,
				fRange * NearZ,
				0
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// TwoNearZ*ReciprocalWidth,0,0,0
			M.r[0] = vTemp;
			// 0,TwoNearZ*ReciprocalHeight,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// 0,0,fRange,1.0f
			M.r[2] = vector_set((ViewLeft + ViewRight) * ReciprocalWidth,
				(ViewTop + ViewBottom) * ReciprocalHeight,
				fRange,
				-1.0f);
			// 0,0,-fRange * NearZ,0.0f
			vValues = _mm_and_ps(vValues, g_MaskZ);
			M.r[3] = vValues;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixOrthographicLH
		(
			f32 ViewWidth,
			f32 ViewHeight,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(!ScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 fRange = 1.0f / (FarZ - NearZ);

			const vec_float_t Zero = vdupq_n_f32(0);
			mat_float_t M;
			M.r[0] = vsetq_lane_f32(2.0f / ViewWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(2.0f / ViewHeight, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, Zero, 2);
			M.r[3] = vsetq_lane_f32(-fRange * NearZ, g_IdentityR3.v, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 fRange = 1.0f / (FarZ - NearZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				2.0f / ViewWidth,
				2.0f / ViewHeight,
				fRange,
				-fRange * NearZ
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// 2.0f / ViewWidth,0,0,0
			M.r[0] = vTemp;
			// 0,2.0f / ViewHeight,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// x=fRange,y=-fRange * NearZ,0,1.0f
			vTemp = _mm_setzero_ps();
			vValues = _mm_shuffle_ps(vValues, g_IdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
			// 0,0,fRange,0.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 0, 0, 0));
			M.r[2] = vTemp;
			// 0,0,-fRange * NearZ,1.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 1, 0, 0));
			M.r[3] = vTemp;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixOrthographicRH
		(
			f32 ViewWidth,
			f32 ViewHeight,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(!ScalarNearEqual(ViewWidth, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(ViewHeight, 0.0f, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 fRange = 1.0f / (NearZ - FarZ);

			const vec_float_t Zero = vdupq_n_f32(0);
			mat_float_t M;
			M.r[0] = vsetq_lane_f32(2.0f / ViewWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(2.0f / ViewHeight, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, Zero, 2);
			M.r[3] = vsetq_lane_f32(fRange * NearZ, g_IdentityR3.v, 2);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 fRange = 1.0f / (NearZ - FarZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				2.0f / ViewWidth,
				2.0f / ViewHeight,
				fRange,
				fRange * NearZ
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// 2.0f / ViewWidth,0,0,0
			M.r[0] = vTemp;
			// 0,2.0f / ViewHeight,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			M.r[1] = vTemp;
			// x=fRange,y=fRange * NearZ,0,1.0f
			vTemp = _mm_setzero_ps();
			vValues = _mm_shuffle_ps(vValues, g_IdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
			// 0,0,fRange,0.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 0, 0, 0));
			M.r[2] = vTemp;
			// 0,0,fRange * NearZ,1.0f
			vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 1, 0, 0));
			M.r[3] = vTemp;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixOrthographicOffCenterLH
		(
			f32 ViewLeft,
			f32 ViewRight,
			f32 ViewBottom,
			f32 ViewTop,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(!ScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
			luassert(!ScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = 1.0f / (FarZ - NearZ);
			const vec_float_t Zero = vdupq_n_f32(0);
			mat_float_t M;
			M.r[0] = vsetq_lane_f32(ReciprocalWidth + ReciprocalWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(ReciprocalHeight + ReciprocalHeight, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, Zero, 2);
			M.r[3] = vector_set(-(ViewLeft + ViewRight) * ReciprocalWidth,
				-(ViewTop + ViewBottom) * ReciprocalHeight,
				-fRange * NearZ,
				1.0f);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 fReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 fReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = 1.0f / (FarZ - NearZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				fReciprocalWidth,
				fReciprocalHeight,
				fRange,
				1.0f
			};
			vec_float_t rMem2 = {
				-(ViewLeft + ViewRight),
				-(ViewTop + ViewBottom),
				-NearZ,
				1.0f
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// fReciprocalWidth*2,0,0,0
			vTemp = _mm_add_ss(vTemp, vTemp);
			M.r[0] = vTemp;
			// 0,fReciprocalHeight*2,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			vTemp = _mm_add_ps(vTemp, vTemp);
			M.r[1] = vTemp;
			// 0,0,fRange,0.0f
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskZ);
			M.r[2] = vTemp;
			// -(ViewLeft + ViewRight)*fReciprocalWidth,-(ViewTop + ViewBottom)*fReciprocalHeight,fRange*-NearZ,1.0f
			vValues = _mm_mul_ps(vValues, rMem2);
			M.r[3] = vValues;
			return M;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV MatrixOrthographicOffCenterRH
		(
			f32 ViewLeft,
			f32 ViewRight,
			f32 ViewBottom,
			f32 ViewTop,
			f32 NearZ,
			f32 FarZ
		)
		{
			luassert(!ScalarNearEqual(ViewRight, ViewLeft, 0.00001f));
			luassert(!ScalarNearEqual(ViewTop, ViewBottom, 0.00001f));
			luassert(!ScalarNearEqual(FarZ, NearZ, 0.00001f));

#if defined(LUNA_PLATFORM_ARM_NEON)
			f32 ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = 1.0f / (NearZ - FarZ);
			const vec_float_t Zero = vdupq_n_f32(0);
			mat_float_t M;
			M.r[0] = vsetq_lane_f32(ReciprocalWidth + ReciprocalWidth, Zero, 0);
			M.r[1] = vsetq_lane_f32(ReciprocalHeight + ReciprocalHeight, Zero, 1);
			M.r[2] = vsetq_lane_f32(fRange, Zero, 2);
			M.r[3] = vector_set(-(ViewLeft + ViewRight) * ReciprocalWidth,
				-(ViewTop + ViewBottom) * ReciprocalHeight,
				fRange * NearZ,
				1.0f);
			return M;
#elif defined(LUNA_PLATFORM_SSE2)
			mat_float_t M;
			f32 fReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
			f32 fReciprocalHeight = 1.0f / (ViewTop - ViewBottom);
			f32 fRange = 1.0f / (NearZ - FarZ);
			// Note: This is recorded on the stack
			vec_float_t rMem = {
				fReciprocalWidth,
				fReciprocalHeight,
				fRange,
				1.0f
			};
			vec_float_t rMem2 = {
				-(ViewLeft + ViewRight),
				-(ViewTop + ViewBottom),
				NearZ,
				1.0f
			};
			// Copy from memory to SSE register
			vec_float_t vValues = rMem;
			vec_float_t vTemp = _mm_setzero_ps();
			// Copy x only
			vTemp = _mm_move_ss(vTemp, vValues);
			// fReciprocalWidth*2,0,0,0
			vTemp = _mm_add_ss(vTemp, vTemp);
			M.r[0] = vTemp;
			// 0,fReciprocalHeight*2,0,0
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskY);
			vTemp = _mm_add_ps(vTemp, vTemp);
			M.r[1] = vTemp;
			// 0,0,fRange,0.0f
			vTemp = vValues;
			vTemp = _mm_and_ps(vTemp, g_MaskZ);
			M.r[2] = vTemp;
			// -(ViewLeft + ViewRight)*fReciprocalWidth,-(ViewTop + ViewBottom)*fReciprocalHeight,fRange*-NearZ,1.0f
			vValues = _mm_mul_ps(vValues, rMem2);
			M.r[3] = vValues;
			return M;
#endif
		}

#ifdef _PREFAST_
#pragma prefast(pop)
#endif

		/****************************************************************************
		 *
		 * mat_float_t operators and methods
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------

		inline mat_float_t::mat_float_t
		(
			f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33
		)
		{
			r[0] = vector_set(m00, m01, m02, m03);
			r[1] = vector_set(m10, m11, m12, m13);
			r[2] = vector_set(m20, m21, m22, m23);
			r[3] = vector_set(m30, m31, m32, m33);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t::mat_float_t
		(
			const f32* pArray
		)
		{
			luassert(pArray != nullptr);
			r[0] = load_float4(reinterpret_cast<const f32*>(pArray));
			r[1] = load_float4(reinterpret_cast<const f32*>(pArray + 4));
			r[2] = load_float4(reinterpret_cast<const f32*>(pArray + 8));
			r[3] = load_float4(reinterpret_cast<const f32*>(pArray + 12));
		}

		//------------------------------------------------------------------------------

		inline mat_float_t mat_float_t::operator- () const
		{
			mat_float_t R;
			R.r[0] = VectorNegate(r[0]);
			R.r[1] = VectorNegate(r[1]);
			R.r[2] = VectorNegate(r[2]);
			R.r[3] = VectorNegate(r[3]);
			return R;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t& LUNA_SIMD_CALLCONV mat_float_t::operator+= (mat_float_p1_t M)
		{
			r[0] = VectorAdd(r[0], M.r[0]);
			r[1] = VectorAdd(r[1], M.r[1]);
			r[2] = VectorAdd(r[2], M.r[2]);
			r[3] = VectorAdd(r[3], M.r[3]);
			return *this;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t& LUNA_SIMD_CALLCONV mat_float_t::operator-= (mat_float_p1_t M)
		{
			r[0] = VectorSubtract(r[0], M.r[0]);
			r[1] = VectorSubtract(r[1], M.r[1]);
			r[2] = VectorSubtract(r[2], M.r[2]);
			r[3] = VectorSubtract(r[3], M.r[3]);
			return *this;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t& LUNA_SIMD_CALLCONV mat_float_t::operator*=(mat_float_p1_t M)
		{
			*this = MatrixMultiply(*this, M);
			return *this;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t& mat_float_t::operator*= (f32 S)
		{
			r[0] = VectorScale(r[0], S);
			r[1] = VectorScale(r[1], S);
			r[2] = VectorScale(r[2], S);
			r[3] = VectorScale(r[3], S);
			return *this;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t& mat_float_t::operator/= (f32 S)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
			float32x4_t vS = vdupq_n_f32(S);
			r[0] = vdivq_f32(r[0], vS);
			r[1] = vdivq_f32(r[1], vS);
			r[2] = vdivq_f32(r[2], vS);
			r[3] = vdivq_f32(r[3], vS);
#else
			// 2 iterations of Newton-Raphson refinement of reciprocal
			float32x2_t vS = vdup_n_f32(S);
			float32x2_t R0 = vrecpe_f32(vS);
			float32x2_t S0 = vrecps_f32(R0, vS);
			R0 = vmul_f32(S0, R0);
			S0 = vrecps_f32(R0, vS);
			R0 = vmul_f32(S0, R0);
			float32x4_t Reciprocal = vcombine_u32(R0, R0);
			r[0] = vmulq_f32(r[0], Reciprocal);
			r[1] = vmulq_f32(r[1], Reciprocal);
			r[2] = vmulq_f32(r[2], Reciprocal);
			r[3] = vmulq_f32(r[3], Reciprocal);
#endif
			return *this;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 vS = _mm_set_ps1(S);
			r[0] = _mm_div_ps(r[0], vS);
			r[1] = _mm_div_ps(r[1], vS);
			r[2] = _mm_div_ps(r[2], vS);
			r[3] = _mm_div_ps(r[3], vS);
			return *this;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV mat_float_t::operator+ (mat_float_p1_t M) const
		{
			mat_float_t R;
			R.r[0] = VectorAdd(r[0], M.r[0]);
			R.r[1] = VectorAdd(r[1], M.r[1]);
			R.r[2] = VectorAdd(r[2], M.r[2]);
			R.r[3] = VectorAdd(r[3], M.r[3]);
			return R;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV mat_float_t::operator- (mat_float_p1_t M) const
		{
			mat_float_t R;
			R.r[0] = VectorSubtract(r[0], M.r[0]);
			R.r[1] = VectorSubtract(r[1], M.r[1]);
			R.r[2] = VectorSubtract(r[2], M.r[2]);
			R.r[3] = VectorSubtract(r[3], M.r[3]);
			return R;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV mat_float_t::operator*(mat_float_p1_t M) const
		{
			return MatrixMultiply(*this, M);
		}

		//------------------------------------------------------------------------------

		inline mat_float_t mat_float_t::operator* (f32 S) const
		{
			mat_float_t R;
			R.r[0] = VectorScale(r[0], S);
			R.r[1] = VectorScale(r[1], S);
			R.r[2] = VectorScale(r[2], S);
			R.r[3] = VectorScale(r[3], S);
			return R;
		}

		//------------------------------------------------------------------------------

		inline mat_float_t mat_float_t::operator/ (f32 S) const
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
			float32x4_t vS = vdupq_n_f32(S);
			mat_float_t R;
			R.r[0] = vdivq_f32(r[0], vS);
			R.r[1] = vdivq_f32(r[1], vS);
			R.r[2] = vdivq_f32(r[2], vS);
			R.r[3] = vdivq_f32(r[3], vS);
#else
			// 2 iterations of Newton-Raphson refinement of reciprocal
			float32x2_t vS = vdup_n_f32(S);
			float32x2_t R0 = vrecpe_f32(vS);
			float32x2_t S0 = vrecps_f32(R0, vS);
			R0 = vmul_f32(S0, R0);
			S0 = vrecps_f32(R0, vS);
			R0 = vmul_f32(S0, R0);
			float32x4_t Reciprocal = vcombine_u32(R0, R0);
			mat_float_t R;
			R.r[0] = vmulq_f32(r[0], Reciprocal);
			R.r[1] = vmulq_f32(r[1], Reciprocal);
			R.r[2] = vmulq_f32(r[2], Reciprocal);
			R.r[3] = vmulq_f32(r[3], Reciprocal);
#endif
			return R;
#elif defined(LUNA_PLATFORM_SSE2)
			__m128 vS = _mm_set_ps1(S);
			mat_float_t R;
			R.r[0] = _mm_div_ps(r[0], vS);
			R.r[1] = _mm_div_ps(r[1], vS);
			R.r[2] = _mm_div_ps(r[2], vS);
			R.r[3] = _mm_div_ps(r[3], vS);
			return R;
#endif
		}

		//------------------------------------------------------------------------------

		inline mat_float_t LUNA_SIMD_CALLCONV operator*
			(
				f32 S,
				mat_float_p1_t M
				)
		{
			mat_float_t R;
			R.r[0] = VectorScale(M.r[0], S);
			R.r[1] = VectorScale(M.r[1], S);
			R.r[2] = VectorScale(M.r[2], S);
			R.r[3] = VectorScale(M.r[3], S);
			return R;
		}
	}
}

#endif