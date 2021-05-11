// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Simd.hpp
* @author JxMaster
* @date 2019/1/2
* @brief Multi-platform SIMD Library modified from Microsoft's DirectX Math.
* See: https://github.com/Microsoft/DirectXMath
 */
#pragma once

#include "MathBase.hpp"
#include "../../Assert.hpp"

#if (defined(LUNA_PLATFORM_SSE2) || defined(LUNA_PLATFORM_ARM_NEON)) && !defined(LUNA_DISABLE_SIMD) && !defined(LUNA_SIMD_ENABLED)
#define LUNA_SIMD_ENABLED	1
#endif

#if defined(LUNA_SIMD_ENABLED)

#if defined(LUNA_COMPILER_MSVC) && !defined(LUNA_PLATFORM_ARM32) && !defined(LUNA_PLATFORM_ARM64)
#define LUNA_SIMD_VECTORCALL_ENABLED 1
#endif

#if LUNA_SIMD_VECTORCALL_ENABLED
#define LUNA_SIMD_CALLCONV __vectorcall
#else
#define LUNA_SIMD_CALLCONV __fastcall
#endif

#if defined(LUNA_PLATFORM_SSE2)
#include <xmmintrin.h>
#include <emmintrin.h>
#elif defined(LUNA_PLATFORM_ARM_NEON)
#if defined(LUNA_PLATFORM_ARM64)
#include <arm64_neon.h>
#else
#include <arm_neon.h>
#endif
#endif

#include <intrin.h>

#endif // LUNA_SIMD_ENABLED

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

/****************************************************************************
*
*	Conditional intrinsics
*
****************************************************************************/

#if defined(LUNA_PLATFORM_SSE2) && defined(LUNA_SIMD_ENABLED)

#if defined(LUNA_SIMD_NO_MOVNT)
#define LUNA_SIMD_STREAM_PS( p, a ) _mm_store_ps( p, a )
#define LUNA_SIMD_SFENCE()
#else
#define LUNA_SIMD_STREAM_PS( p, a ) _mm_stream_ps( p, a )
#define LUNA_SIMD_SFENCE() _mm_sfence()
#endif

#if defined(LUNA_PLATFORM_AVX)
#define LUNA_SIMD_PERMUTE_PS( v, c ) _mm_permute_ps( v, c )
#else
#define LUNA_SIMD_PERMUTE_PS( v, c ) _mm_shuffle_ps( v, v, c )
#endif

#endif // LUNA_PLATFORM_SSE2 && LUNA_SIMD_ENABLED

namespace Luna
{
#if defined(LUNA_SIMD_ENABLED)
	namespace Simd
	{
		constexpr u32 g_select_0 = 0x00000000;
		constexpr u32 g_select_1 = 0xFFFFFFFF;

		constexpr u32 g_permute_0x = 0;
		constexpr u32 g_permute_0y = 1;
		constexpr u32 g_permute_0z = 2;
		constexpr u32 g_permute_0w = 3;
		constexpr u32 g_permute_1x = 4;
		constexpr u32 g_permute_1y = 5;
		constexpr u32 g_permute_1z = 6;
		constexpr u32 g_permute_1w = 7;

		constexpr u32 g_swizzle_x = 0;
		constexpr u32 g_swizzle_y = 1;
		constexpr u32 g_swizzle_z = 2;
		constexpr u32 g_swizzle_w = 3;

		constexpr u32 g_crmask_cr6 = 0x000000F0;
		constexpr u32 g_crmask_cr6true = 0x00000080;
		constexpr u32 g_crmask_cr6false = 0x00000020;
		constexpr u32 g_crmask_cr6bounds = g_crmask_cr6false;

		constexpr usize g_cache_line_size = 64;

		// Condition register evaluation proceeding a recording (R) comparison

		inline bool comparison_all_true(u32 CR) { return (((CR)& g_crmask_cr6true) == g_crmask_cr6true); }
		inline bool comparison_any_true(u32 CR) { return (((CR)& g_crmask_cr6false) != g_crmask_cr6false); }
		inline bool comparison_all_false(u32 CR) { return (((CR)& g_crmask_cr6false) == g_crmask_cr6false); }
		inline bool comparison_any_false(u32 CR) { return (((CR)& g_crmask_cr6true) != g_crmask_cr6true); }
		inline bool comparison_mixed(u32 CR) { return (((CR)& g_crmask_cr6) == 0); }
		inline bool comparison_all_in_bounds(u32 CR) { return (((CR)& g_crmask_cr6bounds) == g_crmask_cr6bounds); }
		inline bool comparison_any_out_of_bounds(u32 CR) { return (((CR)& g_crmask_cr6bounds) != g_crmask_cr6bounds); }

		/****************************************************************************
		 *
		 * Data types
		 *
		 ****************************************************************************/

#if defined(LUNA_PLATFORM_SSE2)
		using vec_float_t = __m128;
		using vec_int32_t = __m128i;
#elif defined(LUNA_PLATFORM_ARM_NEON)
		using vec_float_t = float32x4_t;
		using vec_int32_t = int32x4_t;
#else
#error "Unsupported SIMD Target."
#endif

		 // Fix-up for (1st-3rd) vec_float_t parameters that are pass-in-register for x86, ARM, ARM64, and vector call; by reference otherwise
#if defined(LUNA_PLATFORM_X86) || defined(LUNA_PLATFORM_ARM32) || defined(LUNA_PLATFORM_ARM64) || LUNA_SIMD_VECTORCALL_ENABLED 
		using vec_float_p1_t = const vec_float_t;
		//using vec_int32_p1 = const vec_int32_t;
		//using vec_uint32_p1 = const vec_uint32_t;
#else
		using vec_float_p1_t = const vec_float_t&;
		//using vec_int32_p1 = const vec_int32_t&;
		//using vec_uint32_p1 = const vec_uint32_t&;
#endif

		// Fix-up for (4th) vec_float_t parameter to pass in-register for ARM, ARM64, and x64 vector call; by reference otherwise
#if defined(LUNA_PLATFORM_ARM32) || defined(LUNA_PLATFORM_ARM64) || (LUNA_SIMD_VECTORCALL_ENABLED && !defined(LUNA_PLATFORM_X86) )
		using vec_float_p2_t = const vec_float_t;
		//using vec_int32_p2 = const vec_int32_t;
		//using vec_uint32_p2 = const vec_uint32_t;
#else
		using vec_float_p2_t = const vec_float_t&;
		//using vec_int32_p2 = const vec_int32_t&;
		//using vec_uint32_p2 = const vec_uint32_t&;
#endif

		// Fix-up for (5th & 6th) vec_float_t parameter to pass in-register for ARM64 and vector call; by reference otherwise
#if defined(LUNA_PLATFORM_ARM64) || defined(LUNA_SIMD_VECTORCALL_ENABLED)
		using vec_float_p3_t = const vec_float_t;
		//using vec_int32_p3 = const vec_int32_t;
		//using vec_uint32_p3 = const vec_uint32_t;
#else
		using vec_float_p3_t = const vec_float_t&;
		//using vec_int32_p3 = const vec_int32_t&;
		//using vec_uint32_p3 = const vec_uint32_t&;
#endif

		// Fix-up for (7th+) vec_float_t parameters to pass by reference
		using vec_float_p4_t = const vec_float_t&;
		//using vec_int32_p4 = const vec_int32_t&;
		//using vec_uint32_p4 = const vec_uint32_t&;

		//------------------------------------------------------------------------------
		// Conversion types for constants
		struct alignas(16) vec_f32_t
		{
			union
			{
				f32 f[4];
				vec_float_t v;
			};

			inline operator vec_float_t() const { return v; }
			inline operator const f32*() const { return f; }
#if defined(LUNA_PLATFORM_SSE2)
			inline operator __m128i() const { return _mm_castps_si128(v); }
			inline operator __m128d() const { return _mm_castps_pd(v); }
#endif
		};

		struct alignas(16) vec_i32_t
		{
			union
			{
				int32_t i[4];
				vec_float_t v;
			};

			inline operator vec_float_t() const { return v; }
#if defined(LUNA_PLATFORM_SSE2)
			inline operator __m128i() const { return _mm_castps_si128(v); }
			inline operator __m128d() const { return _mm_castps_pd(v); }
#endif
		};

		struct alignas(16) vec_u8_t
		{
			union
			{
				uint8_t u[16];
				vec_float_t v;
			};

			inline operator vec_float_t() const { return v; }
#if defined(LUNA_PLATFORM_SSE2)
			inline operator __m128i() const { return _mm_castps_si128(v); }
			inline operator __m128d() const { return _mm_castps_pd(v); }
#endif
		};

		struct alignas(16) vec_u32_t
		{
			union
			{
				uint32_t u[4];
				vec_float_t v;
			};

			inline operator vec_float_t() const { return v; }
#if defined(LUNA_PLATFORM_SSE2)
			inline operator __m128i() const { return _mm_castps_si128(v); }
			inline operator __m128d() const { return _mm_castps_pd(v); }
#endif
		};

		//------------------------------------------------------------------------------
		// Matrix type: Sixteen 32 bit floating point components aligned on a
		// 16 byte boundary and mapped to four hardware vector registers

		struct mat_float_t;

		// Fix-up for (1st) mat_float_t parameter to pass in-register for ARM64 and vector call; by reference otherwise
#if defined(LUNA_PLATFORM_ARM64) || LUNA_SIMD_VECTORCALL_ENABLED 
		using mat_float_p1_t = const mat_float_t;
#else
		using mat_float_p1_t = const mat_float_t&;
#endif

		// Fix-up for (2nd+) mat_float_t parameters to pass by reference
		using mat_float_p2_t = const mat_float_t&;

		struct alignas(16) mat_float_t
		{
			vec_float_t r[4];

			mat_float_t() = default;
			mat_float_t(const mat_float_t&) = default;
			mat_float_t& operator=(const mat_float_t&) = default;
			mat_float_t(mat_float_t&&) = default;
			mat_float_t& operator=(mat_float_t&&) = default;

			constexpr mat_float_t(vec_float_p1_t R0, vec_float_p1_t R1, vec_float_p1_t R2, vec_float_p4_t R3) : r{ R0,R1,R2,R3 } {}

