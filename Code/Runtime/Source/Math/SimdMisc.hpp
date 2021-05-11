// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file SimdConvert.inl
* @author JxMaster
* @date 2019/1/4
* @brief Multi-platform SIMD Library modified from Microsoft's DirectX Math.
* See: https://github.com/Microsoft/DirectXMath
 */
#pragma once

#include "SimdMatrix.hpp"

#if defined(LUNA_SIMD_ENABLED)

namespace Luna
{
	namespace Simd
	{
		/****************************************************************************
		 *
		 * Quaternion
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV QuaternionEqual
		(
			vec_float_p1_t Q1,
			vec_float_p1_t Q2
		)
		{
			return Vector4Equal(Q1, Q2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV QuaternionNotEqual
		(
			vec_float_p1_t Q1,
			vec_float_p1_t Q2
		)
		{
			return Vector4NotEqual(Q1, Q2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV QuaternionIsNaN
		(
			vec_float_p1_t Q
		)
		{
			return Vector4IsNaN(Q);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV QuaternionIsInfinite
		(
			vec_float_p1_t Q
		)
		{
			return Vector4IsInfinite(Q);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV QuaternionIsIdentity
		(
			vec_float_p1_t Q
		)
		{
			return Vector4Equal(Q, g_IdentityR3.v);
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionDot
		(
			vec_float_p1_t Q1,
			vec_float_p1_t Q2
		)
		{
			return Vector4Dot(Q1, Q2);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionMultiply
		(
			vec_float_p1_t Q1,
			vec_float_p1_t Q2
		)
		{
			// Returns the product Q2*Q1 (which is the concatenation of a rotation Q1 followed by the rotation Q2)

			// [ (Q2.w * Q1.x) + (Q2.x * Q1.w) + (Q2.y * Q1.z) - (Q2.z * Q1.y),
			//   (Q2.w * Q1.y) - (Q2.x * Q1.z) + (Q2.y * Q1.w) + (Q2.z * Q1.x),
			//   (Q2.w * Q1.z) + (Q2.x * Q1.y) - (Q2.y * Q1.x) + (Q2.z * Q1.w),
			//   (Q2.w * Q1.w) - (Q2.x * Q1.x) - (Q2.y * Q1.y) - (Q2.z * Q1.z) ]

#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t ControlWZYX = { { { 1.0f, -1.0f, 1.0f, -1.0f } } };
			static const vec_f32_t ControlZWXY = { { { 1.0f, 1.0f, -1.0f, -1.0f } } };
			static const vec_f32_t ControlYXWZ = { { { -1.0f, 1.0f, 1.0f, -1.0f } } };

			float32x2_t Q2L = vget_low_f32(Q2);
			float32x2_t Q2H = vget_high_f32(Q2);

			float32x4_t Q2X = vdupq_lane_f32(Q2L, 0);
			float32x4_t Q2Y = vdupq_lane_f32(Q2L, 1);
			float32x4_t Q2Z = vdupq_lane_f32(Q2H, 0);
			vec_float vResult = vmulq_lane_f32(Q1, Q2H, 1);

			// Mul by Q1WZYX
			float32x4_t vTemp = vrev64q_f32(Q1);
			vTemp = vcombine_f32(vget_high_f32(vTemp), vget_low_f32(vTemp));
			Q2X = vmulq_f32(Q2X, vTemp);
			vResult = vmlaq_f32(vResult, Q2X, ControlWZYX);

			// Mul by Q1ZWXY
			vTemp = vrev64q_u32(vTemp);
			Q2Y = vmulq_f32(Q2Y, vTemp);
			vResult = vmlaq_f32(vResult, Q2Y, ControlZWXY);

			// Mul by Q1YXWZ
			vTemp = vrev64q_u32(vTemp);
			vTemp = vcombine_f32(vget_high_f32(vTemp), vget_low_f32(vTemp));
			Q2Z = vmulq_f32(Q2Z, vTemp);
			vResult = vmlaq_f32(vResult, Q2Z, ControlYXWZ);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t ControlWZYX = { { { 1.0f, -1.0f, 1.0f, -1.0f } } };
			static const vec_f32_t ControlZWXY = { { { 1.0f, 1.0f, -1.0f, -1.0f } } };
			static const vec_f32_t ControlYXWZ = { { { -1.0f, 1.0f, 1.0f, -1.0f } } };
			// Copy to SSE registers and use as few as possible for x86
			vec_float_t Q2X = Q2;
			vec_float_t Q2Y = Q2;
			vec_float_t Q2Z = Q2;
			vec_float_t vResult = Q2;
			// Splat with one instruction
			vResult = LUNA_SIMD_PERMUTE_PS(vResult, _MM_SHUFFLE(3, 3, 3, 3));
			Q2X = LUNA_SIMD_PERMUTE_PS(Q2X, _MM_SHUFFLE(0, 0, 0, 0));
			Q2Y = LUNA_SIMD_PERMUTE_PS(Q2Y, _MM_SHUFFLE(1, 1, 1, 1));
			Q2Z = LUNA_SIMD_PERMUTE_PS(Q2Z, _MM_SHUFFLE(2, 2, 2, 2));
			// Retire Q1 and perform Q1*Q2W
			vResult = _mm_mul_ps(vResult, Q1);
			vec_float_t Q1Shuffle = Q1;
			// Shuffle the copies of Q1
			Q1Shuffle = LUNA_SIMD_PERMUTE_PS(Q1Shuffle, _MM_SHUFFLE(0, 1, 2, 3));
			// Mul by Q1WZYX
			Q2X = _mm_mul_ps(Q2X, Q1Shuffle);
			Q1Shuffle = LUNA_SIMD_PERMUTE_PS(Q1Shuffle, _MM_SHUFFLE(2, 3, 0, 1));
			// Flip the signs on y and z
			Q2X = _mm_mul_ps(Q2X, ControlWZYX);
			// Mul by Q1ZWXY
			Q2Y = _mm_mul_ps(Q2Y, Q1Shuffle);
			Q1Shuffle = LUNA_SIMD_PERMUTE_PS(Q1Shuffle, _MM_SHUFFLE(0, 1, 2, 3));
			// Flip the signs on z and w
			Q2Y = _mm_mul_ps(Q2Y, ControlZWXY);
			// Mul by Q1YXWZ
			Q2Z = _mm_mul_ps(Q2Z, Q1Shuffle);
			vResult = _mm_add_ps(vResult, Q2X);
			// Flip the signs on x and w
			Q2Z = _mm_mul_ps(Q2Z, ControlYXWZ);
			Q2Y = _mm_add_ps(Q2Y, Q2Z);
			vResult = _mm_add_ps(vResult, Q2Y);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionLengthSq
		(
			vec_float_p1_t Q
		)
		{
			return Vector4LengthSq(Q);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionReciprocalLength
		(
			vec_float_p1_t Q
		)
		{
			return Vector4ReciprocalLength(Q);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionLength
		(
			vec_float_p1_t Q
		)
		{
			return Vector4Length(Q);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionNormalizeEst
		(
			vec_float_p1_t Q
		)
		{
			return Vector4NormalizeEst(Q);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionNormalize
		(
			vec_float_p1_t Q
		)
		{
			return Vector4Normalize(Q);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionConjugate
		(
			vec_float_p1_t Q
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t NegativeOne3 = { { { -1.0f, -1.0f, -1.0f, 1.0f } } };
			return vmulq_f32(Q, NegativeOne3.v);
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t NegativeOne3 = { { { -1.0f, -1.0f, -1.0f, 1.0f } } };
			return _mm_mul_ps(Q, NegativeOne3);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionInverse
		(
			vec_float_p1_t Q
		)
		{
			const vec_float_t  Zero = vector_zero();

			vec_float_t L = Vector4LengthSq(Q);
			vec_float_t Conjugate = QuaternionConjugate(Q);

			vec_float_t Control = VectorLessOrEqual(L, g_Epsilon.v);

			vec_float_t Result = VectorDivide(Conjugate, L);

			Result = VectorSelect(Result, Zero, Control);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionLn
		(
			vec_float_p1_t Q
		)
		{
			static const vec_f32_t OneMinusEpsilon = { { { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f } } };

			vec_float_t QW = vector_splat_w(Q);
			vec_float_t Q0 = VectorSelect(g_Select1110.v, Q, g_Select1110.v);

			vec_float_t ControlW = VectorInBounds(QW, OneMinusEpsilon.v);

			vec_float_t Theta = VectorACos(QW);
			vec_float_t SinTheta = VectorSin(Theta);

			vec_float_t S = VectorDivide(Theta, SinTheta);

			vec_float_t Result = VectorMultiply(Q0, S);
			Result = VectorSelect(Q0, Result, ControlW);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionExp
		(
			vec_float_p1_t Q
		)
		{
			vec_float_t Theta = Vector3Length(Q);

			vec_float_t SinTheta, CosTheta;
			VectorSinCos(&SinTheta, &CosTheta, Theta);

			vec_float_t S = VectorDivide(SinTheta, Theta);

			vec_float_t Result = VectorMultiply(Q, S);

			const vec_float_t Zero = vector_zero();
			vec_float_t Control = VectorNearEqual(Theta, Zero, g_Epsilon.v);
			Result = VectorSelect(Result, Q, Control);

			Result = VectorSelect(CosTheta, Result, g_Select1110.v);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionSlerp
		(
			vec_float_p1_t Q0,
			vec_float_p1_t Q1,
			f32    t
		)
		{
			vec_float_t T = vector_replicate(t);
			return QuaternionSlerpV(Q0, Q1, T);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionSlerpV
		(
			vec_float_p1_t Q0,
			vec_float_p1_t Q1,
			vec_float_p1_t T
		)
		{
			luassert((VectorGetY(T) == VectorGetX(T)) && (VectorGetZ(T) == VectorGetX(T)) && (VectorGetW(T) == VectorGetX(T)));

			// Result = Q0 * sin((1.0 - t) * Omega) / sin(Omega) + Q1 * sin(t * Omega) / sin(Omega)

#if defined(LUNA_PLATFORM_ARM_NEON)

			const vec_f32_t OneMinusEpsilon = { { { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f } } };

			vec_float CosOmega = QuaternionDot(Q0, Q1);

			const vec_float Zero = vector_zero();
			vec_float Control = VectorLess(CosOmega, Zero);
			vec_float Sign = VectorSelect(g_One.v, g_NegativeOne.v, Control);

			CosOmega = VectorMultiply(CosOmega, Sign);

			Control = VectorLess(CosOmega, OneMinusEpsilon);

			vec_float SinOmega = VectorNegativeMultiplySubtract(CosOmega, CosOmega, g_One.v);
			SinOmega = VectorSqrt(SinOmega);

			vec_float Omega = VectorATan2(SinOmega, CosOmega);

			vec_float SignMask = vector_splat_sign_mask();
			vec_float V01 = VectorShiftLeft(T, Zero, 2);
			SignMask = VectorShiftLeft(SignMask, Zero, 3);
			V01 = VectorXorInt(V01, SignMask);
			V01 = VectorAdd(g_IdentityR0.v, V01);

			vec_float InvSinOmega = VectorReciprocal(SinOmega);

			vec_float S0 = VectorMultiply(V01, Omega);
			S0 = VectorSin(S0);
			S0 = VectorMultiply(S0, InvSinOmega);

			S0 = VectorSelect(V01, S0, Control);

			vec_float S1 = vector_splat_y(S0);
			S0 = vector_splat_x(S0);

			S1 = VectorMultiply(S1, Sign);

			vec_float Result = VectorMultiply(Q0, S0);
			Result = VectorMultiplyAdd(Q1, S1, Result);

			return Result;

#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t OneMinusEpsilon = { { { 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f, 1.0f - 0.00001f } } };
			static const vec_u32_t SignMask2 = { { { 0x80000000, 0x00000000, 0x00000000, 0x00000000 } } };

			vec_float_t CosOmega = QuaternionDot(Q0, Q1);

			const vec_float_t Zero = vector_zero();
			vec_float_t Control = VectorLess(CosOmega, Zero);
			vec_float_t Sign = VectorSelect(g_One, g_NegativeOne, Control);

			CosOmega = _mm_mul_ps(CosOmega, Sign);

			Control = VectorLess(CosOmega, OneMinusEpsilon);

			vec_float_t SinOmega = _mm_mul_ps(CosOmega, CosOmega);
			SinOmega = _mm_sub_ps(g_One, SinOmega);
			SinOmega = _mm_sqrt_ps(SinOmega);

			vec_float_t Omega = VectorATan2(SinOmega, CosOmega);

			vec_float_t V01 = LUNA_SIMD_PERMUTE_PS(T, _MM_SHUFFLE(2, 3, 0, 1));
			V01 = _mm_and_ps(V01, g_MaskXY);
			V01 = _mm_xor_ps(V01, SignMask2);
			V01 = _mm_add_ps(g_IdentityR0, V01);

			vec_float_t S0 = _mm_mul_ps(V01, Omega);
			S0 = VectorSin(S0);
			S0 = _mm_div_ps(S0, SinOmega);

			S0 = VectorSelect(V01, S0, Control);

			vec_float_t S1 = vector_splat_y(S0);
			S0 = vector_splat_x(S0);

			S1 = _mm_mul_ps(S1, Sign);
			vec_float_t Result = _mm_mul_ps(Q0, S0);
			S1 = _mm_mul_ps(S1, Q1);
			Result = _mm_add_ps(Result, S1);
			return Result;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionSquad
		(
			vec_float_p1_t Q0,
			vec_float_p1_t Q1,
			vec_float_p1_t Q2,
			vec_float_p2_t Q3,
			f32    t
		)
		{
			vec_float_t T = vector_replicate(t);
			return QuaternionSquadV(Q0, Q1, Q2, Q3, T);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionSquadV
		(
			vec_float_p1_t Q0,
			vec_float_p1_t Q1,
			vec_float_p1_t Q2,
			vec_float_p2_t Q3,
			vec_float_p3_t T
		)
		{
			luassert((VectorGetY(T) == VectorGetX(T)) && (VectorGetZ(T) == VectorGetX(T)) && (VectorGetW(T) == VectorGetX(T)));

			vec_float_t TP = T;
			const vec_float_t Two = vector_splat_constant(2, 0);

			vec_float_t Q03 = QuaternionSlerpV(Q0, Q3, T);
			vec_float_t Q12 = QuaternionSlerpV(Q1, Q2, T);

			TP = VectorNegativeMultiplySubtract(TP, TP, TP);
			TP = VectorMultiply(TP, Two);

			vec_float_t Result = QuaternionSlerpV(Q03, Q12, TP);

			return Result;
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV QuaternionSquadSetup
		(
			vec_float_t* pA,
			vec_float_t* pB,
			vec_float_t* pC,
			vec_float_p1_t  Q0,
			vec_float_p1_t  Q1,
			vec_float_p1_t  Q2,
			vec_float_p2_t  Q3
		)
		{
			luassert(pA);
			luassert(pB);
			luassert(pC);

			vec_float_t LS12 = QuaternionLengthSq(VectorAdd(Q1, Q2));
			vec_float_t LD12 = QuaternionLengthSq(VectorSubtract(Q1, Q2));
			vec_float_t SQ2 = VectorNegate(Q2);

			vec_float_t Control1 = VectorLess(LS12, LD12);
			SQ2 = VectorSelect(Q2, SQ2, Control1);

			vec_float_t LS01 = QuaternionLengthSq(VectorAdd(Q0, Q1));
			vec_float_t LD01 = QuaternionLengthSq(VectorSubtract(Q0, Q1));
			vec_float_t SQ0 = VectorNegate(Q0);

			vec_float_t LS23 = QuaternionLengthSq(VectorAdd(SQ2, Q3));
			vec_float_t LD23 = QuaternionLengthSq(VectorSubtract(SQ2, Q3));
			vec_float_t SQ3 = VectorNegate(Q3);

			vec_float_t Control0 = VectorLess(LS01, LD01);
			vec_float_t Control2 = VectorLess(LS23, LD23);

			SQ0 = VectorSelect(Q0, SQ0, Control0);
			SQ3 = VectorSelect(Q3, SQ3, Control2);

			vec_float_t InvQ1 = QuaternionInverse(Q1);
			vec_float_t InvQ2 = QuaternionInverse(SQ2);

			vec_float_t LnQ0 = QuaternionLn(QuaternionMultiply(InvQ1, SQ0));
			vec_float_t LnQ2 = QuaternionLn(QuaternionMultiply(InvQ1, SQ2));
			vec_float_t LnQ1 = QuaternionLn(QuaternionMultiply(InvQ2, Q1));
			vec_float_t LnQ3 = QuaternionLn(QuaternionMultiply(InvQ2, SQ3));

			const vec_float_t NegativeOneQuarter = vector_splat_constant(-1, 2);

			vec_float_t ExpQ02 = VectorMultiply(VectorAdd(LnQ0, LnQ2), NegativeOneQuarter);
			vec_float_t ExpQ13 = VectorMultiply(VectorAdd(LnQ1, LnQ3), NegativeOneQuarter);
			ExpQ02 = QuaternionExp(ExpQ02);
			ExpQ13 = QuaternionExp(ExpQ13);

			*pA = QuaternionMultiply(Q1, ExpQ02);
			*pB = QuaternionMultiply(SQ2, ExpQ13);
			*pC = SQ2;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionBaryCentric
		(
			vec_float_p1_t Q0,
			vec_float_p1_t Q1,
			vec_float_p1_t Q2,
			f32    f,
			f32    g
		)
		{
			f32 s = f + g;

			vec_float_t Result;
			if ((s < 0.00001f) && (s > -0.00001f))
			{
				Result = Q0;
			}
			else
			{
				vec_float_t Q01 = QuaternionSlerp(Q0, Q1, s);
				vec_float_t Q02 = QuaternionSlerp(Q0, Q2, s);

				Result = QuaternionSlerp(Q01, Q02, g / s);
			}

			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionBaryCentricV
		(
			vec_float_p1_t Q0,
			vec_float_p1_t Q1,
			vec_float_p1_t Q2,
			vec_float_p2_t F,
			vec_float_p3_t G
		)
		{
			luassert((VectorGetY(F) == VectorGetX(F)) && (VectorGetZ(F) == VectorGetX(F)) && (VectorGetW(F) == VectorGetX(F)));
			luassert((VectorGetY(G) == VectorGetX(G)) && (VectorGetZ(G) == VectorGetX(G)) && (VectorGetW(G) == VectorGetX(G)));

			const vec_float_t Epsilon = vector_splat_constant(1, 16);

			vec_float_t S = VectorAdd(F, G);

			vec_float_t Result;
			if (Vector4InBounds(S, Epsilon))
			{
				Result = Q0;
			}
			else
			{
				vec_float_t Q01 = QuaternionSlerpV(Q0, Q1, S);
				vec_float_t Q02 = QuaternionSlerpV(Q0, Q2, S);
				vec_float_t GS = VectorReciprocal(S);
				GS = VectorMultiply(G, GS);

				Result = QuaternionSlerpV(Q01, Q02, GS);
			}

			return Result;
		}

		//------------------------------------------------------------------------------
		// Transformation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionIdentity()
		{
			return g_IdentityR3.v;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionRotationRollPitchYaw
		(
			f32 Pitch,
			f32 Yaw,
			f32 Roll
		)
		{
			vec_float_t Angles = vector_set(Pitch, Yaw, Roll, 0.0f);
			vec_float_t Q = QuaternionRotationRollPitchYawFromVector(Angles);
			return Q;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionRotationRollPitchYawFromVector
		(
			vec_float_p1_t Angles // <Pitch, Yaw, Roll, 0>
		)
		{
			static const vec_f32_t  Sign = { { { 1.0f, -1.0f, -1.0f, 1.0f } } };

			vec_float_t HalfAngles = VectorMultiply(Angles, g_OneHalf.v);

			vec_float_t SinAngles, CosAngles;
			VectorSinCos(&SinAngles, &CosAngles, HalfAngles);

			vec_float_t P0 = VectorPermute<g_permute_0x, g_permute_1x, g_permute_1x, g_permute_1x>(SinAngles, CosAngles);
			vec_float_t Y0 = VectorPermute<g_permute_1y, g_permute_0y, g_permute_1y, g_permute_1y>(SinAngles, CosAngles);
			vec_float_t R0 = VectorPermute<g_permute_1z, g_permute_1z, g_permute_0z, g_permute_1z>(SinAngles, CosAngles);
			vec_float_t P1 = VectorPermute<g_permute_0x, g_permute_1x, g_permute_1x, g_permute_1x>(CosAngles, SinAngles);
			vec_float_t Y1 = VectorPermute<g_permute_1y, g_permute_0y, g_permute_1y, g_permute_1y>(CosAngles, SinAngles);
			vec_float_t R1 = VectorPermute<g_permute_1z, g_permute_1z, g_permute_0z, g_permute_1z>(CosAngles, SinAngles);

			vec_float_t Q1 = VectorMultiply(P1, Sign.v);
			vec_float_t Q0 = VectorMultiply(P0, Y0);
			Q1 = VectorMultiply(Q1, Y1);
			Q0 = VectorMultiply(Q0, R0);
			vec_float_t Q = VectorMultiplyAdd(Q1, R1, Q0);

			return Q;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionRotationNormal
		(
			vec_float_p1_t NormalAxis,
			f32    Angle
		)
		{
#if defined(LUNA_PLATFORM_SSE2)
			vec_float_t N = _mm_and_ps(NormalAxis, g_Mask3);
			N = _mm_or_ps(N, g_IdentityR3);
			vec_float_t Scale = _mm_set_ps1(0.5f * Angle);
			vec_float_t vSine;
			vec_float_t vCosine;
			VectorSinCos(&vSine, &vCosine, Scale);
			Scale = _mm_and_ps(vSine, g_Mask3);
			vCosine = _mm_and_ps(vCosine, g_MaskW);
			Scale = _mm_or_ps(Scale, vCosine);
			N = _mm_mul_ps(N, Scale);
			return N;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionRotationAxis
		(
			vec_float_p1_t Axis,
			f32    Angle
		)
		{
			luassert(!Vector3Equal(Axis, vector_zero()));
			luassert(!Vector3IsInfinite(Axis));

			vec_float_t Normal = Vector3Normalize(Axis);
			vec_float_t Q = QuaternionRotationNormal(Normal, Angle);
			return Q;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV QuaternionRotationMatrix
		(
			mat_float_p1_t M
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			static const vec_f32_t XMPMMP = { { { +1.0f, -1.0f, -1.0f, +1.0f } } };
			static const vec_f32_t XMMPMP = { { { -1.0f, +1.0f, -1.0f, +1.0f } } };
			static const vec_f32_t XMMMPP = { { { -1.0f, -1.0f, +1.0f, +1.0f } } };
			static const vec_u32_t Select0110 = { { { g_select_0, g_select_1, g_select_1, g_select_0 } } };
			static const vec_u32_t Select0010 = { { { g_select_0, g_select_0, g_select_1, g_select_0 } } };

			vec_float r0 = M.r[0];
			vec_float r1 = M.r[1];
			vec_float r2 = M.r[2];

			vec_float r00 = vdupq_lane_f32(vget_low_f32(r0), 0);
			vec_float r11 = vdupq_lane_f32(vget_low_f32(r1), 1);
			vec_float r22 = vdupq_lane_f32(vget_high_f32(r2), 0);

			// x^2 >= y^2 equivalent to r11 - r00 <= 0
			vec_float r11mr00 = vsubq_f32(r11, r00);
			vec_float x2gey2 = vcleq_f32(r11mr00, g_Zero);

			// z^2 >= w^2 equivalent to r11 + r00 <= 0
			vec_float r11pr00 = vaddq_f32(r11, r00);
			vec_float z2gew2 = vcleq_f32(r11pr00, g_Zero);

			// x^2 + y^2 >= z^2 + w^2 equivalent to r22 <= 0
			vec_float x2py2gez2pw2 = vcleq_f32(r22, g_Zero);

			// (4*x^2, 4*y^2, 4*z^2, 4*w^2)
			vec_float t0 = vmulq_f32(XMPMMP, r00);
			vec_float x2y2z2w2 = vmlaq_f32(t0, XMMPMP, r11);
			x2y2z2w2 = vmlaq_f32(x2y2z2w2, XMMMPP, r22);
			x2y2z2w2 = vaddq_f32(x2y2z2w2, g_One);

			// (r01, r02, r12, r11)
			t0 = vextq_f32(r0, r0, 1);
			vec_float t1 = vextq_f32(r1, r1, 1);
			t0 = vcombine_f32(vget_low_f32(t0), vrev64_f32(vget_low_f32(t1)));

			// (r10, r20, r21, r10)
			t1 = vextq_f32(r2, r2, 3);
			vec_float r10 = vdupq_lane_f32(vget_low_f32(r1), 0);
			t1 = vbslq_f32(Select0110, t1, r10);

			// (4*x*y, 4*x*z, 4*y*z, unused)
			vec_float xyxzyz = vaddq_f32(t0, t1);

			// (r21, r20, r10, r10)
			t0 = vcombine_f32(vrev64_f32(vget_low_f32(r2)), vget_low_f32(r10));

			// (r12, r02, r01, r12)
			vec_float t2 = vcombine_f32(vrev64_f32(vget_high_f32(r0)), vrev64_f32(vget_low_f32(r0)));
			vec_float t3 = vdupq_lane_f32(vget_high_f32(r1), 0);
			t1 = vbslq_f32(Select0110, t2, t3);

			// (4*x*w, 4*y*w, 4*z*w, unused)
			vec_float xwywzw = vsubq_f32(t0, t1);
			xwywzw = vmulq_f32(XMMPMP, xwywzw);

			// (4*x*x, 4*x*y, 4*x*z, 4*x*w)
			t0 = vextq_f32(xyxzyz, xyxzyz, 3);
			t1 = vbslq_f32(Select0110, t0, x2y2z2w2);
			t2 = vdupq_lane_f32(vget_low_f32(xwywzw), 0);
			vec_float tensor0 = vbslq_f32(g_Select1110, t1, t2);

			// (4*y*x, 4*y*y, 4*y*z, 4*y*w)
			t0 = vbslq_f32(g_Select1011, xyxzyz, x2y2z2w2);
			t1 = vdupq_lane_f32(vget_low_f32(xwywzw), 1);
			vec_float tensor1 = vbslq_f32(g_Select1110, t0, t1);

			// (4*z*x, 4*z*y, 4*z*z, 4*z*w)
			t0 = vextq_f32(xyxzyz, xyxzyz, 1);
			t1 = vcombine_f32(vget_low_f32(t0), vrev64_f32(vget_high_f32(xwywzw)));
			vec_float tensor2 = vbslq_f32(Select0010, x2y2z2w2, t1);

			// (4*w*x, 4*w*y, 4*w*z, 4*w*w)
			vec_float tensor3 = vbslq_f32(g_Select1110, xwywzw, x2y2z2w2);

			// Select the row of the tensor-product matrix that has the largest
			// magnitude.
			t0 = vbslq_f32(x2gey2, tensor0, tensor1);
			t1 = vbslq_f32(z2gew2, tensor2, tensor3);
			t2 = vbslq_f32(x2py2gez2pw2, t0, t1);

			// normalize the row.  No division by zero is possible because the
			// Quaternion is unit-length (and the row is a nonzero multiple of
			// the Quaternion).
			t0 = Vector4Length(t2);
			return VectorDivide(t2, t0);
#elif defined(LUNA_PLATFORM_SSE2)
			static const vec_f32_t XMPMMP = { { { +1.0f, -1.0f, -1.0f, +1.0f } } };
			static const vec_f32_t XMMPMP = { { { -1.0f, +1.0f, -1.0f, +1.0f } } };
			static const vec_f32_t XMMMPP = { { { -1.0f, -1.0f, +1.0f, +1.0f } } };

			vec_float_t r0 = M.r[0];  // (r00, r01, r02, 0)
			vec_float_t r1 = M.r[1];  // (r10, r11, r12, 0)
			vec_float_t r2 = M.r[2];  // (r20, r21, r22, 0)

			// (r00, r00, r00, r00)
			vec_float_t r00 = LUNA_SIMD_PERMUTE_PS(r0, _MM_SHUFFLE(0, 0, 0, 0));
			// (r11, r11, r11, r11)
			vec_float_t r11 = LUNA_SIMD_PERMUTE_PS(r1, _MM_SHUFFLE(1, 1, 1, 1));
			// (r22, r22, r22, r22)
			vec_float_t r22 = LUNA_SIMD_PERMUTE_PS(r2, _MM_SHUFFLE(2, 2, 2, 2));

			// x^2 >= y^2 equivalent to r11 - r00 <= 0
			// (r11 - r00, r11 - r00, r11 - r00, r11 - r00)
			vec_float_t r11mr00 = _mm_sub_ps(r11, r00);
			vec_float_t x2gey2 = _mm_cmple_ps(r11mr00, g_Zero);

			// z^2 >= w^2 equivalent to r11 + r00 <= 0
			// (r11 + r00, r11 + r00, r11 + r00, r11 + r00)
			vec_float_t r11pr00 = _mm_add_ps(r11, r00);
			vec_float_t z2gew2 = _mm_cmple_ps(r11pr00, g_Zero);

			// x^2 + y^2 >= z^2 + w^2 equivalent to r22 <= 0
			vec_float_t x2py2gez2pw2 = _mm_cmple_ps(r22, g_Zero);

			// (+r00, -r00, -r00, +r00)
			vec_float_t t0 = _mm_mul_ps(XMPMMP, r00);

			// (-r11, +r11, -r11, +r11)
			vec_float_t t1 = _mm_mul_ps(XMMPMP, r11);

			// (-r22, -r22, +r22, +r22)
			vec_float_t t2 = _mm_mul_ps(XMMMPP, r22);

			// (4*x^2, 4*y^2, 4*z^2, 4*w^2)
			vec_float_t x2y2z2w2 = _mm_add_ps(t0, t1);
			x2y2z2w2 = _mm_add_ps(t2, x2y2z2w2);
			x2y2z2w2 = _mm_add_ps(x2y2z2w2, g_One);

			// (r01, r02, r12, r11)
			t0 = _mm_shuffle_ps(r0, r1, _MM_SHUFFLE(1, 2, 2, 1));
			// (r10, r10, r20, r21)
			t1 = _mm_shuffle_ps(r1, r2, _MM_SHUFFLE(1, 0, 0, 0));
			// (r10, r20, r21, r10)
			t1 = LUNA_SIMD_PERMUTE_PS(t1, _MM_SHUFFLE(1, 3, 2, 0));
			// (4*x*y, 4*x*z, 4*y*z, unused)
			vec_float_t xyxzyz = _mm_add_ps(t0, t1);

			// (r21, r20, r10, r10)
			t0 = _mm_shuffle_ps(r2, r1, _MM_SHUFFLE(0, 0, 0, 1));
			// (r12, r12, r02, r01)
			t1 = _mm_shuffle_ps(r1, r0, _MM_SHUFFLE(1, 2, 2, 2));
			// (r12, r02, r01, r12)
			t1 = LUNA_SIMD_PERMUTE_PS(t1, _MM_SHUFFLE(1, 3, 2, 0));
			// (4*x*w, 4*y*w, 4*z*w, unused)
			vec_float_t xwywzw = _mm_sub_ps(t0, t1);
			xwywzw = _mm_mul_ps(XMMPMP, xwywzw);

			// (4*x^2, 4*y^2, 4*x*y, unused)
			t0 = _mm_shuffle_ps(x2y2z2w2, xyxzyz, _MM_SHUFFLE(0, 0, 1, 0));
			// (4*z^2, 4*w^2, 4*z*w, unused)
			t1 = _mm_shuffle_ps(x2y2z2w2, xwywzw, _MM_SHUFFLE(0, 2, 3, 2));
			// (4*x*z, 4*y*z, 4*x*w, 4*y*w)
			t2 = _mm_shuffle_ps(xyxzyz, xwywzw, _MM_SHUFFLE(1, 0, 2, 1));

			// (4*x*x, 4*x*y, 4*x*z, 4*x*w)
			vec_float_t tensor0 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(2, 0, 2, 0));
			// (4*y*x, 4*y*y, 4*y*z, 4*y*w)
			vec_float_t tensor1 = _mm_shuffle_ps(t0, t2, _MM_SHUFFLE(3, 1, 1, 2));
			// (4*z*x, 4*z*y, 4*z*z, 4*z*w)
			vec_float_t tensor2 = _mm_shuffle_ps(t2, t1, _MM_SHUFFLE(2, 0, 1, 0));
			// (4*w*x, 4*w*y, 4*w*z, 4*w*w)
			vec_float_t tensor3 = _mm_shuffle_ps(t2, t1, _MM_SHUFFLE(1, 2, 3, 2));

			// Select the row of the tensor-product matrix that has the largest
			// magnitude.
			t0 = _mm_and_ps(x2gey2, tensor0);
			t1 = _mm_andnot_ps(x2gey2, tensor1);
			t0 = _mm_or_ps(t0, t1);
			t1 = _mm_and_ps(z2gew2, tensor2);
			t2 = _mm_andnot_ps(z2gew2, tensor3);
			t1 = _mm_or_ps(t1, t2);
			t0 = _mm_and_ps(x2py2gez2pw2, t0);
			t1 = _mm_andnot_ps(x2py2gez2pw2, t1);
			t2 = _mm_or_ps(t0, t1);

			// normalize the row.  No division by zero is possible because the
			// Quaternion is unit-length (and the row is a nonzero multiple of
			// the Quaternion).
			t0 = Vector4Length(t2);
			return _mm_div_ps(t2, t0);
#endif
		}

		//------------------------------------------------------------------------------
		// Conversion operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV QuaternionToAxisAngle
		(
			vec_float_t* pAxis,
			f32*    pAngle,
			vec_float_p1_t  Q
		)
		{
			luassert(pAxis);
			luassert(pAngle);

			*pAxis = Q;

			*pAngle = 2.0f * ScalarACos(VectorGetW(Q));
		}

		/****************************************************************************
		 *
		 * Plane
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV PlaneEqual
		(
			vec_float_p1_t P1,
			vec_float_p1_t P2
		)
		{
			return Vector4Equal(P1, P2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV PlaneNearEqual
		(
			vec_float_p1_t P1,
			vec_float_p1_t P2,
			vec_float_p1_t Epsilon
		)
		{
			vec_float_t NP1 = PlaneNormalize(P1);
			vec_float_t NP2 = PlaneNormalize(P2);
			return Vector4NearEqual(NP1, NP2, Epsilon);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV PlaneNotEqual
		(
			vec_float_p1_t P1,
			vec_float_p1_t P2
		)
		{
			return Vector4NotEqual(P1, P2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV PlaneIsNaN
		(
			vec_float_p1_t P
		)
		{
			return Vector4IsNaN(P);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV PlaneIsInfinite
		(
			vec_float_p1_t P
		)
		{
			return Vector4IsInfinite(P);
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneDot
		(
			vec_float_p1_t P,
			vec_float_p1_t V
		)
		{
			return Vector4Dot(P, V);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneDotCoord
		(
			vec_float_p1_t P,
			vec_float_p1_t V
		)
		{
			// Result = P[0] * V[0] + P[1] * V[1] + P[2] * V[2] + P[3]

			vec_float_t V3 = VectorSelect(g_One.v, V, g_Select1110.v);
			vec_float_t Result = Vector4Dot(P, V3);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneDotNormal
		(
			vec_float_p1_t P,
			vec_float_p1_t V
		)
		{
			return Vector3Dot(P, V);
		}

		//------------------------------------------------------------------------------
		// PlaneNormalizeEst uses a reciprocal estimate and
		// returns QNaN on zero and infinite vectors.

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneNormalizeEst
		(
			vec_float_p1_t P
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)

			vec_float Result = Vector3ReciprocalLengthEst(P);
			return VectorMultiply(P, Result);

#elif defined(LUNA_PLATFORM_SSE4)
			vec_float vTemp = _mm_dp_ps(P, P, 0x7f);
			vec_float vResult = _mm_rsqrt_ps(vTemp);
			return _mm_mul_ps(vResult, P);
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product
			vec_float_t vDot = _mm_mul_ps(P, P);
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
			// Get the reciprocal
			vDot = _mm_mul_ps(vDot, P);
			return vDot;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneNormalize
		(
			vec_float_p1_t P
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float vLength = Vector3ReciprocalLength(P);
			return VectorMultiply(P, vLength);
#elif defined(LUNA_PLATFORM_SSE4)
			vec_float vLengthSq = _mm_dp_ps(P, P, 0x7f);
			// Prepare for the division
			vec_float vResult = _mm_sqrt_ps(vLengthSq);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Reciprocal mul to perform the normalization
			vResult = _mm_div_ps(P, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vLengthSq);
			return vResult;
#elif defined(LUNA_PLATFORM_SSE2)
			// Perform the dot product on x,y and z only
			vec_float_t vLengthSq = _mm_mul_ps(P, P);
			vec_float_t vTemp = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(2, 1, 2, 1));
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vTemp = LUNA_SIMD_PERMUTE_PS(vTemp, _MM_SHUFFLE(1, 1, 1, 1));
			vLengthSq = _mm_add_ss(vLengthSq, vTemp);
			vLengthSq = LUNA_SIMD_PERMUTE_PS(vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
			// Prepare for the division
			vec_float_t vResult = _mm_sqrt_ps(vLengthSq);
			// Failsafe on zero (Or epsilon) length planes
			// If the length is infinity, set the elements to zero
			vLengthSq = _mm_cmpneq_ps(vLengthSq, g_Infinity);
			// Reciprocal mul to perform the normalization
			vResult = _mm_div_ps(P, vResult);
			// Any that are infinity, set to zero
			vResult = _mm_and_ps(vResult, vLengthSq);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneIntersectLine
		(
			vec_float_p1_t P,
			vec_float_p1_t LinePoint1,
			vec_float_p1_t LinePoint2
		)
		{
			vec_float_t V1 = Vector3Dot(P, LinePoint1);
			vec_float_t V2 = Vector3Dot(P, LinePoint2);
			vec_float_t D = VectorSubtract(V1, V2);

			vec_float_t VT = PlaneDotCoord(P, LinePoint1);
			VT = VectorDivide(VT, D);

			vec_float_t Point = VectorSubtract(LinePoint2, LinePoint1);
			Point = VectorMultiplyAdd(Point, VT, LinePoint1);

			const vec_float_t Zero = vector_zero();
			vec_float_t Control = VectorNearEqual(D, Zero, g_Epsilon.v);

			return VectorSelect(Point, g_QNaN.v, Control);
		}

		//------------------------------------------------------------------------------

		inline void LUNA_SIMD_CALLCONV PlaneIntersectPlane
		(
			vec_float_t* pLinePoint1,
			vec_float_t* pLinePoint2,
			vec_float_p1_t  P1,
			vec_float_p1_t  P2
		)
		{
			luassert(pLinePoint1);
			luassert(pLinePoint2);

			vec_float_t V1 = Vector3Cross(P2, P1);

			vec_float_t LengthSq = Vector3LengthSq(V1);

			vec_float_t V2 = Vector3Cross(P2, V1);

			vec_float_t P1W = vector_splat_w(P1);
			vec_float_t Point = VectorMultiply(V2, P1W);

			vec_float_t V3 = Vector3Cross(V1, P1);

			vec_float_t P2W = vector_splat_w(P2);
			Point = VectorMultiplyAdd(V3, P2W, Point);

			vec_float_t LinePoint1 = VectorDivide(Point, LengthSq);

			vec_float_t LinePoint2 = VectorAdd(LinePoint1, V1);

			vec_float_t Control = VectorLessOrEqual(LengthSq, g_Epsilon.v);
			*pLinePoint1 = VectorSelect(LinePoint1, g_QNaN.v, Control);
			*pLinePoint2 = VectorSelect(LinePoint2, g_QNaN.v, Control);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneTransform
		(
			vec_float_p1_t P,
			mat_float_p1_t M
		)
		{
			vec_float_t W = vector_splat_w(P);
			vec_float_t Z = vector_splat_z(P);
			vec_float_t Y = vector_splat_y(P);
			vec_float_t X = vector_splat_x(P);

			vec_float_t Result = VectorMultiply(W, M.r[3]);
			Result = VectorMultiplyAdd(Z, M.r[2], Result);
			Result = VectorMultiplyAdd(Y, M.r[1], Result);
			Result = VectorMultiplyAdd(X, M.r[0], Result);
			return Result;
		}

		//------------------------------------------------------------------------------

		inline f32* LUNA_SIMD_CALLCONV PlaneTransformStream
		(
			f32*       pOutputStream,
			usize          OutputStride,
			const f32* pInputStream,
			usize          InputStride,
			usize          PlaneCount,
			mat_float_p1_t       M
		)
		{
			return Vector4TransformStream(pOutputStream,
				OutputStride,
				pInputStream,
				InputStride,
				PlaneCount,
				M);
		}

		//------------------------------------------------------------------------------
		// Conversion operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneFromPointNormal
		(
			vec_float_p1_t Point,
			vec_float_p1_t Normal
		)
		{
			vec_float_t W = Vector3Dot(Point, Normal);
			W = VectorNegate(W);
			return VectorSelect(W, Normal, g_Select1110.v);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV PlaneFromPoints
		(
			vec_float_p1_t Point1,
			vec_float_p1_t Point2,
			vec_float_p1_t Point3
		)
		{
			vec_float_t V21 = VectorSubtract(Point1, Point2);
			vec_float_t V31 = VectorSubtract(Point1, Point3);

			vec_float_t N = Vector3Cross(V21, V31);
			N = Vector3Normalize(N);

			vec_float_t D = PlaneDotNormal(N, Point1);
			D = VectorNegate(D);

			vec_float_t Result = VectorSelect(D, N, g_Select1110.v);

			return Result;
		}

		/****************************************************************************
		 *
		 * Color
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------
		 // Comparison operations
		 //------------------------------------------------------------------------------

		 //------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorEqual
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return Vector4Equal(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorNotEqual
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return Vector4NotEqual(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorGreater
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return Vector4Greater(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorGreaterOrEqual
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return Vector4GreaterOrEqual(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorLess
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return Vector4Less(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorLessOrEqual
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return Vector4LessOrEqual(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorIsNaN
		(
			vec_float_p1_t C
		)
		{
			return Vector4IsNaN(C);
		}

		//------------------------------------------------------------------------------

		inline bool LUNA_SIMD_CALLCONV ColorIsInfinite
		(
			vec_float_p1_t C
		)
		{
			return Vector4IsInfinite(C);
		}

		//------------------------------------------------------------------------------
		// Computation operations
		//------------------------------------------------------------------------------

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorNegative
		(
			vec_float_p1_t vColor
		)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float vTemp = veorq_u32(vColor, g_Negate3);
			return vaddq_f32(vTemp, g_One3);
#elif defined(LUNA_PLATFORM_SSE2)
			// Negate only x,y and z.
			vec_float_t vTemp = _mm_xor_ps(vColor, g_Negate3);
			// Add 1,1,1,0 to -x,-y,-z,w
			return _mm_add_ps(vTemp, g_One3);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorModulate
		(
			vec_float_p1_t C1,
			vec_float_p1_t C2
		)
		{
			return VectorMultiply(C1, C2);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorAdjustSaturation
		(
			vec_float_p1_t vColor,
			f32    fSaturation
		)
		{
			// Luminance = 0.2125f * C[0] + 0.7154f * C[1] + 0.0721f * C[2];
			// Result = (C - Luminance) * Saturation + Luminance;

			const vec_f32_t gvLuminance = { { { 0.2125f, 0.7154f, 0.0721f, 0.0f } } };
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float vLuminance = Vector3Dot(vColor, gvLuminance);
			vec_float vResult = vsubq_f32(vColor, vLuminance);
			vResult = vmlaq_n_f32(vLuminance, vResult, fSaturation);
			return vbslq_f32(g_Select1110, vResult, vColor);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vLuminance = Vector3Dot(vColor, gvLuminance);
			// Splat fSaturation
			vec_float_t vSaturation = _mm_set_ps1(fSaturation);
			// vResult = ((vColor-vLuminance)*vSaturation)+vLuminance;
			vec_float_t vResult = _mm_sub_ps(vColor, vLuminance);
			vResult = _mm_mul_ps(vResult, vSaturation);
			vResult = _mm_add_ps(vResult, vLuminance);
			// Retain w from the source color
			vLuminance = _mm_shuffle_ps(vResult, vColor, _MM_SHUFFLE(3, 2, 2, 2));   // x = vResult.z,y = vResult.z,z = vColor.z,w=vColor.w
			vResult = _mm_shuffle_ps(vResult, vLuminance, _MM_SHUFFLE(3, 0, 1, 0));  // x = vResult.x,y = vResult.y,z = vResult.z,w=vColor.w
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorAdjustContrast
		(
			vec_float_p1_t vColor,
			f32    fContrast
		)
		{
			// Result = (vColor - 0.5f) * fContrast + 0.5f;

#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_float vResult = vsubq_f32(vColor, g_OneHalf.v);
			vResult = vmlaq_n_f32(g_OneHalf.v, vResult, fContrast);
			return vbslq_f32(g_Select1110, vResult, vColor);
#elif defined(LUNA_PLATFORM_SSE2)
			vec_float_t vScale = _mm_set_ps1(fContrast);           // Splat the scale
			vec_float_t vResult = _mm_sub_ps(vColor, g_OneHalf);  // Subtract 0.5f from the source (Saving source)
			vResult = _mm_mul_ps(vResult, vScale);               // Mul by scale
			vResult = _mm_add_ps(vResult, g_OneHalf);          // Add 0.5f
		// Retain w from the source color
			vScale = _mm_shuffle_ps(vResult, vColor, _MM_SHUFFLE(3, 2, 2, 2));   // x = vResult.z,y = vResult.z,z = vColor.z,w=vColor.w
			vResult = _mm_shuffle_ps(vResult, vScale, _MM_SHUFFLE(3, 0, 1, 0));  // x = vResult.x,y = vResult.y,z = vResult.z,w=vColor.w
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorRGBToHSL(vec_float_p1_t rgb)
		{
			vec_float_t r = vector_splat_x(rgb);
			vec_float_t g = vector_splat_y(rgb);
			vec_float_t b = vector_splat_z(rgb);

			vec_float_t min = VectorMin(r, VectorMin(g, b));
			vec_float_t max = VectorMax(r, VectorMax(g, b));

			vec_float_t l = VectorMultiply(VectorAdd(min, max), g_OneHalf);

			vec_float_t d = VectorSubtract(max, min);

			vec_float_t la = VectorSelect(rgb, l, g_Select1110);

			if (Vector3Less(d, g_Epsilon))
			{
				// Achromatic, assume H and S of 0
				return VectorSelect(la, g_Zero, g_Select1100);
			}
			else
			{
				vec_float_t s, h;

				vec_float_t d2 = VectorAdd(min, max);

				if (Vector3Greater(l, g_OneHalf))
				{
					// d / (2-max-min)
					s = VectorDivide(d, VectorSubtract(g_Two, d2));
				}
				else
				{
					// d / (max+min)
					s = VectorDivide(d, d2);
				}

				if (Vector3Equal(r, max))
				{
					// Red is max
					h = VectorDivide(VectorSubtract(g, b), d);
				}
				else if (Vector3Equal(g, max))
				{
					// Green is max
					h = VectorDivide(VectorSubtract(b, r), d);
					h = VectorAdd(h, g_Two);
				}
				else
				{
					// Blue is max
					h = VectorDivide(VectorSubtract(r, g), d);
					h = VectorAdd(h, g_Four);
				}

				h = VectorDivide(h, g_Six);

				if (Vector3Less(h, g_Zero))
					h = VectorAdd(h, g_One);

				vec_float_t lha = VectorSelect(la, h, g_Select1100);
				return VectorSelect(s, lha, g_Select1011);
			}
		}

		//------------------------------------------------------------------------------

		namespace Internal
		{

			inline vec_float_t LUNA_SIMD_CALLCONV ColorHue2Clr(vec_float_p1_t p, vec_float_p1_t q, vec_float_p1_t h)
			{
				static const vec_f32_t oneSixth = { { { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f } } };
				static const vec_f32_t twoThirds = { { { 2.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f } } };

				vec_float_t t = h;

				if (Vector3Less(t, g_Zero))
					t = VectorAdd(t, g_One);

				if (Vector3Greater(t, g_One))
					t = VectorSubtract(t, g_One);

				if (Vector3Less(t, oneSixth))
				{
					// p + (q - p) * 6 * t
					vec_float_t t1 = VectorSubtract(q, p);
					vec_float_t t2 = VectorMultiply(g_Six, t);
					return VectorMultiplyAdd(t1, t2, p);
				}

				if (Vector3Less(t, g_OneHalf))
					return q;

				if (Vector3Less(t, twoThirds))
				{
					// p + (q - p) * 6 * (2/3 - t)
					vec_float_t t1 = VectorSubtract(q, p);
					vec_float_t t2 = VectorMultiply(g_Six, VectorSubtract(twoThirds, t));
					return VectorMultiplyAdd(t1, t2, p);
				}

				return p;
			}

		} // namespace Internal

		inline vec_float_t LUNA_SIMD_CALLCONV ColorHSLToRGB(vec_float_p1_t hsl)
		{
			static const vec_f32_t oneThird = { { { 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f, 1.0f / 3.0f } } };

			vec_float_t s = vector_splat_y(hsl);
			vec_float_t l = vector_splat_z(hsl);

			if (Vector3NearEqual(s, g_Zero, g_Epsilon))
			{
				// Achromatic
				return VectorSelect(hsl, l, g_Select1110);
			}
			else
			{
				vec_float_t h = vector_splat_x(hsl);

				vec_float_t q;
				if (Vector3Less(l, g_OneHalf))
				{
					q = VectorMultiply(l, VectorAdd(g_One, s));
				}
				else
				{
					q = VectorSubtract(VectorAdd(l, s), VectorMultiply(l, s));
				}

				vec_float_t p = VectorSubtract(VectorMultiply(g_Two, l), q);

				vec_float_t r = Internal::ColorHue2Clr(p, q, VectorAdd(h, oneThird));
				vec_float_t g = Internal::ColorHue2Clr(p, q, h);
				vec_float_t b = Internal::ColorHue2Clr(p, q, VectorSubtract(h, oneThird));

				vec_float_t rg = VectorSelect(g, r, g_Select1000);
				vec_float_t ba = VectorSelect(hsl, b, g_Select1110);

				return VectorSelect(ba, rg, g_Select1100);
			}
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorRGBToHSV(vec_float_p1_t rgb)
		{
			vec_float_t r = vector_splat_x(rgb);
			vec_float_t g = vector_splat_y(rgb);
			vec_float_t b = vector_splat_z(rgb);

			vec_float_t min = VectorMin(r, VectorMin(g, b));
			vec_float_t v = VectorMax(r, VectorMax(g, b));

			vec_float_t d = VectorSubtract(v, min);

			vec_float_t s = (Vector3NearEqual(v, g_Zero, g_Epsilon)) ? g_Zero : VectorDivide(d, v);

			if (Vector3Less(d, g_Epsilon))
			{
				// Achromatic, assume H of 0
				vec_float_t hv = VectorSelect(v, g_Zero, g_Select1000);
				vec_float_t hva = VectorSelect(rgb, hv, g_Select1110);
				return VectorSelect(s, hva, g_Select1011);
			}
			else
			{
				vec_float_t h;

				if (Vector3Equal(r, v))
				{
					// Red is max
					h = VectorDivide(VectorSubtract(g, b), d);

					if (Vector3Less(g, b))
						h = VectorAdd(h, g_Six);
				}
				else if (Vector3Equal(g, v))
				{
					// Green is max
					h = VectorDivide(VectorSubtract(b, r), d);
					h = VectorAdd(h, g_Two);
				}
				else
				{
					// Blue is max
					h = VectorDivide(VectorSubtract(r, g), d);
					h = VectorAdd(h, g_Four);
				}

				h = VectorDivide(h, g_Six);

				vec_float_t hv = VectorSelect(v, h, g_Select1000);
				vec_float_t hva = VectorSelect(rgb, hv, g_Select1110);
				return VectorSelect(s, hva, g_Select1011);
			}
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorHSVToRGB(vec_float_p1_t hsv)
		{
			vec_float_t h = vector_splat_x(hsv);
			vec_float_t s = vector_splat_y(hsv);
			vec_float_t v = vector_splat_z(hsv);

			vec_float_t h6 = VectorMultiply(h, g_Six);

			vec_float_t i = VectorFloor(h6);
			vec_float_t f = VectorSubtract(h6, i);

			// p = v* (1-s)
			vec_float_t p = VectorMultiply(v, VectorSubtract(g_One, s));

			// q = v*(1-f*s)
			vec_float_t q = VectorMultiply(v, VectorSubtract(g_One, VectorMultiply(f, s)));

			// t = v*(1 - (1-f)*s)
			vec_float_t t = VectorMultiply(v, VectorSubtract(g_One, VectorMultiply(VectorSubtract(g_One, f), s)));

			auto ii = static_cast<int>(VectorGetX(VectorMod(i, g_Six)));

			vec_float_t _rgb;

			switch (ii)
			{
			case 0: // rgb = vtp
			{
				vec_float_t vt = VectorSelect(t, v, g_Select1000);
				_rgb = VectorSelect(p, vt, g_Select1100);
			}
			break;
			case 1: // rgb = qvp
			{
				vec_float_t qv = VectorSelect(v, q, g_Select1000);
				_rgb = VectorSelect(p, qv, g_Select1100);
			}
			break;
			case 2: // rgb = pvt
			{
				vec_float_t pv = VectorSelect(v, p, g_Select1000);
				_rgb = VectorSelect(t, pv, g_Select1100);
			}
			break;
			case 3: // rgb = pqv
			{
				vec_float_t pq = VectorSelect(q, p, g_Select1000);
				_rgb = VectorSelect(v, pq, g_Select1100);
			}
			break;
			case 4: // rgb = tpv
			{
				vec_float_t tp = VectorSelect(p, t, g_Select1000);
				_rgb = VectorSelect(v, tp, g_Select1100);
			}
			break;
			default: // rgb = vpq
			{
				vec_float_t vp = VectorSelect(p, v, g_Select1000);
				_rgb = VectorSelect(q, vp, g_Select1100);
			}
			break;
			}

			return VectorSelect(hsv, _rgb, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorRGBToYUV(vec_float_p1_t rgb)
		{
			static const vec_f32_t Scale0 = { { { 0.299f, -0.147f, 0.615f, 0.0f } } };
			static const vec_f32_t Scale1 = { { { 0.587f, -0.289f, -0.515f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { 0.114f, 0.436f, -0.100f, 0.0f } } };

			mat_float_t M(Scale0, Scale1, Scale2, g_Zero);
			vec_float_t clr = Vector3Transform(rgb, M);

			return VectorSelect(rgb, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorYUVToRGB(vec_float_p1_t yuv)
		{
			static const vec_f32_t Scale1 = { { { 0.0f, -0.395f, 2.032f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { 1.140f, -0.581f, 0.0f, 0.0f } } };

			mat_float_t M(g_One, Scale1, Scale2, g_Zero);
			vec_float_t clr = Vector3Transform(yuv, M);

			return VectorSelect(yuv, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorRGBToYUV_HD(vec_float_p1_t rgb)
		{
			static const vec_f32_t Scale0 = { { { 0.2126f, -0.0997f, 0.6150f, 0.0f } } };
			static const vec_f32_t Scale1 = { { { 0.7152f, -0.3354f, -0.5586f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { 0.0722f, 0.4351f, -0.0564f, 0.0f } } };

			mat_float_t M(Scale0, Scale1, Scale2, g_Zero);
			vec_float_t clr = Vector3Transform(rgb, M);

			return VectorSelect(rgb, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorYUVToRGB_HD(vec_float_p1_t yuv)
		{
			static const vec_f32_t Scale1 = { { { 0.0f, -0.2153f, 2.1324f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { 1.2803f, -0.3806f, 0.0f, 0.0f } } };

			mat_float_t M(g_One, Scale1, Scale2, g_Zero);
			vec_float_t clr = Vector3Transform(yuv, M);

			return VectorSelect(yuv, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorRGBToXYZ(vec_float_p1_t rgb)
		{
			static const vec_f32_t Scale0 = { { { 0.4887180f, 0.1762044f, 0.0000000f, 0.0f } } };
			static const vec_f32_t Scale1 = { { { 0.3106803f, 0.8129847f, 0.0102048f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { 0.2006017f, 0.0108109f, 0.9897952f, 0.0f } } };
			static const vec_f32_t Scale = { { { 1.f / 0.17697f, 1.f / 0.17697f, 1.f / 0.17697f, 0.0f } } };

			mat_float_t M(Scale0, Scale1, Scale2, g_Zero);
			vec_float_t clr = VectorMultiply(Vector3Transform(rgb, M), Scale);

			return VectorSelect(rgb, clr, g_Select1110);
		}

		inline vec_float_t LUNA_SIMD_CALLCONV ColorXYZToRGB(vec_float_p1_t xyz)
		{
			static const vec_f32_t Scale0 = { { { 2.3706743f, -0.5138850f, 0.0052982f, 0.0f } } };
			static const vec_f32_t Scale1 = { { { -0.9000405f, 1.4253036f, -0.0146949f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { -0.4706338f, 0.0885814f, 1.0093968f, 0.0f } } };
			static const vec_f32_t Scale = { { { 0.17697f, 0.17697f, 0.17697f, 0.0f } } };

			mat_float_t M(Scale0, Scale1, Scale2, g_Zero);
			vec_float_t clr = Vector3Transform(VectorMultiply(xyz, Scale), M);

			return VectorSelect(xyz, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorXYZToSRGB(vec_float_p1_t xyz)
		{
			static const vec_f32_t Scale0 = { { { 3.2406f, -0.9689f, 0.0557f, 0.0f } } };
			static const vec_f32_t Scale1 = { { { -1.5372f, 1.8758f, -0.2040f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { -0.4986f, 0.0415f, 1.0570f, 0.0f } } };
			static const vec_f32_t Cutoff = { { { 0.0031308f, 0.0031308f, 0.0031308f, 0.0f } } };
			static const vec_f32_t Exp = { { { 1.0f / 2.4f, 1.0f / 2.4f, 1.0f / 2.4f, 1.0f } } };

			mat_float_t M(Scale0, Scale1, Scale2, g_Zero);
			vec_float_t lclr = Vector3Transform(xyz, M);

			vec_float_t sel = VectorGreater(lclr, Cutoff);

			// clr = 12.92 * lclr for lclr <= 0.0031308f
			vec_float_t smallC = VectorMultiply(lclr, g_srgbScale);

			// clr = (1+a)*pow(lclr, 1/2.4) - a for lclr > 0.0031308 (where a = 0.055)
			vec_float_t largeC = VectorSubtract(VectorMultiply(g_srgbA1, VectorPow(lclr, Exp)), g_srgbA);

			vec_float_t clr = VectorSelect(smallC, largeC, sel);

			return VectorSelect(xyz, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorSRGBToXYZ(vec_float_p1_t srgb)
		{
			static const vec_f32_t Scale0 = { { { 0.4124f, 0.2126f, 0.0193f, 0.0f } } };
			static const vec_f32_t Scale1 = { { { 0.3576f, 0.7152f, 0.1192f, 0.0f } } };
			static const vec_f32_t Scale2 = { { { 0.1805f, 0.0722f, 0.9505f, 0.0f } } };
			static const vec_f32_t Cutoff = { { { 0.04045f, 0.04045f, 0.04045f, 0.0f } } };
			static const vec_f32_t Exp = { { { 2.4f, 2.4f, 2.4f, 1.0f } } };

			vec_float_t sel = VectorGreater(srgb, Cutoff);

			// lclr = clr / 12.92
			vec_float_t smallC = VectorDivide(srgb, g_srgbScale);

			// lclr = pow( (clr + a) / (1+a), 2.4 )
			vec_float_t largeC = VectorPow(VectorDivide(VectorAdd(srgb, g_srgbA), g_srgbA1), Exp);

			vec_float_t lclr = VectorSelect(smallC, largeC, sel);

			mat_float_t M(Scale0, Scale1, Scale2, g_Zero);
			vec_float_t clr = Vector3Transform(lclr, M);

			return VectorSelect(srgb, clr, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorRGBToSRGB(vec_float_p1_t rgb)
		{
			static const vec_f32_t Cutoff = { { { 0.0031308f, 0.0031308f, 0.0031308f, 1.f } } };
			static const vec_f32_t Linear = { { { 12.92f, 12.92f, 12.92f, 1.f } } };
			static const vec_f32_t Scale = { { { 1.055f, 1.055f, 1.055f, 1.f } } };
			static const vec_f32_t Bias = { { { 0.055f, 0.055f, 0.055f, 0.f } } };
			static const vec_f32_t InvGamma = { { { 1.0f / 2.4f, 1.0f / 2.4f, 1.0f / 2.4f, 1.f } } };

			vec_float_t V = VectorSaturate(rgb);
			vec_float_t V0 = VectorMultiply(V, Linear);
			vec_float_t V1 = VectorSubtract(VectorMultiply(Scale, VectorPow(V, InvGamma)), Bias);
			vec_float_t select = VectorLess(V, Cutoff);
			V = VectorSelect(V1, V0, select);
			return VectorSelect(rgb, V, g_Select1110);
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV ColorSRGBToRGB(vec_float_p1_t srgb)
		{
			static const vec_f32_t Cutoff = { { { 0.04045f, 0.04045f, 0.04045f, 1.f } } };
			static const vec_f32_t ILinear = { { { 1.f / 12.92f, 1.f / 12.92f, 1.f / 12.92f, 1.f } } };
			static const vec_f32_t Scale = { { { 1.f / 1.055f, 1.f / 1.055f, 1.f / 1.055f, 1.f } } };
			static const vec_f32_t Bias = { { { 0.055f, 0.055f, 0.055f, 0.f } } };
			static const vec_f32_t Gamma = { { { 2.4f, 2.4f, 2.4f, 1.f } } };

			vec_float_t V = VectorSaturate(srgb);
			vec_float_t V0 = VectorMultiply(V, ILinear);
			vec_float_t V1 = VectorPow(VectorMultiply(VectorAdd(V, Bias), Scale), Gamma);
			vec_float_t select = VectorGreater(V, Cutoff);
			V = VectorSelect(V0, V1, select);
			return VectorSelect(srgb, V, g_Select1110);
		}

		/****************************************************************************
		 *
		 * Miscellaneous
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------

		inline bool VerifyCPUSupport()
		{
#if defined(LUNA_PLATFORM_SSE2)
			int CPUInfo[4] = { -1 };
			__cpuid(CPUInfo, 0);

#ifdef __AVX2__
			if (CPUInfo[0] < 7)
				return false;
#else
			if (CPUInfo[0] < 1)
				return false;
#endif

			__cpuid(CPUInfo, 1);

#if defined(__AVX2__) || defined(LUNA_PLATFORM_AVX2)
			// The compiler can emit FMA3 instructions even without explicit intrinsics use
			if ((CPUInfo[2] & 0x38081001) != 0x38081001)
				return false; // No F16C/AVX/OSXSAVE/SSE4.1/FMA3/SSE3 support
#elif defined(LUNA_PLATFORM_FMA3) && defined(LUNA_PLATFORM_F16C)
			if ((CPUInfo[2] & 0x38081001) != 0x38081001)
				return false; // No F16C/AVX/OSXSAVE/SSE4.1/FMA3/SSE3 support
#elif defined(LUNA_PLATFORM_FMA3)
			if ((CPUInfo[2] & 0x18081001) != 0x18081001)
				return false; // No AVX/OSXSAVE/SSE4.1/FMA3/SSE3 support
#elif defined(LUNA_PLATFORM_F16C)
			if ((CPUInfo[2] & 0x38080001) != 0x38080001)
				return false; // No F16C/AVX/OSXSAVE/SSE4.1/SSE3 support
#elif defined(__AVX__) || defined(LUNA_PLATFORM_AVX)
			if ((CPUInfo[2] & 0x18080001) != 0x18080001)
				return false; // No AVX/OSXSAVE/SSE4.1/SSE3 support
#elif defined(LUNA_PLATFORM_SSE4)
			if ((CPUInfo[2] & 0x80001) != 0x80001)
				return false; // No SSE3/SSE4.1 support
#elif defined(LUNA_PLATFORM_SSE3)
			if (!(CPUInfo[2] & 0x1))
				return false; // No SSE3 support  
#endif

	// The x64 processor model requires SSE2 support, but no harm in checking
			if ((CPUInfo[3] & 0x6000000) != 0x6000000)
				return false; // No SSE2/SSE support

#if defined(__AVX2__) || defined(LUNA_PLATFORM_AVX2)
			__cpuidex(CPUInfo, 7, 0);
			if (!(CPUInfo[1] & 0x20))
				return false; // No AVX2 support
#endif

			return true;
#elif defined(LUNA_PLATFORM_ARM_NEON)
			// ARM-NEON support is required for the Windows on ARM platform
			return true;
#else
			// No intrinsics path always supported
			return true;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV FresnelTerm
		(
			vec_float_p1_t CosIncidentAngle,
			vec_float_p1_t RefractionIndex
		)
		{
			luassert(!Vector4IsInfinite(CosIncidentAngle));

			// Result = 0.5f * (g - c)^2 / (g + c)^2 * ((c * (g + c) - 1)^2 / (c * (g - c) + 1)^2 + 1) where
			// c = CosIncidentAngle
			// g = sqrt(c^2 + RefractionIndex^2 - 1)

#if defined(LUNA_PLATFORM_ARM_NEON)

			vec_float G = VectorMultiplyAdd(RefractionIndex, RefractionIndex, g_NegativeOne.v);
			G = VectorMultiplyAdd(CosIncidentAngle, CosIncidentAngle, G);
			G = VectorAbs(G);
			G = VectorSqrt(G);

			vec_float S = VectorAdd(G, CosIncidentAngle);
			vec_float D = VectorSubtract(G, CosIncidentAngle);

			vec_float V0 = VectorMultiply(D, D);
			vec_float V1 = VectorMultiply(S, S);
			V1 = VectorReciprocal(V1);
			V0 = VectorMultiply(g_OneHalf.v, V0);
			V0 = VectorMultiply(V0, V1);

			vec_float V2 = VectorMultiplyAdd(CosIncidentAngle, S, g_NegativeOne.v);
			vec_float V3 = VectorMultiplyAdd(CosIncidentAngle, D, g_One.v);
			V2 = VectorMultiply(V2, V2);
			V3 = VectorMultiply(V3, V3);
			V3 = VectorReciprocal(V3);
			V2 = VectorMultiplyAdd(V2, V3, g_One.v);

			vec_float Result = VectorMultiply(V0, V2);

			Result = VectorSaturate(Result);

			return Result;

#elif defined(LUNA_PLATFORM_SSE2)
	// G = sqrt(abs((RefractionIndex^2-1) + CosIncidentAngle^2))
			vec_float_t G = _mm_mul_ps(RefractionIndex, RefractionIndex);
			vec_float_t vTemp = _mm_mul_ps(CosIncidentAngle, CosIncidentAngle);
			G = _mm_sub_ps(G, g_One);
			vTemp = _mm_add_ps(vTemp, G);
			// max((0-vTemp),vTemp) == abs(vTemp)
			// The abs is needed to deal with refraction and cosine being zero
			G = _mm_setzero_ps();
			G = _mm_sub_ps(G, vTemp);
			G = _mm_max_ps(G, vTemp);
			// Last operation, the sqrt()
			G = _mm_sqrt_ps(G);

			// Calc G-C and G+C
			vec_float_t GAddC = _mm_add_ps(G, CosIncidentAngle);
			vec_float_t GSubC = _mm_sub_ps(G, CosIncidentAngle);
			// Perform the term (0.5f *(g - c)^2) / (g + c)^2 
			vec_float_t vResult = _mm_mul_ps(GSubC, GSubC);
			vTemp = _mm_mul_ps(GAddC, GAddC);
			vResult = _mm_mul_ps(vResult, g_OneHalf);
			vResult = _mm_div_ps(vResult, vTemp);
			// Perform the term ((c * (g + c) - 1)^2 / (c * (g - c) + 1)^2 + 1)
			GAddC = _mm_mul_ps(GAddC, CosIncidentAngle);
			GSubC = _mm_mul_ps(GSubC, CosIncidentAngle);
			GAddC = _mm_sub_ps(GAddC, g_One);
			GSubC = _mm_add_ps(GSubC, g_One);
			GAddC = _mm_mul_ps(GAddC, GAddC);
			GSubC = _mm_mul_ps(GSubC, GSubC);
			GAddC = _mm_div_ps(GAddC, GSubC);
			GAddC = _mm_add_ps(GAddC, g_One);
			// Multiply the two term parts
			vResult = _mm_mul_ps(vResult, GAddC);
			// clamp to 0.0 - 1.0f
			vResult = _mm_max_ps(vResult, g_Zero);
			vResult = _mm_min_ps(vResult, g_One);
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline bool ScalarNearEqual
		(
			f32 S1,
			f32 S2,
			f32 Epsilon
		)
		{
			f32 Delta = S1 - S2;
			return (fabsf(Delta) <= Epsilon);
		}

		//------------------------------------------------------------------------------
		// Modulo the range of the given angle such that -pi <= Angle < pi
		inline f32 ScalarModAngle
		(
			f32 Angle
		)
		{
			// Note: The modulo is performed with unsigned math only to work
			// around a precision error on numbers that are close to PI

			// normalize the range from 0.0f to two_pi
			Angle = Angle + pi;
			// Perform the modulo, unsigned
			f32 fTemp = fabsf(Angle);
			fTemp = fTemp - (two_pi * static_cast<f32>(static_cast<i32>(fTemp / two_pi)));
			// Restore the number to the range of -pi to pi-epsilon
			fTemp = fTemp - pi;
			// If the modulo'd value was negative, restore negation
			if (Angle < 0.0f) {
				fTemp = -fTemp;
			}
			return fTemp;
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarSin
		(
			f32 Value
		)
		{
			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			f32 quotient = one_div_two_pi * Value;
			if (Value >= 0.0f)
			{
				quotient = static_cast<f32>(static_cast<int>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<f32>(static_cast<int>(quotient - 0.5f));
			}
			f32 y = Value - two_pi * quotient;

			// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
			if (y > pi_div_two)
			{
				y = pi - y;
			}
			else if (y < -pi_div_two)
			{
				y = -pi - y;
			}

			// 11-degree minimax approximation
			f32 y2 = y * y;
			return (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarSinEst
		(
			f32 Value
		)
		{
			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			f32 quotient = one_div_two_pi * Value;
			if (Value >= 0.0f)
			{
				quotient = static_cast<f32>(static_cast<int>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<f32>(static_cast<int>(quotient - 0.5f));
			}
			f32 y = Value - two_pi * quotient;

			// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
			if (y > pi_div_two)
			{
				y = pi - y;
			}
			else if (y < -pi_div_two)
			{
				y = -pi - y;
			}

			// 7-degree minimax approximation
			f32 y2 = y * y;
			return (((-0.00018524670f * y2 + 0.0083139502f) * y2 - 0.16665852f) * y2 + 1.0f) * y;
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarCos
		(
			f32 Value
		)
		{
			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			f32 quotient = one_div_two_pi * Value;
			if (Value >= 0.0f)
			{
				quotient = static_cast<f32>(static_cast<int>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<f32>(static_cast<int>(quotient - 0.5f));
			}
			f32 y = Value - two_pi * quotient;

			// Map y to [-pi/2,pi/2] with cos(y) = sign*cos(x).
			f32 sign;
			if (y > pi_div_two)
			{
				y = pi - y;
				sign = -1.0f;
			}
			else if (y < -pi_div_two)
			{
				y = -pi - y;
				sign = -1.0f;
			}
			else
			{
				sign = +1.0f;
			}

			// 10-degree minimax approximation
			f32 y2 = y * y;
			f32 p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
			return sign * p;
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarCosEst
		(
			f32 Value
		)
		{
			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			f32 quotient = one_div_two_pi * Value;
			if (Value >= 0.0f)
			{
				quotient = static_cast<f32>(static_cast<int>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<f32>(static_cast<int>(quotient - 0.5f));
			}
			f32 y = Value - two_pi * quotient;

			// Map y to [-pi/2,pi/2] with cos(y) = sign*cos(x).
			f32 sign;
			if (y > pi_div_two)
			{
				y = pi - y;
				sign = -1.0f;
			}
			else if (y < -pi_div_two)
			{
				y = -pi - y;
				sign = -1.0f;
			}
			else
			{
				sign = +1.0f;
			}

			// 6-degree minimax approximation
			f32 y2 = y * y;
			f32 p = ((-0.0012712436f * y2 + 0.041493919f) * y2 - 0.49992746f) * y2 + 1.0f;
			return sign * p;
		}

		//------------------------------------------------------------------------------


		inline void ScalarSinCos
		(
			f32* pSin,
			f32* pCos,
			f32  Value
		)
		{
			luassert(pSin);
			luassert(pCos);

			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			f32 quotient = one_div_two_pi * Value;
			if (Value >= 0.0f)
			{
				quotient = static_cast<f32>(static_cast<int>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<f32>(static_cast<int>(quotient - 0.5f));
			}
			f32 y = Value - two_pi * quotient;

			// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
			f32 sign;
			if (y > pi_div_two)
			{
				y = pi - y;
				sign = -1.0f;
			}
			else if (y < -pi_div_two)
			{
				y = -pi - y;
				sign = -1.0f;
			}
			else
			{
				sign = +1.0f;
			}

			f32 y2 = y * y;

			// 11-degree minimax approximation
			*pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

			// 10-degree minimax approximation
			f32 p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
			*pCos = sign * p;
		}

		//------------------------------------------------------------------------------


		inline void ScalarSinCosEst
		(
			f32* pSin,
			f32* pCos,
			f32  Value
		)
		{
			luassert(pSin);
			luassert(pCos);

			// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
			f32 quotient = one_div_two_pi * Value;
			if (Value >= 0.0f)
			{
				quotient = static_cast<f32>(static_cast<int>(quotient + 0.5f));
			}
			else
			{
				quotient = static_cast<f32>(static_cast<int>(quotient - 0.5f));
			}
			f32 y = Value - two_pi * quotient;

			// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
			f32 sign;
			if (y > pi_div_two)
			{
				y = pi - y;
				sign = -1.0f;
			}
			else if (y < -pi_div_two)
			{
				y = -pi - y;
				sign = -1.0f;
			}
			else
			{
				sign = +1.0f;
			}

			f32 y2 = y * y;

			// 7-degree minimax approximation
			*pSin = (((-0.00018524670f * y2 + 0.0083139502f) * y2 - 0.16665852f) * y2 + 1.0f) * y;

			// 6-degree minimax approximation
			f32 p = ((-0.0012712436f * y2 + 0.041493919f) * y2 - 0.49992746f) * y2 + 1.0f;
			*pCos = sign * p;
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarASin
		(
			f32 Value
		)
		{
			// clamp input to [-1,1].
			bool nonnegative = (Value >= 0.0f);
			f32 x = fabsf(Value);
			f32 omx = 1.0f - x;
			if (omx < 0.0f)
			{
				omx = 0.0f;
			}
			f32 root = sqrtf(omx);

			// 7-degree minimax approximation
			f32 result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + 1.5707963050f;
			result *= root;  // acos(|x|)

			// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
			return (nonnegative ? pi_div_two - result : result - pi_div_two);
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarASinEst
		(
			f32 Value
		)
		{
			// clamp input to [-1,1].
			bool nonnegative = (Value >= 0.0f);
			f32 x = fabsf(Value);
			f32 omx = 1.0f - x;
			if (omx < 0.0f)
			{
				omx = 0.0f;
			}
			f32 root = sqrtf(omx);

			// 3-degree minimax approximation
			f32 result = ((-0.0187293f*x + 0.0742610f)*x - 0.2121144f)*x + 1.5707288f;
			result *= root;  // acos(|x|)

			// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
			return (nonnegative ? pi_div_two - result : result - pi_div_two);
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarACos
		(
			f32 Value
		)
		{
			// clamp input to [-1,1].
			bool nonnegative = (Value >= 0.0f);
			f32 x = fabsf(Value);
			f32 omx = 1.0f - x;
			if (omx < 0.0f)
			{
				omx = 0.0f;
			}
			f32 root = sqrtf(omx);

			// 7-degree minimax approximation
			f32 result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + 1.5707963050f;
			result *= root;

			// acos(x) = pi - acos(-x) when x < 0
			return (nonnegative ? result : pi - result);
		}

		//------------------------------------------------------------------------------

		inline f32 ScalarACosEst
		(
			f32 Value
		)
		{
			// clamp input to [-1,1].
			bool nonnegative = (Value >= 0.0f);
			f32 x = fabsf(Value);
			f32 omx = 1.0f - x;
			if (omx < 0.0f)
			{
				omx = 0.0f;
			}
			f32 root = sqrtf(omx);

			// 3-degree minimax approximation
			f32 result = ((-0.0187293f * x + 0.0742610f) * x - 0.2121144f) * x + 1.5707288f;
			result *= root;

			// acos(x) = pi - acos(-x) when x < 0
			return (nonnegative ? result : pi - result);
		}
	}
}

#endif