			mat_float_t(f32 m00, f32 m01, f32 m02, f32 m03,
				f32 m10, f32 m11, f32 m12, f32 m13,
				f32 m20, f32 m21, f32 m22, f32 m23,
				f32 m30, f32 m31, f32 m32, f32 m33);
			explicit mat_float_t(const f32* pArray);

			mat_float_t    operator+ () const { return *this; }
			mat_float_t    operator- () const;

			mat_float_t&   LUNA_SIMD_CALLCONV     operator+= (mat_float_p1_t M);
			mat_float_t&   LUNA_SIMD_CALLCONV     operator-= (mat_float_p1_t M);
			mat_float_t&   LUNA_SIMD_CALLCONV     operator*= (mat_float_p1_t M);
			mat_float_t&   operator*= (f32 S);
			mat_float_t&   operator/= (f32 S);

			mat_float_t    LUNA_SIMD_CALLCONV     operator+ (mat_float_p1_t M) const;
			mat_float_t    LUNA_SIMD_CALLCONV     operator- (mat_float_p1_t M) const;
			mat_float_t    LUNA_SIMD_CALLCONV     operator* (mat_float_p1_t M) const;
			mat_float_t    operator* (f32 S) const;
			mat_float_t    operator/ (f32 S) const;

			friend mat_float_t     LUNA_SIMD_CALLCONV     operator* (f32 S, mat_float_p1_t M);
		};

		/****************************************************************************
		 *
		 *	Data conversion operations
		 *
		 ****************************************************************************/

		vec_float_t LUNA_SIMD_CALLCONV convert_vector_int_to_float(vec_float_p1_t vint, u32 divExponent);
		vec_float_t LUNA_SIMD_CALLCONV convert_vector_float_to_int(vec_float_p1_t vfloat, u32 mulExponent);
		vec_float_t LUNA_SIMD_CALLCONV convert_vector_uint_to_float(vec_float_p1_t vuint, u32 divExponent);
		vec_float_t LUNA_SIMD_CALLCONV convert_vector_float_to_uint(vec_float_p1_t vfloat, u32 mulExponent);
		vec_float_t LUNA_SIMD_CALLCONV vector_set_binary_constant(u32 c0, u32 c1, u32 c2, u32 c3);
		vec_float_t LUNA_SIMD_CALLCONV vector_splat_constant(i32 intConstant, u32 divExponent);
		vec_float_t LUNA_SIMD_CALLCONV vector_splat_constant_int(i32 intConstant);

		/****************************************************************************
		 *
		 *	Load operations
		 *
		 ****************************************************************************/

		vec_float_t    LUNA_SIMD_CALLCONV     load_uint(const u32* pSource);
		vec_float_t	   LUNA_SIMD_CALLCONV	  load_int(const i32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float(const f32* pSource);

		vec_float_t    LUNA_SIMD_CALLCONV     load_uint2(const u32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_uint2a(const u32* PSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_int2(const i32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_int2a(const i32* PSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float2(const f32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float2a(const f32* pSource);

		vec_float_t    LUNA_SIMD_CALLCONV     load_uint3(const u32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_uint3a(const u32* PSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_int3(const i32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_int3a(const i32* PSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float3(const f32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float3a(const f32* pSource);

		vec_float_t    LUNA_SIMD_CALLCONV     load_uint4(const u32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_uint4a(const u32* PSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_int4(const i32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_int4a(const i32* PSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float4(const f32* pSource);
		vec_float_t    LUNA_SIMD_CALLCONV     load_float4a(const f32* pSource);

		mat_float_t    LUNA_SIMD_CALLCONV     load_float3x3(const f32* pSource);
		mat_float_t    LUNA_SIMD_CALLCONV     load_float4x3(const f32* pSource);
		mat_float_t    LUNA_SIMD_CALLCONV     load_float4x3a(const f32* pSource);
		mat_float_t    LUNA_SIMD_CALLCONV     load_float3x4(const f32* pSource);
		mat_float_t    LUNA_SIMD_CALLCONV     load_float3x4a(const f32* pSource);
		mat_float_t    LUNA_SIMD_CALLCONV     load_float4x4(const f32* pSource);
		mat_float_t    LUNA_SIMD_CALLCONV     load_float4x4a(const f32* pSource);

		/****************************************************************************
		 *
		 *	Store operations
		 *
		 ****************************************************************************/

		void        LUNA_SIMD_CALLCONV     store_int(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float(f32* pDestination, vec_float_p1_t V);

		void        LUNA_SIMD_CALLCONV     store_int2(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_int2a(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint2(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint2a(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float2(f32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float2a(f32* pDestination, vec_float_p1_t V);

		void        LUNA_SIMD_CALLCONV     store_int3(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_int3a(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint3(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint3a(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float3(f32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float3a(f32* pDestination, vec_float_p1_t V);

		void        LUNA_SIMD_CALLCONV     store_int4(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_int4a(i32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint4(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_uint4a(u32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float4(f32* pDestination, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     store_float4a(f32* pDestination, vec_float_p1_t V);

		void        LUNA_SIMD_CALLCONV     store_float3x3(f32* pDestination, mat_float_p1_t M);
		void        LUNA_SIMD_CALLCONV     store_float4x3(f32* pDestination, mat_float_p1_t M);
		void        LUNA_SIMD_CALLCONV     store_float4x3a(f32* pDestination, mat_float_p1_t M);
		void        LUNA_SIMD_CALLCONV     store_float3x4(f32* pDestination, mat_float_p1_t M);
		void        LUNA_SIMD_CALLCONV     store_float3x4a(f32* pDestination, mat_float_p1_t M);
		void        LUNA_SIMD_CALLCONV     store_float4x4(f32* pDestination, mat_float_p1_t M);
		void        LUNA_SIMD_CALLCONV     store_float4x4a(f32* pDestination, mat_float_p1_t M);

		/****************************************************************************
		 *
		 *	General vector operations
		 *
		 ****************************************************************************/

		vec_float_t    LUNA_SIMD_CALLCONV     vector_zero();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_set(f32 x, f32 y, f32 z, f32 w);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_set_int(u32 x, u32 y, u32 z, u32 w);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_replicate(f32 Value);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_replicate_ptr(const f32* pValue);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_replicate_int(u32 Value);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_replicate_int_ptr(const u32* pValue);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_true_int();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_false_int();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_x(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_y(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_z(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_w(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_one();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_infinity();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_qnan();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_epsilon();
		vec_float_t    LUNA_SIMD_CALLCONV     vector_splat_sign_mask();

		f32       LUNA_SIMD_CALLCONV     VectorGetByIndex(vec_float_p1_t V, usize i);
		f32       LUNA_SIMD_CALLCONV     VectorGetX(vec_float_p1_t V);
		f32       LUNA_SIMD_CALLCONV     VectorGetY(vec_float_p1_t V);
		f32       LUNA_SIMD_CALLCONV     VectorGetZ(vec_float_p1_t V);
		f32       LUNA_SIMD_CALLCONV     VectorGetW(vec_float_p1_t V);

		void        LUNA_SIMD_CALLCONV     VectorGetByIndexPtr(f32* f, vec_float_p1_t V, usize i);
		void        LUNA_SIMD_CALLCONV     VectorGetXPtr(f32* x, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorGetYPtr(f32* y, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorGetZPtr(f32* z, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorGetWPtr(f32* w, vec_float_p1_t V);

		u32      LUNA_SIMD_CALLCONV     VectorGetIntByIndex(vec_float_p1_t V, usize i);
		u32      LUNA_SIMD_CALLCONV     VectorGetIntX(vec_float_p1_t V);
		u32      LUNA_SIMD_CALLCONV     VectorGetIntY(vec_float_p1_t V);
		u32      LUNA_SIMD_CALLCONV     VectorGetIntZ(vec_float_p1_t V);
		u32      LUNA_SIMD_CALLCONV     VectorGetIntW(vec_float_p1_t V);

		void        LUNA_SIMD_CALLCONV     VectorGetIntByIndexPtr(u32* x, vec_float_p1_t V, usize i);
		void        LUNA_SIMD_CALLCONV     VectorGetIntXPtr(u32* x, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorGetIntYPtr(u32* y, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorGetIntZPtr(u32* z, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorGetIntWPtr(u32* w, vec_float_p1_t V);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetByIndex(vec_float_p1_t V, f32 f, usize i);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetX(vec_float_p1_t V, f32 x);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetY(vec_float_p1_t V, f32 y);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetZ(vec_float_p1_t V, f32 z);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetW(vec_float_p1_t V, f32 w);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetByIndexPtr(vec_float_p1_t V, const f32* f, usize i);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetXPtr(vec_float_p1_t V, const f32* x);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetYPtr(vec_float_p1_t V, const f32* y);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetZPtr(vec_float_p1_t V, const f32* z);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetWPtr(vec_float_p1_t V, const f32* w);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntByIndex(vec_float_p1_t V, u32 x, usize i);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntX(vec_float_p1_t V, u32 x);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntY(vec_float_p1_t V, u32 y);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntZ(vec_float_p1_t V, u32 z);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntW(vec_float_p1_t V, u32 w);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntByIndexPtr(vec_float_p1_t V, const u32* x, usize i);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntXPtr(vec_float_p1_t V, const u32* x);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntYPtr(vec_float_p1_t V, const u32* y);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntZPtr(vec_float_p1_t V, const u32* z);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSetIntWPtr(vec_float_p1_t V, const u32* w);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorSwizzle(vec_float_p1_t V, u32 E0, u32 E1, u32 E2, u32 E3);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorPermute(vec_float_p1_t V1, vec_float_p1_t V2, u32 PermuteX, u32 PermuteY, u32 PermuteZ, u32 PermuteW);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSelectControl(u32 VectorIndex0, u32 VectorIndex1, u32 VectorIndex2, u32 VectorIndex3);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSelect(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t Control);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorMergeXY(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorMergeZW(vec_float_p1_t V1, vec_float_p1_t V2);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorShiftLeft(vec_float_p1_t V1, vec_float_p1_t V2, u32 Elements);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorRotateLeft(vec_float_p1_t V, u32 Elements);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorRotateRight(vec_float_p1_t V, u32 Elements);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorInsert(vec_float_p1_t VD, vec_float_p1_t VS, u32 VSLeftRotateElements,
			u32 Select0, u32 Select1, u32 Select2, u32 Select3);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorEqualR(u32* pCR, vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorEqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorEqualIntR(u32* pCR, vec_float_p1_t V, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorNearEqual(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t Epsilon);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorNotEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorNotEqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorGreater(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorGreaterR(u32* pCR, vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorGreaterOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorGreaterOrEqualR(u32* pCR, vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLess(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLessOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorInBounds(vec_float_p1_t V, vec_float_p1_t Bounds);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorInBoundsR(u32* pCR, vec_float_p1_t V, vec_float_p1_t Bounds);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorIsNaN(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorIsInfinite(vec_float_p1_t V);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorMin(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorMax(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorRound(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorTruncate(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorFloor(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorCeiling(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorClamp(vec_float_p1_t V, vec_float_p1_t min, vec_float_p1_t max);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSaturate(vec_float_p1_t V);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorAndInt(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorAndCInt(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorOrInt(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorNorInt(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorXorInt(vec_float_p1_t V1, vec_float_p1_t V2);

		vec_float_t    LUNA_SIMD_CALLCONV     VectorNegate(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorAdd(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSum(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorAddAngles(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSubtract(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSubtractAngles(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorMultiply(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorMultiplyAdd(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t V3);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorDivide(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorNegativeMultiplySubtract(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t V3);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorScale(vec_float_p1_t V, f32 ScaleFactor);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorReciprocalEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorReciprocal(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSqrtEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSqrt(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorReciprocalSqrtEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorReciprocalSqrt(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorExp2(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorExpE(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorExp(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLog2(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLogE(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLog(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorPow(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorAbs(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorMod(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorModAngles(vec_float_p1_t Angles);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSin(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSinEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorCos(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorCosEst(vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorSinCos(vec_float_t* pSin, vec_float_t* pCos, vec_float_p1_t V);
		void        LUNA_SIMD_CALLCONV     VectorSinCosEst(vec_float_t* pSin, vec_float_t* pCos, vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorTan(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorTanEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorSinH(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorCosH(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorTanH(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorASin(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorASinEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorACos(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorACosEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorATan(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorATanEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorATan2(vec_float_p1_t Y, vec_float_p1_t X);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorATan2Est(vec_float_p1_t Y, vec_float_p1_t X);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLerp(vec_float_p1_t V0, vec_float_p1_t V1, f32 t);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorLerpV(vec_float_p1_t V0, vec_float_p1_t V1, vec_float_p1_t T);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorHermite(vec_float_p1_t Position0, vec_float_p1_t Tangent0, vec_float_p1_t Position1, vec_float_p2_t Tangent1, f32 t);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorHermiteV(vec_float_p1_t Position0, vec_float_p1_t Tangent0, vec_float_p1_t Position1, vec_float_p2_t Tangent1, vec_float_p3_t T);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorCatmullRom(vec_float_p1_t Position0, vec_float_p1_t Position1, vec_float_p1_t Position2, vec_float_p2_t Position3, f32 t);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorCatmullRomV(vec_float_p1_t Position0, vec_float_p1_t Position1, vec_float_p1_t Position2, vec_float_p2_t Position3, vec_float_p3_t T);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorBaryCentric(vec_float_p1_t Position0, vec_float_p1_t Position1, vec_float_p1_t Position2, f32 f, f32 g);
		vec_float_t    LUNA_SIMD_CALLCONV     VectorBaryCentricV(vec_float_p1_t Position0, vec_float_p1_t Position1, vec_float_p1_t Position2, vec_float_p2_t F, vec_float_p3_t G);

		/****************************************************************************
		 *
		 *	2D vector operations
		 *
		 ****************************************************************************/


		bool        LUNA_SIMD_CALLCONV     Vector2Equal(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector2EqualR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2EqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector2EqualIntR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2NearEqual(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t Epsilon);
		bool        LUNA_SIMD_CALLCONV     Vector2NotEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2NotEqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2Greater(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector2GreaterR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2GreaterOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector2GreaterOrEqualR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2Less(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2LessOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector2InBounds(vec_float_p1_t V, vec_float_p1_t Bounds);

		bool        LUNA_SIMD_CALLCONV     Vector2IsNaN(vec_float_p1_t V);
		bool        LUNA_SIMD_CALLCONV     Vector2IsInfinite(vec_float_p1_t V);

		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Dot(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Cross(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2LengthSq(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2ReciprocalLengthEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2ReciprocalLength(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2LengthEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Length(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2NormalizeEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Normalize(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2ClampLength(vec_float_p1_t V, f32 LengthMin, f32 LengthMax);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2ClampLengthV(vec_float_p1_t V, vec_float_p1_t LengthMin, vec_float_p1_t LengthMax);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Reflect(vec_float_p1_t Incident, vec_float_p1_t Normal);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Refract(vec_float_p1_t Incident, vec_float_p1_t Normal, f32 RefractionIndex);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2RefractV(vec_float_p1_t Incident, vec_float_p1_t Normal, vec_float_p1_t RefractionIndex);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Orthogonal(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2AngleBetweenNormalsEst(vec_float_p1_t N1, vec_float_p1_t N2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2AngleBetweenNormals(vec_float_p1_t N1, vec_float_p1_t N2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2AngleBetweenVectors(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2LinePointDistance(vec_float_p1_t LinePoint1, vec_float_p1_t LinePoint2, vec_float_p1_t Point);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2IntersectLine(vec_float_p1_t Line1Point1, vec_float_p1_t Line1Point2, vec_float_p1_t Line2Point1, vec_float_p2_t Line2Point2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2Transform(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector2TransformStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2TransformCoord(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector2TransformCoordStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector2TransformNormal(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector2TransformNormalStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);

		/****************************************************************************
		 *
		 *	3D vector operations
		 *
		 ****************************************************************************/

		bool        LUNA_SIMD_CALLCONV     Vector3Equal(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector3EqualR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3EqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector3EqualIntR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3NearEqual(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t Epsilon);
		bool        LUNA_SIMD_CALLCONV     Vector3NotEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3NotEqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3Greater(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector3GreaterR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3GreaterOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector3GreaterOrEqualR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3Less(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3LessOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector3InBounds(vec_float_p1_t V, vec_float_p1_t Bounds);

		bool        LUNA_SIMD_CALLCONV     Vector3IsNaN(vec_float_p1_t V);
		bool        LUNA_SIMD_CALLCONV     Vector3IsInfinite(vec_float_p1_t V);

		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Dot(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Cross(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3LengthSq(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3ReciprocalLengthEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3ReciprocalLength(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3LengthEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Length(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3NormalizeEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Normalize(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3ClampLength(vec_float_p1_t V, f32 LengthMin, f32 LengthMax);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3ClampLengthV(vec_float_p1_t V, vec_float_p1_t LengthMin, vec_float_p1_t LengthMax);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Reflect(vec_float_p1_t Incident, vec_float_p1_t Normal);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Refract(vec_float_p1_t Incident, vec_float_p1_t Normal, f32 RefractionIndex);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3RefractV(vec_float_p1_t Incident, vec_float_p1_t Normal, vec_float_p1_t RefractionIndex);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Orthogonal(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3AngleBetweenNormalsEst(vec_float_p1_t N1, vec_float_p1_t N2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3AngleBetweenNormals(vec_float_p1_t N1, vec_float_p1_t N2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3AngleBetweenVectors(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3LinePointDistance(vec_float_p1_t LinePoint1, vec_float_p1_t LinePoint2, vec_float_p1_t Point);
		void        LUNA_SIMD_CALLCONV     Vector3ComponentsFromNormal(vec_float_t* pParallel, vec_float_t* pPerpendicular, vec_float_p1_t V, vec_float_p1_t Normal);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Rotate(vec_float_p1_t V, vec_float_p1_t RotationQuaternion);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3InverseRotate(vec_float_p1_t V, vec_float_p1_t RotationQuaternion);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Transform(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector3TransformStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3TransformCoord(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector3TransformCoordStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3TransformNormal(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector3TransformNormalStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Project(vec_float_p1_t V, f32 ViewportX, f32 ViewportY, f32 ViewportWidth, f32 ViewportHeight, f32 ViewportMinZ, f32 ViewportMaxZ,
			mat_float_p1_t Projection, mat_float_p2_t View, mat_float_p2_t World);
		f32*   LUNA_SIMD_CALLCONV     Vector3ProjectStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount,
			f32 ViewportX, f32 ViewportY, f32 ViewportWidth, f32 ViewportHeight, f32 ViewportMinZ, f32 ViewportMaxZ,
			mat_float_p1_t Projection, mat_float_p2_t View, mat_float_p2_t World);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector3Unproject(vec_float_p1_t V, f32 ViewportX, f32 ViewportY, f32 ViewportWidth, f32 ViewportHeight, f32 ViewportMinZ, f32 ViewportMaxZ,
			mat_float_p1_t Projection, mat_float_p2_t View, mat_float_p2_t World);
		f32*   LUNA_SIMD_CALLCONV     Vector3UnprojectStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount,
			f32 ViewportX, f32 ViewportY, f32 ViewportWidth, f32 ViewportHeight, f32 ViewportMinZ, f32 ViewportMaxZ,
			mat_float_p1_t Projection, mat_float_p2_t View, mat_float_p2_t World);

		/****************************************************************************
		 *
		 *	4D vector operations
		 *
		 ****************************************************************************/

		bool        LUNA_SIMD_CALLCONV     Vector4Equal(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector4EqualR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4EqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector4EqualIntR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4NearEqual(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t Epsilon);
		bool        LUNA_SIMD_CALLCONV     Vector4NotEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4NotEqualInt(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4Greater(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector4GreaterR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4GreaterOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		u32    LUNA_SIMD_CALLCONV     Vector4GreaterOrEqualR(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4Less(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4LessOrEqual(vec_float_p1_t V1, vec_float_p1_t V2);
		bool        LUNA_SIMD_CALLCONV     Vector4InBounds(vec_float_p1_t V, vec_float_p1_t Bounds);

		bool        LUNA_SIMD_CALLCONV     Vector4IsNaN(vec_float_p1_t V);
		bool        LUNA_SIMD_CALLCONV     Vector4IsInfinite(vec_float_p1_t V);

		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Dot(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Cross(vec_float_p1_t V1, vec_float_p1_t V2, vec_float_p1_t V3);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4LengthSq(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4ReciprocalLengthEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4ReciprocalLength(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4LengthEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Length(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4NormalizeEst(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Normalize(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4ClampLength(vec_float_p1_t V, f32 LengthMin, f32 LengthMax);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4ClampLengthV(vec_float_p1_t V, vec_float_p1_t LengthMin, vec_float_p1_t LengthMax);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Reflect(vec_float_p1_t Incident, vec_float_p1_t Normal);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Refract(vec_float_p1_t Incident, vec_float_p1_t Normal, f32 RefractionIndex);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4RefractV(vec_float_p1_t Incident, vec_float_p1_t Normal, vec_float_p1_t RefractionIndex);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Orthogonal(vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4AngleBetweenNormalsEst(vec_float_p1_t N1, vec_float_p1_t N2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4AngleBetweenNormals(vec_float_p1_t N1, vec_float_p1_t N2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4AngleBetweenVectors(vec_float_p1_t V1, vec_float_p1_t V2);
		vec_float_t    LUNA_SIMD_CALLCONV     Vector4Transform(vec_float_p1_t V, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     Vector4TransformStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize VectorCount, mat_float_p1_t M);

		/****************************************************************************
		 *
		 *	Matrix operations
		 *
		 ****************************************************************************/

		bool        LUNA_SIMD_CALLCONV     MatrixIsNaN(mat_float_p1_t M);
		bool        LUNA_SIMD_CALLCONV     MatrixIsInfinite(mat_float_p1_t M);
		bool        LUNA_SIMD_CALLCONV     MatrixIsIdentity(mat_float_p1_t M);

		mat_float_t    LUNA_SIMD_CALLCONV     MatrixMultiply(mat_float_p1_t M1, mat_float_p2_t M2);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixMultiplyTranspose(mat_float_p1_t M1, mat_float_p2_t M2);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixTranspose(mat_float_p1_t M);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixInverse(_Out_opt_ vec_float_t* pDeterminant, mat_float_p1_t M);
		vec_float_t    LUNA_SIMD_CALLCONV     MatrixDeterminant(mat_float_p1_t M);
		_Success_(return)
			bool        LUNA_SIMD_CALLCONV     MatrixDecompose(vec_float_t *outScale, vec_float_t *outRotQuat, vec_float_t *outTrans, mat_float_p1_t M);

		mat_float_t    LUNA_SIMD_CALLCONV     MatrixIdentity();
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixSet(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixTranslation(f32 OffsetX, f32 OffsetY, f32 OffsetZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixTranslationFromVector(vec_float_p1_t Offset);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixScaling(f32 ScaleX, f32 ScaleY, f32 ScaleZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixScalingFromVector(vec_float_p1_t Scale);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationX(f32 Angle);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationY(f32 Angle);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationZ(f32 Angle);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationRollPitchYaw(f32 Pitch, f32 Yaw, f32 Roll);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationRollPitchYawFromVector(vec_float_p1_t Angles);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationNormal(vec_float_p1_t NormalAxis, f32 Angle);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationAxis(vec_float_p1_t Axis, f32 Angle);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixRotationQuaternion(vec_float_p1_t Quaternion);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixTransformation2D(vec_float_p1_t ScalingOrigin, f32 ScalingOrientation, vec_float_p1_t Scaling,
			vec_float_p1_t RotationOrigin, f32 Rotation, vec_float_p2_t Translation);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixTransformation(vec_float_p1_t ScalingOrigin, vec_float_p1_t ScalingOrientationQuaternion, vec_float_p1_t Scaling,
			vec_float_p2_t RotationOrigin, vec_float_p3_t RotationQuaternion, vec_float_p3_t Translation);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixAffineTransformation2D(vec_float_p1_t Scaling, vec_float_p1_t RotationOrigin, f32 Rotation, vec_float_p1_t Translation);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixAffineTransformation(vec_float_p1_t Scaling, vec_float_p1_t RotationOrigin, vec_float_p1_t RotationQuaternion, vec_float_p2_t Translation);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixReflect(vec_float_p1_t ReflectionPlane);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixShadow(vec_float_p1_t ShadowPlane, vec_float_p1_t LightPosition);

		mat_float_t    LUNA_SIMD_CALLCONV     MatrixLookAtLH(vec_float_p1_t EyePosition, vec_float_p1_t FocusPosition, vec_float_p1_t UpDirection);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixLookAtRH(vec_float_p1_t EyePosition, vec_float_p1_t FocusPosition, vec_float_p1_t UpDirection);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixLookToLH(vec_float_p1_t EyePosition, vec_float_p1_t EyeDirection, vec_float_p1_t UpDirection);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixLookToRH(vec_float_p1_t EyePosition, vec_float_p1_t EyeDirection, vec_float_p1_t UpDirection);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixPerspectiveLH(f32 ViewWidth, f32 ViewHeight, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixPerspectiveRH(f32 ViewWidth, f32 ViewHeight, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixPerspectiveFovLH(f32 FovAngleY, f32 AspectRatio, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixPerspectiveFovRH(f32 FovAngleY, f32 AspectRatio, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixPerspectiveOffCenterLH(f32 ViewLeft, f32 ViewRight, f32 ViewBottom, f32 ViewTop, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixPerspectiveOffCenterRH(f32 ViewLeft, f32 ViewRight, f32 ViewBottom, f32 ViewTop, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixOrthographicLH(f32 ViewWidth, f32 ViewHeight, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixOrthographicRH(f32 ViewWidth, f32 ViewHeight, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixOrthographicOffCenterLH(f32 ViewLeft, f32 ViewRight, f32 ViewBottom, f32 ViewTop, f32 NearZ, f32 FarZ);
		mat_float_t    LUNA_SIMD_CALLCONV     MatrixOrthographicOffCenterRH(f32 ViewLeft, f32 ViewRight, f32 ViewBottom, f32 ViewTop, f32 NearZ, f32 FarZ);


		/****************************************************************************
		 *
		 *	Quaternion operations
		 *
		 ****************************************************************************/

		bool        LUNA_SIMD_CALLCONV     QuaternionEqual(vec_float_p1_t Q1, vec_float_p1_t Q2);
		bool        LUNA_SIMD_CALLCONV     QuaternionNotEqual(vec_float_p1_t Q1, vec_float_p1_t Q2);

		bool        LUNA_SIMD_CALLCONV     QuaternionIsNaN(vec_float_p1_t Q);
		bool        LUNA_SIMD_CALLCONV     QuaternionIsInfinite(vec_float_p1_t Q);
		bool        LUNA_SIMD_CALLCONV     QuaternionIsIdentity(vec_float_p1_t Q);

		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionDot(vec_float_p1_t Q1, vec_float_p1_t Q2);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionMultiply(vec_float_p1_t Q1, vec_float_p1_t Q2);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionLengthSq(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionReciprocalLength(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionLength(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionNormalizeEst(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionNormalize(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionConjugate(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionInverse(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionLn(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionExp(vec_float_p1_t Q);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionSlerp(vec_float_p1_t Q0, vec_float_p1_t Q1, f32 t);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionSlerpV(vec_float_p1_t Q0, vec_float_p1_t Q1, vec_float_p1_t T);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionSquad(vec_float_p1_t Q0, vec_float_p1_t Q1, vec_float_p1_t Q2, vec_float_p2_t Q3, f32 t);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionSquadV(vec_float_p1_t Q0, vec_float_p1_t Q1, vec_float_p1_t Q2, vec_float_p2_t Q3, vec_float_p3_t T);
		void        LUNA_SIMD_CALLCONV     QuaternionSquadSetup(vec_float_t* pA, vec_float_t* pB, vec_float_t* pC, vec_float_p1_t Q0, vec_float_p1_t Q1, vec_float_p1_t Q2, vec_float_p2_t Q3);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionBaryCentric(vec_float_p1_t Q0, vec_float_p1_t Q1, vec_float_p1_t Q2, f32 f, f32 g);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionBaryCentricV(vec_float_p1_t Q0, vec_float_p1_t Q1, vec_float_p1_t Q2, vec_float_p2_t F, vec_float_p3_t G);

		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionIdentity();
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionRotationRollPitchYaw(f32 Pitch, f32 Yaw, f32 Roll);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionRotationRollPitchYawFromVector(vec_float_p1_t Angles);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionRotationNormal(vec_float_p1_t NormalAxis, f32 Angle);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionRotationAxis(vec_float_p1_t Axis, f32 Angle);
		vec_float_t    LUNA_SIMD_CALLCONV     QuaternionRotationMatrix(mat_float_p1_t M);

		void        LUNA_SIMD_CALLCONV     QuaternionToAxisAngle(vec_float_t* pAxis, f32* pAngle, vec_float_p1_t Q);

		/****************************************************************************
		 *
		 *	Plane operations
		 *
		 ****************************************************************************/

		bool        LUNA_SIMD_CALLCONV     PlaneEqual(vec_float_p1_t P1, vec_float_p1_t P2);
		bool        LUNA_SIMD_CALLCONV     PlaneNearEqual(vec_float_p1_t P1, vec_float_p1_t P2, vec_float_p1_t Epsilon);
		bool        LUNA_SIMD_CALLCONV     PlaneNotEqual(vec_float_p1_t P1, vec_float_p1_t P2);

		bool        LUNA_SIMD_CALLCONV     PlaneIsNaN(vec_float_p1_t P);
		bool        LUNA_SIMD_CALLCONV     PlaneIsInfinite(vec_float_p1_t P);

		vec_float_t    LUNA_SIMD_CALLCONV     PlaneDot(vec_float_p1_t P, vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneDotCoord(vec_float_p1_t P, vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneDotNormal(vec_float_p1_t P, vec_float_p1_t V);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneNormalizeEst(vec_float_p1_t P);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneNormalize(vec_float_p1_t P);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneIntersectLine(vec_float_p1_t P, vec_float_p1_t LinePoint1, vec_float_p1_t LinePoint2);
		void        LUNA_SIMD_CALLCONV     PlaneIntersectPlane(vec_float_t* pLinePoint1, vec_float_t* pLinePoint2, vec_float_p1_t P1, vec_float_p1_t P2);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneTransform(vec_float_p1_t P, mat_float_p1_t M);
		f32*   LUNA_SIMD_CALLCONV     PlaneTransformStream(f32* pOutputStream,
			usize OutputStride,
			const f32* pInputStream,
			usize InputStride, usize PlaneCount, mat_float_p1_t M);

		vec_float_t    LUNA_SIMD_CALLCONV     PlaneFromPointNormal(vec_float_p1_t Point, vec_float_p1_t Normal);
		vec_float_t    LUNA_SIMD_CALLCONV     PlaneFromPoints(vec_float_p1_t Point1, vec_float_p1_t Point2, vec_float_p1_t Point3);

		/****************************************************************************
		 *
		 *	Color operations
		 *
		 ****************************************************************************/

		bool        LUNA_SIMD_CALLCONV     ColorEqual(vec_float_p1_t C1, vec_float_p1_t C2);
		bool        LUNA_SIMD_CALLCONV     ColorNotEqual(vec_float_p1_t C1, vec_float_p1_t C2);
		bool        LUNA_SIMD_CALLCONV     ColorGreater(vec_float_p1_t C1, vec_float_p1_t C2);
		bool        LUNA_SIMD_CALLCONV     ColorGreaterOrEqual(vec_float_p1_t C1, vec_float_p1_t C2);
		bool        LUNA_SIMD_CALLCONV     ColorLess(vec_float_p1_t C1, vec_float_p1_t C2);
		bool        LUNA_SIMD_CALLCONV     ColorLessOrEqual(vec_float_p1_t C1, vec_float_p1_t C2);

		bool        LUNA_SIMD_CALLCONV     ColorIsNaN(vec_float_p1_t C);
		bool        LUNA_SIMD_CALLCONV     ColorIsInfinite(vec_float_p1_t C);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorNegative(vec_float_p1_t C);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorModulate(vec_float_p1_t C1, vec_float_p1_t C2);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorAdjustSaturation(vec_float_p1_t C, f32 Saturation);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorAdjustContrast(vec_float_p1_t C, f32 Contrast);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorRGBToHSL(vec_float_p1_t rgb);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorHSLToRGB(vec_float_p1_t hsl);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorRGBToHSV(vec_float_p1_t rgb);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorHSVToRGB(vec_float_p1_t hsv);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorRGBToYUV(vec_float_p1_t rgb);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorYUVToRGB(vec_float_p1_t yuv);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorRGBToYUV_HD(vec_float_p1_t rgb);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorYUVToRGB_HD(vec_float_p1_t yuv);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorRGBToXYZ(vec_float_p1_t rgb);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorXYZToRGB(vec_float_p1_t xyz);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorXYZToSRGB(vec_float_p1_t xyz);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorSRGBToXYZ(vec_float_p1_t srgb);

		vec_float_t    LUNA_SIMD_CALLCONV     ColorRGBToSRGB(vec_float_p1_t rgb);
		vec_float_t    LUNA_SIMD_CALLCONV     ColorSRGBToRGB(vec_float_p1_t srgb);


		/****************************************************************************
		 *
		 *	Miscellaneous operations
		 *
		 ****************************************************************************/

		bool            VerifyCPUSupport();

		vec_float_t    LUNA_SIMD_CALLCONV     FresnelTerm(vec_float_p1_t CosIncidentAngle, vec_float_p1_t RefractionIndex);

		bool            ScalarNearEqual(f32 S1, f32 S2, f32 Epsilon);
		f32           ScalarModAngle(f32 Value);

		f32           ScalarSin(f32 Value);
		f32           ScalarSinEst(f32 Value);

		f32           ScalarCos(f32 Value);
		f32           ScalarCosEst(f32 Value);

		void            ScalarSinCos(f32* pSin, f32* pCos, f32 Value);
		void            ScalarSinCosEst(f32* pSin, f32* pCos, f32 Value);

		f32           ScalarASin(f32 Value);
		f32           ScalarASinEst(f32 Value);

		f32           ScalarACos(f32 Value);
		f32           ScalarACosEst(f32 Value);

		//------------------------------------------------------------------------------

#if defined(LUNA_PLATFORM_SSE2)

// PermuteHelper internal template (SSE only)
		namespace Internal
		{
			// Slow path fall back for permutes that do not map to a single SSE shuffle opcode.
			template<u32 Shuffle, bool WhichX, bool WhichY, bool WhichZ, bool WhichW> struct PermuteHelper
			{
				static vec_float_t     LUNA_SIMD_CALLCONV     Permute(vec_float_p1_t v1, vec_float_p1_t v2)
				{
					static alignas(16) const u32 selectMask[4] =
					{
							WhichX ? 0xFFFFFFFF : 0,
							WhichY ? 0xFFFFFFFF : 0,
							WhichZ ? 0xFFFFFFFF : 0,
							WhichW ? 0xFFFFFFFF : 0,
					};

					vec_int32_t vec_select_i = _mm_load_si128((vec_int32_t*)selectMask);
					vec_float_t vec_select = _mm_castsi128_ps(vec_select_i);

					vec_float_t shuffled1 = LUNA_SIMD_PERMUTE_PS(v1, Shuffle);
					vec_float_t shuffled2 = LUNA_SIMD_PERMUTE_PS(v2, Shuffle);

					vec_float_t masked1 = _mm_andnot_ps(vec_select, shuffled1);
					vec_float_t masked2 = _mm_and_ps(vec_select, shuffled2);

					return _mm_or_ps(masked1, masked2);
				}
			};

			// Fast path for permutes that only read from the first vector.
			template<u32 Shuffle> struct PermuteHelper<Shuffle, false, false, false, false>
			{
				static vec_float_t     LUNA_SIMD_CALLCONV     Permute(vec_float_p1_t v1, vec_float_p1_t) { return LUNA_SIMD_PERMUTE_PS(v1, Shuffle); }
			};

			// Fast path for permutes that only read from the second vector.
			template<u32 Shuffle> struct PermuteHelper<Shuffle, true, true, true, true>
			{
				static vec_float_t     LUNA_SIMD_CALLCONV     Permute(vec_float_p1_t, vec_float_p1_t v2) { return LUNA_SIMD_PERMUTE_PS(v2, Shuffle); }
			};

			// Fast path for permutes that read XY from the first vector, ZW from the second.
			template<u32 Shuffle> struct PermuteHelper<Shuffle, false, false, true, true>
			{
				static vec_float_t     LUNA_SIMD_CALLCONV     Permute(vec_float_p1_t v1, vec_float_p1_t v2) { return _mm_shuffle_ps(v1, v2, Shuffle); }
			};

			// Fast path for permutes that read XY from the second vector, ZW from the first.
			template<u32 Shuffle> struct PermuteHelper<Shuffle, true, true, false, false>
			{
				static vec_float_t     LUNA_SIMD_CALLCONV     Permute(vec_float_p1_t v1, vec_float_p1_t v2) { return _mm_shuffle_ps(v2, v1, Shuffle); }
			};
		}

#endif // LUNA_PLATFORM_SSE2

		// General permute template
		template<uint32_t PermuteX, uint32_t PermuteY, uint32_t PermuteZ, uint32_t PermuteW>
		inline vec_float_t     LUNA_SIMD_CALLCONV     VectorPermute(vec_float_p1_t V1, vec_float_p1_t V2)
		{
			static_assert(PermuteX <= 7, "PermuteX template parameter out of range");
			static_assert(PermuteY <= 7, "PermuteY template parameter out of range");
			static_assert(PermuteZ <= 7, "PermuteZ template parameter out of range");
			static_assert(PermuteW <= 7, "PermuteW template parameter out of range");

#if defined(LUNA_PLATFORM_SSE2)
			const uint32_t Shuffle = _MM_SHUFFLE(PermuteW & 3, PermuteZ & 3, PermuteY & 3, PermuteX & 3);

			const bool WhichX = PermuteX > 3;
			const bool WhichY = PermuteY > 3;
			const bool WhichZ = PermuteZ > 3;
			const bool WhichW = PermuteW > 3;

			return Internal::PermuteHelper<Shuffle, WhichX, WhichY, WhichZ, WhichW>::Permute(V1, V2);
#else

			return VectorPermute(V1, V2, PermuteX, PermuteY, PermuteZ, PermuteW);

#endif
		}

		// Special-case permute templates
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 2, 3>(vec_float_p1_t V1, vec_float_p1_t) { return V1; }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<4, 5, 6, 7>(vec_float_p1_t, vec_float_p1_t V2) { return V2; }

#if defined(LUNA_PLATFORM_SSE2)
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 4, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_movelh_ps(V1, V2); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<6, 7, 2, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_movehl_ps(V1, V2); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<0, 4, 1, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_unpacklo_ps(V1, V2); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<2, 6, 3, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_unpackhi_ps(V1, V2); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorPermute<2, 3, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_castpd_ps(_mm_unpackhi_pd(_mm_castps_pd(V1), _mm_castps_pd(V2))); }
#endif

#if defined(LUNA_PLATFORM_SSE4)
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 1, 2, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x1); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 5, 2, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x2); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 5, 2, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x3); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 6, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x4); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 1, 6, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x5); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 5, 6, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x6); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 5, 6, 3>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x7); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 2, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x8); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 1, 2, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0x9); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 5, 2, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0xA); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 5, 2, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0xB); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0xC); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<4, 1, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0xD); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 5, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return _mm_blend_ps(V1, V2, 0xE); }
#endif

#if defined(LUNA_PLATFORM_ARM_NEON)

		// If the indices are all in the range 0-3 or 4-7, then use VectorSwizzle instead
		// The mirror cases are not spelled out here as the programmer can always swap the arguments
		// (i.e. prefer permutes where the X element comes from the V1 vector instead of the V2 vector)

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 4, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_low_f32(V1), vget_low_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 0, 4, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_low_f32(V1)), vget_low_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 5, 4>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_low_f32(V1), vrev64_f32(vget_low_f32(V2))); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 0, 5, 4>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_low_f32(V1)), vrev64_f32(vget_low_f32(V2))); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<2, 3, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_high_f32(V1), vget_high_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<3, 2, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_high_f32(V1)), vget_high_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<2, 3, 7, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_high_f32(V1), vrev64_f32(vget_high_f32(V2))); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<3, 2, 7, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_high_f32(V1)), vrev64_f32(vget_high_f32(V2))); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_low_f32(V1), vget_high_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 0, 6, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_low_f32(V1)), vget_high_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 1, 7, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_low_f32(V1), vrev64_f32(vget_high_f32(V2))); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 0, 7, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_low_f32(V1)), vrev64_f32(vget_high_f32(V2))); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<3, 2, 4, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_high_f32(V1)), vget_low_f32(V2)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<2, 3, 5, 4>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vget_high_f32(V1), vrev64_f32(vget_low_f32(V2))); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<3, 2, 5, 4>(vec_float_p1_t V1, vec_float_p1_t V2) { return vcombine_f32(vrev64_f32(vget_high_f32(V1)), vrev64_f32(vget_low_f32(V2))); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 4, 2, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vtrnq_f32(V1, V2).val[0]; }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 5, 3, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vtrnq_f32(V1, V2).val[1]; }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 4, 1, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return vzipq_f32(V1, V2).val[0]; }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<2, 6, 3, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vzipq_f32(V1, V2).val[1]; }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<0, 2, 4, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vuzpq_f32(V1, V2).val[0]; }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 3, 5, 7>(vec_float_p1_t V1, vec_float_p1_t V2) { return vuzpq_f32(V1, V2).val[1]; }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<1, 2, 3, 4>(vec_float_p1_t V1, vec_float_p1_t V2) { return vextq_f32(V1, V2, 1); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<2, 3, 4, 5>(vec_float_p1_t V1, vec_float_p1_t V2) { return vextq_f32(V1, V2, 2); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorPermute<3, 4, 5, 6>(vec_float_p1_t V1, vec_float_p1_t V2) { return vextq_f32(V1, V2, 3); }

#endif // LUNA_PLATFORM_ARM_NEON

		//------------------------------------------------------------------------------

		// General swizzle template
		template<uint32_t SwizzleX, uint32_t SwizzleY, uint32_t SwizzleZ, uint32_t SwizzleW>
		inline vec_float_t     LUNA_SIMD_CALLCONV     VectorSwizzle(vec_float_p1_t V)
		{
			static_assert(SwizzleX <= 3, "SwizzleX template parameter out of range");
			static_assert(SwizzleY <= 3, "SwizzleY template parameter out of range");
			static_assert(SwizzleZ <= 3, "SwizzleZ template parameter out of range");
			static_assert(SwizzleW <= 3, "SwizzleW template parameter out of range");

#if defined(LUNA_PLATFORM_SSE2)
			return LUNA_SIMD_PERMUTE_PS(V, _MM_SHUFFLE(SwizzleW, SwizzleZ, SwizzleY, SwizzleX));
#else

			return VectorSwizzle(V, SwizzleX, SwizzleY, SwizzleZ, SwizzleW);

#endif
		}

		// Specialized swizzles
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 1, 2, 3>(vec_float_p1_t V) { return V; }

#if defined(LUNA_PLATFORM_SSE2)
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 1, 0, 1>(vec_float_p1_t V) { return _mm_movelh_ps(V, V); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 3, 2, 3>(vec_float_p1_t V) { return _mm_movehl_ps(V, V); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 0, 1, 1>(vec_float_p1_t V) { return _mm_unpacklo_ps(V, V); }
		template<> inline vec_float_t      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 2, 3, 3>(vec_float_p1_t V) { return _mm_unpackhi_ps(V, V); }
#endif

#if defined(LUNA_PLATFORM_SSE3)
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 0, 2, 2>(vec_float_p1_t V) { return _mm_moveldup_ps(V); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 1, 3, 3>(vec_float_p1_t V) { return _mm_movehdup_ps(V); }
#endif

#if defined(LUNA_PLATFORM_AVX2)
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 0, 0, 0>(vec_float_p1_t V) { return _mm_broadcastss_ps(V); }
#endif

#if defined(LUNA_PLATFORM_ARM_NEON)

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 0, 0, 0>(vec_float_p1_t V) { return vdupq_lane_f32(vget_low_f32(V), 0); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 1, 1, 1>(vec_float_p1_t V) { return vdupq_lane_f32(vget_low_f32(V), 1); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 2, 2, 2>(vec_float_p1_t V) { return vdupq_lane_f32(vget_high_f32(V), 0); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<3, 3, 3, 3>(vec_float_p1_t V) { return vdupq_lane_f32(vget_high_f32(V), 1); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 0, 3, 2>(vec_float_p1_t V) { return vrev64q_f32(V); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 1, 0, 1>(vec_float_p1_t V) { float32x2_t vt = vget_low_f32(V); return vcombine_f32(vt, vt); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 3, 2, 3>(vec_float_p1_t V) { float32x2_t vt = vget_high_f32(V); return vcombine_f32(vt, vt); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 0, 1, 0>(vec_float_p1_t V) { float32x2_t vt = vrev64_f32(vget_low_f32(V)); return vcombine_f32(vt, vt); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<3, 2, 3, 2>(vec_float_p1_t V) { float32x2_t vt = vrev64_f32(vget_high_f32(V)); return vcombine_f32(vt, vt); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 1, 3, 2>(vec_float_p1_t V) { return vcombine_f32(vget_low_f32(V), vrev64_f32(vget_high_f32(V))); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 0, 2, 3>(vec_float_p1_t V) { return vcombine_f32(vrev64_f32(vget_low_f32(V)), vget_high_f32(V)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 3, 1, 0>(vec_float_p1_t V) { return vcombine_f32(vget_high_f32(V), vrev64_f32(vget_low_f32(V))); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<3, 2, 0, 1>(vec_float_p1_t V) { return vcombine_f32(vrev64_f32(vget_high_f32(V)), vget_low_f32(V)); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<3, 2, 1, 0>(vec_float_p1_t V) { return vcombine_f32(vrev64_f32(vget_high_f32(V)), vrev64_f32(vget_low_f32(V))); }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 0, 2, 2>(vec_float_p1_t V) { return vtrnq_f32(V, V).val[0]; }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 1, 3, 3>(vec_float_p1_t V) { return vtrnq_f32(V, V).val[1]; }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 0, 1, 1>(vec_float_p1_t V) { return vzipq_f32(V, V).val[0]; }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 2, 3, 3>(vec_float_p1_t V) { return vzipq_f32(V, V).val[1]; }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<0, 2, 0, 2>(vec_float_p1_t V) { return vuzpq_f32(V, V).val[0]; }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 3, 1, 3>(vec_float_p1_t V) { return vuzpq_f32(V, V).val[1]; }

		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<1, 2, 3, 0>(vec_float_p1_t V) { return vextq_f32(V, V, 1); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<2, 3, 0, 1>(vec_float_p1_t V) { return vextq_f32(V, V, 2); }
		template<> inline vec_float      LUNA_SIMD_CALLCONV     VectorSwizzle<3, 0, 1, 2>(vec_float_p1_t V) { return vextq_f32(V, V, 3); }

#endif // LUNA_PLATFORM_ARM_NEON

		//------------------------------------------------------------------------------

		template<uint32_t Elements>
		inline vec_float_t     LUNA_SIMD_CALLCONV     VectorShiftLeft(vec_float_p1_t V1, vec_float_p1_t V2)
		{
			static_assert(Elements < 4, "Elements template parameter out of range");
			return VectorPermute<Elements, (Elements + 1), (Elements + 2), (Elements + 3)>(V1, V2);
		}

		template<uint32_t Elements>
		inline vec_float_t     LUNA_SIMD_CALLCONV     VectorRotateLeft(vec_float_p1_t V)
		{
			static_assert(Elements < 4, "Elements template parameter out of range");
			return VectorSwizzle<Elements & 3, (Elements + 1) & 3, (Elements + 2) & 3, (Elements + 3) & 3>(V);
		}

		template<uint32_t Elements>
		inline vec_float_t     LUNA_SIMD_CALLCONV     VectorRotateRight(vec_float_p1_t V)
		{
			static_assert(Elements < 4, "Elements template parameter out of range");
			return VectorSwizzle<(4 - Elements) & 3, (5 - Elements) & 3, (6 - Elements) & 3, (7 - Elements) & 3>(V);
		}

		template<uint32_t VSLeftRotateElements, uint32_t Select0, uint32_t Select1, uint32_t Select2, uint32_t Select3>
		inline vec_float_t     LUNA_SIMD_CALLCONV     VectorInsert(vec_float_p1_t VD, vec_float_p1_t VS)
		{
			vec_float_t Control = VectorSelectControl(Select0 & 1, Select1 & 1, Select2 & 1, Select3 & 1);
			return VectorSelect(VD, VectorRotateLeft<VSLeftRotateElements>(VS), Control);
		}
		/****************************************************************************
		*
		* Globals
		*
		****************************************************************************/

		constexpr vec_f32_t g_SinCoefficients0 = { { { -0.16666667f, +0.0083333310f, -0.00019840874f, +2.7525562e-06f } } };
		constexpr vec_f32_t g_SinCoefficients1 = { { { -2.3889859e-08f, -0.16665852f /*Est1*/, +0.0083139502f /*Est2*/, -0.00018524670f /*Est3*/ } } };
		constexpr vec_f32_t g_CosCoefficients0 = { { { -0.5f, +0.041666638f, -0.0013888378f, +2.4760495e-05f } } };
		constexpr vec_f32_t g_CosCoefficients1 = { { { -2.6051615e-07f, -0.49992746f /*Est1*/, +0.041493919f /*Est2*/, -0.0012712436f /*Est3*/ } } };
		constexpr vec_f32_t g_TanCoefficients0 = { { { 1.0f, 0.333333333f, 0.133333333f, 5.396825397e-2f } } };
		constexpr vec_f32_t g_TanCoefficients1 = { { { 2.186948854e-2f, 8.863235530e-3f, 3.592128167e-3f, 1.455834485e-3f } } };
		constexpr vec_f32_t g_TanCoefficients2 = { { { 5.900274264e-4f, 2.391290764e-4f, 9.691537707e-5f, 3.927832950e-5f } } };
		constexpr vec_f32_t g_ArcCoefficients0 = { { { +1.5707963050f, -0.2145988016f, +0.0889789874f, -0.0501743046f } } };
		constexpr vec_f32_t g_ArcCoefficients1 = { { { +0.0308918810f, -0.0170881256f, +0.0066700901f, -0.0012624911f } } };
		constexpr vec_f32_t g_ATanCoefficients0 = { { { -0.3333314528f, +0.1999355085f, -0.1420889944f, +0.1065626393f } } };
		constexpr vec_f32_t g_ATanCoefficients1 = { { { -0.0752896400f, +0.0429096138f, -0.0161657367f, +0.0028662257f } } };
		constexpr vec_f32_t g_ATanEstCoefficients0 = { { { +0.999866f, +0.999866f, +0.999866f, +0.999866f } } };
		constexpr vec_f32_t g_ATanEstCoefficients1 = { { { -0.3302995f, +0.180141f, -0.085133f, +0.0208351f } } };
		constexpr vec_f32_t g_TanEstCoefficients = { { { 2.484f, -1.954923183e-1f, 2.467401101f, one_div_pi } } };
		constexpr vec_f32_t g_ArcEstCoefficients = { { { +1.5707288f, -0.2121144f, +0.0742610f, -0.0187293f } } };
		constexpr vec_f32_t g_PiConstants0 = { { { pi, two_pi, one_div_pi, one_div_two_pi } } };
		constexpr vec_f32_t g_IdentityR0 = { { { 1.0f, 0.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_IdentityR1 = { { { 0.0f, 1.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_IdentityR2 = { { { 0.0f, 0.0f, 1.0f, 0.0f } } };
		constexpr vec_f32_t g_IdentityR3 = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		constexpr vec_f32_t g_NegIdentityR0 = { { { -1.0f, 0.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_NegIdentityR1 = { { { 0.0f, -1.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_NegIdentityR2 = { { { 0.0f, 0.0f, -1.0f, 0.0f } } };
		constexpr vec_f32_t g_NegIdentityR3 = { { { 0.0f, 0.0f, 0.0f, -1.0f } } };
		constexpr vec_u32_t g_NegativeZero = { { { 0x80000000, 0x80000000, 0x80000000, 0x80000000 } } };
		constexpr vec_u32_t g_Negate3 = { { { 0x80000000, 0x80000000, 0x80000000, 0x00000000 } } };
		constexpr vec_u32_t g_MaskXY = { { { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 } } };
		constexpr vec_u32_t g_Mask3 = { { { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 } } };
		constexpr vec_u32_t g_MaskX = { { { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000 } } };
		constexpr vec_u32_t g_MaskY = { { { 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000 } } };
		constexpr vec_u32_t g_MaskZ = { { { 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000 } } };
		constexpr vec_u32_t g_MaskW = { { { 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF } } };
		constexpr vec_f32_t g_One = { { { 1.0f, 1.0f, 1.0f, 1.0f } } };
		constexpr vec_f32_t g_One3 = { { { 1.0f, 1.0f, 1.0f, 0.0f } } };
		constexpr vec_f32_t g_Zero = { { { 0.0f, 0.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_Two = { { { 2.f, 2.f, 2.f, 2.f } } };
		constexpr vec_f32_t g_Four = { { { 4.f, 4.f, 4.f, 4.f } } };
		constexpr vec_f32_t g_Six = { { { 6.f, 6.f, 6.f, 6.f } } };
		constexpr vec_f32_t g_NegativeOne = { { { -1.0f, -1.0f, -1.0f, -1.0f } } };
		constexpr vec_f32_t g_OneHalf = { { { 0.5f, 0.5f, 0.5f, 0.5f } } };
		constexpr vec_f32_t g_NegativeOneHalf = { { { -0.5f, -0.5f, -0.5f, -0.5f } } };
		constexpr vec_f32_t g_NegativeTwoPi = { { { -two_pi, -two_pi, -two_pi, -two_pi } } };
		constexpr vec_f32_t g_NegativePi = { { { -pi, -pi, -pi, -pi } } };
		constexpr vec_f32_t g_HalfPi = { { { pi_div_two, pi_div_two, pi_div_two, pi_div_two } } };
		constexpr vec_f32_t g_Pi = { { { pi, pi, pi, pi } } };
		constexpr vec_f32_t g_ReciprocalPi = { { { one_div_pi, one_div_pi, one_div_pi, one_div_pi } } };
		constexpr vec_f32_t g_TwoPi = { { { two_pi, two_pi, two_pi, two_pi } } };
		constexpr vec_f32_t g_ReciprocalTwoPi = { { { one_div_two_pi, one_div_two_pi, one_div_two_pi, one_div_two_pi } } };
		constexpr vec_f32_t g_Epsilon = { { { 1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f } } };
		constexpr vec_i32_t g_Infinity = { { { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 } } };
		constexpr vec_i32_t g_QNaN = { { { 0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 } } };
		constexpr vec_i32_t g_QNaNTest = { { { 0x007FFFFF, 0x007FFFFF, 0x007FFFFF, 0x007FFFFF } } };
		constexpr vec_i32_t g_AbsMask = { { { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF } } };
		constexpr vec_i32_t g_FltMin = { { { 0x00800000, 0x00800000, 0x00800000, 0x00800000 } } };
		constexpr vec_i32_t g_FltMax = { { { 0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF } } };
		constexpr vec_u32_t g_NegOneMask = { { { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF } } };
		constexpr vec_u32_t g_MaskA8R8G8B8 = { { { 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 } } };
		constexpr vec_u32_t g_FlipA8R8G8B8 = { { { 0x00000000, 0x00000000, 0x00000000, 0x80000000 } } };
		constexpr vec_f32_t g_FixAA8R8G8B8 = { { { 0.0f, 0.0f, 0.0f, float(0x80000000U) } } };
		constexpr vec_f32_t g_NormalizeA8R8G8B8 = { { { 1.0f / (255.0f*float(0x10000)), 1.0f / (255.0f*float(0x100)), 1.0f / 255.0f, 1.0f / (255.0f*float(0x1000000)) } } };
		constexpr vec_u32_t g_MaskA2B10G10R10 = { { { 0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000 } } };
		constexpr vec_u32_t g_FlipA2B10G10R10 = { { { 0x00000200, 0x00080000, 0x20000000, 0x80000000 } } };
		constexpr vec_f32_t g_FixAA2B10G10R10 = { { { -512.0f, -512.0f*float(0x400), -512.0f*float(0x100000), float(0x80000000U) } } };
		constexpr vec_f32_t g_NormalizeA2B10G10R10 = { { { 1.0f / 511.0f, 1.0f / (511.0f*float(0x400)), 1.0f / (511.0f*float(0x100000)), 1.0f / (3.0f*float(0x40000000)) } } };
		constexpr vec_u32_t g_MaskX16Y16 = { { { 0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000 } } };
		constexpr vec_i32_t g_FlipX16Y16 = { { { 0x00008000, 0x00000000, 0x00000000, 0x00000000 } } };
		constexpr vec_f32_t g_FixX16Y16 = { { { -32768.0f, 0.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_NormalizeX16Y16 = { { { 1.0f / 32767.0f, 1.0f / (32767.0f*65536.0f), 0.0f, 0.0f } } };
		constexpr vec_u32_t g_MaskX16Y16Z16W16 = { { { 0x0000FFFF, 0x0000FFFF, 0xFFFF0000, 0xFFFF0000 } } };
		constexpr vec_i32_t g_FlipX16Y16Z16W16 = { { { 0x00008000, 0x00008000, 0x00000000, 0x00000000 } } };
		constexpr vec_f32_t g_FixX16Y16Z16W16 = { { { -32768.0f, -32768.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_NormalizeX16Y16Z16W16 = { { { 1.0f / 32767.0f, 1.0f / 32767.0f, 1.0f / (32767.0f*65536.0f), 1.0f / (32767.0f*65536.0f) } } };
		constexpr vec_f32_t g_NoFraction = { { { 8388608.0f, 8388608.0f, 8388608.0f, 8388608.0f } } };
		constexpr vec_i32_t g_MaskByte = { { { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF } } };
		constexpr vec_f32_t g_NegateX = { { { -1.0f, 1.0f, 1.0f, 1.0f } } };
		constexpr vec_f32_t g_NegateY = { { { 1.0f, -1.0f, 1.0f, 1.0f } } };
		constexpr vec_f32_t g_NegateZ = { { { 1.0f, 1.0f, -1.0f, 1.0f } } };
		constexpr vec_f32_t g_NegateW = { { { 1.0f, 1.0f, 1.0f, -1.0f } } };
		constexpr vec_u32_t g_Select0101 = { { { g_select_0, g_select_1, g_select_0, g_select_1 } } };
		constexpr vec_u32_t g_Select1010 = { { { g_select_1, g_select_0, g_select_1, g_select_0 } } };
		constexpr vec_i32_t g_OneHalfMinusEpsilon = { { { 0x3EFFFFFD, 0x3EFFFFFD, 0x3EFFFFFD, 0x3EFFFFFD } } };
		constexpr vec_u32_t g_Select1000 = { { { g_select_1, g_select_0, g_select_0, g_select_0 } } };
		constexpr vec_u32_t g_Select1100 = { { { g_select_1, g_select_1, g_select_0, g_select_0 } } };
		constexpr vec_u32_t g_Select1110 = { { { g_select_1, g_select_1, g_select_1, g_select_0 } } };
		constexpr vec_u32_t g_Select1011 = { { { g_select_1, g_select_0, g_select_1, g_select_1 } } };
		constexpr vec_f32_t g_FixupY16 = { { { 1.0f, 1.0f / 65536.0f, 0.0f, 0.0f } } };
		constexpr vec_f32_t g_FixupY16W16 = { { { 1.0f, 1.0f, 1.0f / 65536.0f, 1.0f / 65536.0f } } };
		constexpr vec_u32_t g_FlipY = { { { 0, 0x80000000, 0, 0 } } };
		constexpr vec_u32_t g_FlipZ = { { { 0, 0, 0x80000000, 0 } } };
		constexpr vec_u32_t g_FlipW = { { { 0, 0, 0, 0x80000000 } } };
		constexpr vec_u32_t g_FlipYZ = { { { 0, 0x80000000, 0x80000000, 0 } } };
		constexpr vec_u32_t g_FlipZW = { { { 0, 0, 0x80000000, 0x80000000 } } };
		constexpr vec_u32_t g_FlipYW = { { { 0, 0x80000000, 0, 0x80000000 } } };
		constexpr vec_i32_t g_MaskDec4 = { { { 0x3FF, 0x3FF << 10, 0x3FF << 20, static_cast<int>(0xC0000000) } } };
		constexpr vec_i32_t g_XorDec4 = { { { 0x200, 0x200 << 10, 0x200 << 20, 0 } } };
		constexpr vec_f32_t g_AddUDec4 = { { { 0, 0, 0, 32768.0f*65536.0f } } };
		constexpr vec_f32_t g_AddDec4 = { { { -512.0f, -512.0f*1024.0f, -512.0f*1024.0f*1024.0f, 0 } } };
		constexpr vec_f32_t g_MulDec4 = { { { 1.0f, 1.0f / 1024.0f, 1.0f / (1024.0f*1024.0f), 1.0f / (1024.0f*1024.0f*1024.0f) } } };
		constexpr vec_u32_t g_MaskByte4 = { { { 0xFF, 0xFF00, 0xFF0000, 0xFF000000 } } };
		constexpr vec_i32_t g_XorByte4 = { { { 0x80, 0x8000, 0x800000, 0x00000000 } } };
		constexpr vec_f32_t g_AddByte4 = { { { -128.0f, -128.0f*256.0f, -128.0f*65536.0f, 0 } } };
		constexpr vec_f32_t g_FixUnsigned = { { { 32768.0f*65536.0f, 32768.0f*65536.0f, 32768.0f*65536.0f, 32768.0f*65536.0f } } };
		constexpr vec_f32_t g_MaxInt = { { { 65536.0f*32768.0f - 128.0f, 65536.0f*32768.0f - 128.0f, 65536.0f*32768.0f - 128.0f, 65536.0f*32768.0f - 128.0f } } };
		constexpr vec_f32_t g_MaxUInt = { { { 65536.0f*65536.0f - 256.0f, 65536.0f*65536.0f - 256.0f, 65536.0f*65536.0f - 256.0f, 65536.0f*65536.0f - 256.0f } } };
		constexpr vec_f32_t g_UnsignedFix = { { { 32768.0f*65536.0f, 32768.0f*65536.0f, 32768.0f*65536.0f, 32768.0f*65536.0f } } };
		constexpr vec_f32_t g_srgbScale = { { { 12.92f, 12.92f, 12.92f, 1.0f } } };
		constexpr vec_f32_t g_srgbA = { { { 0.055f, 0.055f, 0.055f, 0.0f } } };
		constexpr vec_f32_t g_srgbA1 = { { { 1.055f, 1.055f, 1.055f, 1.0f } } };
		constexpr vec_i32_t g_ExponentBias = { { { 127, 127, 127, 127 } } };
		constexpr vec_i32_t g_SubnormalExponent = { { { -126, -126, -126, -126 } } };
		constexpr vec_i32_t g_NumTrailing = { { { 23, 23, 23, 23 } } };
		constexpr vec_i32_t g_MinNormal = { { { 0x00800000, 0x00800000, 0x00800000, 0x00800000 } } };
		constexpr vec_u32_t g_NegInfinity = { { { 0xFF800000, 0xFF800000, 0xFF800000, 0xFF800000 } } };
		constexpr vec_u32_t g_NegQNaN = { { { 0xFFC00000, 0xFFC00000, 0xFFC00000, 0xFFC00000 } } };
		constexpr vec_i32_t g_Bin128 = { { { 0x43000000, 0x43000000, 0x43000000, 0x43000000 } } };
		constexpr vec_u32_t g_BinNeg150 = { { { 0xC3160000, 0xC3160000, 0xC3160000, 0xC3160000 } } };
		constexpr vec_i32_t g_253 = { { { 253, 253, 253, 253 } } };
		constexpr vec_f32_t g_ExpEst1 = { { { -6.93147182e-1f, -6.93147182e-1f, -6.93147182e-1f, -6.93147182e-1f } } };
		constexpr vec_f32_t g_ExpEst2 = { { { +2.40226462e-1f, +2.40226462e-1f, +2.40226462e-1f, +2.40226462e-1f } } };
		constexpr vec_f32_t g_ExpEst3 = { { { -5.55036440e-2f, -5.55036440e-2f, -5.55036440e-2f, -5.55036440e-2f } } };
		constexpr vec_f32_t g_ExpEst4 = { { { +9.61597636e-3f, +9.61597636e-3f, +9.61597636e-3f, +9.61597636e-3f } } };
		constexpr vec_f32_t g_ExpEst5 = { { { -1.32823968e-3f, -1.32823968e-3f, -1.32823968e-3f, -1.32823968e-3f } } };
		constexpr vec_f32_t g_ExpEst6 = { { { +1.47491097e-4f, +1.47491097e-4f, +1.47491097e-4f, +1.47491097e-4f } } };
		constexpr vec_f32_t g_ExpEst7 = { { { -1.08635004e-5f, -1.08635004e-5f, -1.08635004e-5f, -1.08635004e-5f } } };
		constexpr vec_f32_t g_LogEst0 = { { { +1.442693f, +1.442693f, +1.442693f, +1.442693f } } };
		constexpr vec_f32_t g_LogEst1 = { { { -0.721242f, -0.721242f, -0.721242f, -0.721242f } } };
		constexpr vec_f32_t g_LogEst2 = { { { +0.479384f, +0.479384f, +0.479384f, +0.479384f } } };
		constexpr vec_f32_t g_LogEst3 = { { { -0.350295f, -0.350295f, -0.350295f, -0.350295f } } };
		constexpr vec_f32_t g_LogEst4 = { { { +0.248590f, +0.248590f, +0.248590f, +0.248590f } } };
		constexpr vec_f32_t g_LogEst5 = { { { -0.145700f, -0.145700f, -0.145700f, -0.145700f } } };
		constexpr vec_f32_t g_LogEst6 = { { { +0.057148f, +0.057148f, +0.057148f, +0.057148f } } };
		constexpr vec_f32_t g_LogEst7 = { { { -0.010578f, -0.010578f, -0.010578f, -0.010578f } } };
		constexpr vec_f32_t g_LgE = { { { +1.442695f, +1.442695f, +1.442695f, +1.442695f } } };
		constexpr vec_f32_t g_InvLgE = { { { +6.93147182e-1f, +6.93147182e-1f, +6.93147182e-1f, +6.93147182e-1f } } };
		constexpr vec_f32_t g_UByteMax = { { { 255.0f, 255.0f, 255.0f, 255.0f } } };
		constexpr vec_f32_t g_ByteMin = { { { -127.0f, -127.0f, -127.0f, -127.0f } } };
		constexpr vec_f32_t g_ByteMax = { { { 127.0f, 127.0f, 127.0f, 127.0f } } };
		constexpr vec_f32_t g_ShortMin = { { { -32767.0f, -32767.0f, -32767.0f, -32767.0f } } };
		constexpr vec_f32_t g_ShortMax = { { { 32767.0f, 32767.0f, 32767.0f, 32767.0f } } };
		constexpr vec_f32_t g_UShortMax = { { { 65535.0f, 65535.0f, 65535.0f, 65535.0f } } };


		/****************************************************************************
		 *
		 * Implementation
		 *
		 ****************************************************************************/

		 //------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV vector_set_binary_constant(u32 C0, u32 C1, u32 C2, u32 C3)
		{
#if defined(LUNA_PLATFORM_ARM_NEON)
			vec_uint32_t vResult;
			vResult.u[0] = (0 - (C0 & 1)) & 0x3F800000;
			vResult.u[1] = (0 - (C1 & 1)) & 0x3F800000;
			vResult.u[2] = (0 - (C2 & 1)) & 0x3F800000;
			vResult.u[3] = (0 - (C3 & 1)) & 0x3F800000;
			return vResult.v;
#else //LUNA_PLATFORM_SSE2
			constexpr vec_u32_t g_vMask1{ { 1, 1, 1, 1 } };
			// Move the params to a vector
			__m128i vTemp = _mm_set_epi32(static_cast<int>(C3), static_cast<int>(C2), static_cast<int>(C1), static_cast<int>(C0));
			// Mask off the low bits
			vTemp = _mm_and_si128(vTemp, g_vMask1);
			// 0xFFFFFFFF on true bits
			vTemp = _mm_cmpeq_epi32(vTemp, g_vMask1);
			// 0xFFFFFFFF -> 1.0f, 0x00000000 -> 0.0f
			vTemp = _mm_and_si128(vTemp, g_One);
			return _mm_castsi128_ps(vTemp);
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_constant(i32 intConstant, u32 divExponent)
		{
			luassert(intConstant >= -16 && intConstant <= 15);
			luassert(divExponent < 32);
#if defined(LUNA_PLATFORM_ARM_NEON)
			// Splat the int
			int32x4_t vScale = vdupq_n_s32(intConstant);
			// Convert to a float
			XMVECTOR vResult = vcvtq_f32_s32(vScale);
			// Convert DivExponent into 1.0f/(1<<DivExponent)
			uint32_t uScale = 0x3F800000U - (divExponent << 23);
			// Splat the scalar value (It's really a float)
			vScale = vdupq_n_u32(uScale);
			// Multiply by the reciprocal (Perform a right shift by DivExponent)
			vResult = vmulq_f32(vResult, reinterpret_cast<const float32x4_t *>(&vScale)[0]);
			return vResult;
#else // LUNA_PLATFORM_SSE2
			// Splat the int
			__m128i vScale = _mm_set1_epi32(intConstant);
			// Convert to a float
			vec_float_t vResult = _mm_cvtepi32_ps(vScale);
			// Convert DivExponent into 1.0f/(1<<DivExponent)
			uint32_t uScale = 0x3F800000U - (divExponent << 23);
			// Splat the scalar value (It's really a float)
			vScale = _mm_set1_epi32(static_cast<int>(uScale));
			// Multiply by the reciprocal (Perform a right shift by DivExponent)
			vResult = _mm_mul_ps(vResult, _mm_castsi128_ps(vScale));
			return vResult;
#endif
		}

		//------------------------------------------------------------------------------

		inline vec_float_t LUNA_SIMD_CALLCONV vector_splat_constant_int(i32 intConstant)
		{
			luassert(intConstant >= -16 && intConstant <= 15);
#if defined(LUNA_PLATFORM_ARM_NEON)
			int32x4_t V = vdupq_n_s32(intConstant);
			return reinterpret_cast<float32x4_t *>(&V)[0];
#else // LUNA_PLATFORM_SSE2
			__m128i V = _mm_set1_epi32(intConstant);
			return _mm_castsi128_ps(V);
#endif
		}
	}
#endif // LUNA_SIMD_ENABLED
}