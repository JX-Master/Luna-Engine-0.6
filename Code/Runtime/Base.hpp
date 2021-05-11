// Copyright 2018-2020 Luna Project. All rights reserved.
/*
* @file Base.hpp
* @author JXMaster
* @date 2018/10/26
* @brief Defines basic types and functions to be used in Luna Engine.
 */
#pragma once

#include "PlatformDefines.hpp"

// Include C++ STD.
// This is the only place where Luna Engine includes C++ STD. We only uses a minimum set of 
// STD headers so that the engine is portable to most platforms.
#include <cstdint>
#include <cfloat>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cmath>
#include <cctype>

#include <new>
#include <type_traits>
#include <initializer_list>

namespace Luna
{
    //------------------------------------------------------------------------------------------------------
    //  Basic Types
    //------------------------------------------------------------------------------------------------------

	// `bool` and `void` types are used as-is.

    //! Unsigned 8-bit integer (0~255).
    using u8 = std::uint8_t;
    //! Unsigned 16-bit integer (0~65535).
    using u16 = std::uint16_t;
    //! Unsigned 32-bit integer (0~4294967295).
    using u32 = std::uint32_t;
    //! Unsigned 64-bit integer (0~18446744073709551615).
    using u64 = std::uint64_t;
    //! Signed 8-bit integer (-128~127).
    using i8 = std::int8_t;
    //! Signed 16-bit integer (-32768~32767).
    using i16 = std::int16_t;
    //! Signed 32-bit integer (-2147482648~2147483647).
    using i32 = std::int32_t;
    //! Signed 64-bit integer (-9223372036854775808~9223372036854775807).
    using i64 = std::int64_t;
    //! 32-bit (single precision) floating point number.
    using f32 = float;
    //! 64-bit (double precision) floating point number.
    using f64 = double;

    //! `USize` is the unsigned integer type of whose length marches the machine architecture.
	//! In particular, in 32-bit application, this is 32-bit unsigned integer; in 64-bit application, 
    //! this is 64-bit unsigned integer.
	//! The `USize` type is guaranteed to be large enough to store a indexable memory address, so that 
	//! any pointer can be reinterpreted casted to `USize`.
    using usize = std::size_t;
    //! `ISize` is similar to `USize` , but is a signed integer type, so this can be used to 
    //! store the result of subtracting two pointers, and can be used to offset memory addresses and pointers.
    using isize = std::ptrdiff_t;

	//! `Handle` is an alising type of `void*`. It denotes that this pointer represents an address 
    //! that is opaque to the user, and the user should not cast, dereference or doing any operations 
    //! on the address itself. Mostly the underlying system will provide necessary APIs for dealing
    //! with handle.
	using handle_t = void*;

	//! The type that uses the maximum alignment required.
	using std::max_align_t;

    //! 8-bit character. Signed/unsigned is unspecified, cast this to u8/i8 for fetching number.
	using c8 = char;
	static_assert(sizeof(c8) == 1, "Incorrect c8 type size.");
    //! 16-bit character. Signed/unsigned is unspecified, cast this to u16/i16 for fetching number.
    using c16 = char16_t;
    //! 32-bit character. Signed/unsigned is unspecified, cast this to u32/i32 for fetching number.
    using c32 = char32_t;

	template <typename _Ty>
	using InitializerList = std::initializer_list<_Ty>;

	using VarList = va_list;

	//------------------------------------------------------------------------------------------------------
	//  Basic Constants.
	//------------------------------------------------------------------------------------------------------

    //! The maximum number that can be represented by one i8 value.
	constexpr i8 i8_max = (i8)INT8_MAX;
    //! The minimum number that can be represented by one i8 value.
	constexpr i8 i8_min = (i8)INT8_MIN;
    //! The maximum number that can be represented by one i16 value.
	constexpr i16 i16_max = (i16)INT16_MAX;
    //! The minimum number that can be represented by one i16 value.
	constexpr i16 i16_min = (i16)INT16_MIN;
    //! The maximum number that can be represented by one i32 value.
	constexpr i32 i32_max = (i32)INT32_MAX;
    //! The minimum number that can be represented by one i32 value.
	constexpr i32 i32_min = (i32)INT32_MIN;
    //! The maximum number that can be represented by one i64 value.
	constexpr i64 i64_max = (i64)INT64_MAX;
    //! The minimum number that can be represented by one i64 value.
	constexpr i64 i64_min = (i64)INT64_MIN;
    //! The maximum number that can be represented by one u8 value.
	constexpr u8 u8_max = (u8)UINT8_MAX;
    //! The maximum number that can be represented by one u16 value.
	constexpr u16 u16_max = (u16)UINT16_MAX;
    //! The maximum number that can be represented by one u32 value.
	constexpr u32 u32_max = (u32)UINT32_MAX;
    //! The maximum number that can be represented by one u64 value.
	constexpr u64 u64_max = (u64)UINT64_MAX;
#ifdef LUNA_PLATFORM_64BIT
    //! The maximum number that can be represented by one usize value.
	constexpr usize usize_max = u64_max;
    //! The maximum number that can be represented by one isize value.
	constexpr isize isize_max = i64_max;
	//! The minimum number that can be represented by one isize value.
	constexpr isize isize_min = i64_min;
#else
    //! The maximum number that can be represented by one usize value.
	constexpr usize usize_max = u32_max;
    //! The maximum number that can be represented by one isize value.
	constexpr isize isize_max = i32_max;
	//! The minimum number that can be represented by one isize value.
	constexpr isize isize_min = i32_min;
#endif

	//! The maximum alignment requirement for a standard-layout scalar value.
	constexpr usize max_align = alignof(max_align_t);

	constexpr f32 f32_min = (f32)FLT_MIN;
	constexpr f32 f32_max = (f32)FLT_MAX;
	constexpr f32 f32_epsilon = (f32)FLT_EPSILON;

	//------------------------------------------------------------------------------------------------------
	//  Basic Functions
	//------------------------------------------------------------------------------------------------------

	using std::strncpy;
	using std::strcat;
	using std::strncat;
	using std::strxfrm;
	using std::strncmp;
	using std::strcoll;
	using std::strchr;
	using std::strrchr;
	using std::strspn;
	using std::strcspn;
	using std::strpbrk;
	using std::strstr;
	using std::strtok;
	using std::memchr;
	using std::memcmp;
	using std::memset;
	using std::memcpy;
	using std::memmove;

    using std::isalnum;
    using std::isalpha;
    using std::islower;
    using std::isupper;
    using std::isdigit;
    using std::isxdigit;
    using std::iscntrl;
    using std::isgraph;
    using std::isspace;
    using std::isblank;
    using std::isprint;
    using std::ispunct;
    using std::tolower;
    using std::toupper;

	// Additional functions to support c8, c16, c32 types.

	template <typename _CharT>
	usize strlen(const _CharT* s)
	{
		const _CharT* end = s;
		while (*end++ != 0);
		return end - s - 1;
	}

	template <typename _CharT>
	_CharT* strcpy(_CharT* dest, const _CharT* src)
	{
		_CharT* t = dest;
		while (*src)
		{
			*t = *src;
			++t;
			++src;
		}
		*t = (_CharT)0;
		return dest;
	}

	template <typename _CharT>
	i32 strcmp(const _CharT* lhs, const _CharT* rhs)
	{
		_CharT l = *lhs;
		_CharT r = *rhs;
		while (l && (l == r))
		{
			++lhs;
			++rhs;
			l = *lhs;
			r = *rhs;
		}
		return (i32)l - (i32)r;
	}

	namespace Impl
	{
		constexpr const u8 bit_mask[] = {
			0x01,	// 00000001
			0x02,	// 00000010
			0x04,	// 00000100
			0x08,	// 00001000
			0x10,	// 00010000
			0x20,	// 00100000
			0x40,	// 01000000
			0x80		// 10000000
		};

		constexpr const u8 bit_mask_reverse[] = {
			0xfe,	// 11111110
			0xfd,	// 11111101
			0xfb,	// 11111011
			0xf7,	// 11110111
			0xef,	// 11101111
			0xdf,	// 11011111
			0xbf,	// 10111111
			0x7f		// 01111111
		};
	}

	inline constexpr u64 operator"" _kb(u64 v)
	{
		return v * 1024;
	}

	inline constexpr u64 operator"" _mb(u64 v)
	{
		return v * 1024 * 1024;
	}

	inline constexpr u64 operator"" _gb(u64 v)
	{
		return v * 1024 * 1024 * 1024;
	}

	inline constexpr u64 operator"" _tb(u64 v)
	{
		return v * 1024 * 1024 * 1024 * 1024;
	}

	//! Tests if specified bit is 1.
	//! @param[in] base_addr The address of the bit to offset from.
	//! @param[in] bit_offset The number of bits shifted from the `base_addr`.
	//! @return Returns `true` if the bit is 1, `false` if the bit is 0.
	//! @remark The following cases demonstrate the index order of `bit_test`, `bit_set` and `bit_reset`.
	//! 
	//! base_addr: 0x1000, bit_offset: 0, `*((u8*)0x1000)`: 0000 1000b
	//! * test result: false.
	//! * value of `*((u8*)0x1000)` after set: 0000 1001b
	//! * value of `*((u8*)0x1000)` after reset: 0000 1000b.
	//! 
	//! base_addr: 0x1000, bit_offset: 3, `*((u8*)0x1000)`: 0000 1000b
	//! * test result: true.
	//! * value of `*((u8*)0x1000)` after set: 0000 1000b
	//! * value of `*((u8*)0x1000)` after reset: 0000 0000b.
	//! 
	//! base_addr: 0x1000, bit_offset: 8, `*((u8*)0x1001)`: 0000 1000b
	//! * test result: false.
	//! * value of `*((u8*)0x1001)` after set: 0000 1001b
	//! * value of `*((u8*)0x1001)` after reset: 0000 1000b.
	//! 
	//! base_addr: 0x1000, bit_offset: 11, `*((u8*)0x1001)`: 0000 1000b
	//! * test result: true.
	//! * value of `*((u8*)0x1001)` after set: 0000 1000b
	//! * value of `*((u8*)0x1001)` after reset: 0000 0000b.
	inline bool bit_test(const void* base_addr, usize bit_offset)
	{
		return (*(const u8*)((usize)base_addr + bit_offset / 8)) & Impl::bit_mask[bit_offset % 8] ? true : false;;
	}

	//! Sets the specified bit to 1.
	//! @param[in] base_addr The address of the bit to offset from.
	//! @param[in] bit_offset The number of bits shifted from the `base_addr`.
	//! @remark See remarks of `bit_test` for details.
	inline void bit_set(void* addr, usize bit_offset)
	{
		*(u8*)((usize)addr + bit_offset / 8) |= Impl::bit_mask[bit_offset % 8];
	}

	//! Sets the specified bit to 0.
	//! @param[in] base_addr The address of the bit to offset from.
	//! @param[in] bit_offset The number of bits shifted from the `base_addr`.
	//! @remark See remarks of `bit_test` for details.
	inline void bit_reset(void* addr, usize bit_offset)
	{
		*(u8*)((usize)addr + bit_offset / 8) &= Impl::bit_mask_reverse[bit_offset % 8];
	}

	//! Returns the address/size that aligns the origin address/size to the nearest matched aligned 
	//! address/size that is greater than or equal to the the origin address/size.
	//! @param[in] origin The unaligned address/size.
	//! @param[in] alignment The alignment boundary. If alignment is 0, `origin` will be returned as-is.
	//! @return Returns the aligned address/size.
	inline constexpr usize align_upper(usize origin, usize alignment)
	{
		return alignment ? ((origin + alignment - 1) / alignment * alignment) : origin;
	}

	//! Decodes one hex char to corresponding number. 
	//! @param[in] x The hex char to decode. The char must be in `[0-9]|[a-f]|[A-F]` range.
	//! @return Returns the decoded number in range [0, 16).
	//! Returns 0 if the provided char is not a hex char.
	inline constexpr u8 atohex(c8 x)
	{
		return x >= '0' && x <= '9' ? x - '0' :
			x >= 'a' && x <= 'f' ? x - 'a' + 10 :
			x >= 'A' && x <= 'F' ? x - 'A' + 10 : 0;
	}
	static_assert(atohex('3') == 0x03, "atohex Wrong");
	static_assert(atohex('e') == 0x0e, "atohex Wrong");
	static_assert(atohex('C') == 0x0C, "atohex Wrong");

	//! Compares the two string until one of the string sequence reaches null terminator, and
	//! returns 0 if the compared part is equal. Examples:
	//! * "aaa" & "aaab" -> 0 (only "aaa" is compared).
	//! * "aaa" & "aa" -> 0 (only "aa" is compared).
	//! * "" & "aa" -> 0 (nothing is compared).
	//! * "aba" & "aaa" -> not 0.
	//! * "aaab" & "aaac" -> not 0.
	inline i32 strcmp_prefix(const c8* str1, const c8* str2)
	{
		c8 c1, c2;
		while (*str1 && *str2)
		{
			c1 = *str1;
			c2 = *str2;
			if (c1 != c2)
			{
				return (i32)c1 - (i32)c2;
			}
			++str1;
			++str2;
		}
		return 0;
	}

	inline usize strlen16(const c16* s)
	{
		if (!s) return 0;
		usize len = 0;
		while (s[len++]);
		return len;
	}

	//! clear the specified memory region to 0.
	//! @param[in] dest The start address of memory region to clear.
	//! @param[in] byteCount The size, in bytes, of the memory region to clear.
	//! @return Returns the `dest` pointer.
	inline void* memzero(void* dest, usize byte_count)
	{
		return memset(dest, 0, byte_count);
	}

	//! Copies the data for a 2D bitmap.
	//! @param[in] dest A pointer to the first pixel to be copied in destination bitmap.
	//! @param[in] src A pointer to the first pixel to be copied in source bitmap.
	//! @param[in] copy_size_per_row The size of the data to be copied for every row, in bytes.
	//! @param[in] num_rows The number of rows to copy.
	//! @param[in] dest_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @param[in] src_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @return Returns the `dest` pointer.
	inline void* memcpy_bitmap(void* dest, const void* src, usize copy_size_per_row, usize num_rows, usize dest_row_pitch, usize src_row_pitch)
	{
		for (usize r = 0; r < num_rows; ++r)
		{
			memcpy((void*)((usize)dest + r * dest_row_pitch), (const void*)((usize)src + r * src_row_pitch), copy_size_per_row);
		}
		return dest;
	}

	//! Copies the data for a 3D bitmap.
	//! @param[in] dest A pointer to the first pixel to be copied in destination bitmap.
	//! @param[in] src A pointer to the first pixel to be copied in source bitmap.
	//! @param[in] copy_size_per_row The size of the data to be copied for every row, in bytes.
	//! @param[in] num_rows The number of rows to copy.
	//! @param[in] num_slices The number of slices (layers) to copy.
	//! @param[in] dest_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @param[in] src_row_pitch The size to advance for one row in destination bitmap in bytes.
	//! @param[in] dest_slice_pitch The size to advance for one slice (layer) in destination bitmap in bytes.
	//! @param[in] src_slice_pitch The size to advance for one slice (layer) in destination bitmap in bytes.
	//! @return Returns the `dest` pointer.
	inline void* memcpy_bitmap3d(
		void* dest, const void* src,
		usize copy_size_per_row,
		usize num_rows, usize num_slices,
		usize dest_row_pitch, usize src_row_pitch,
		usize dest_slice_pitch, usize src_slice_pitch)
	{
		for (usize r = 0; r < num_slices; ++r)
		{
			memcpy_bitmap((void*)((usize)dest + r * dest_slice_pitch), (const void*)((usize)src + r * src_slice_pitch), copy_size_per_row,
				num_rows, dest_row_pitch, src_row_pitch);
		}
		return dest;
	}

	//! Returns a pointer that offsets the specified pixels in the texture.
	inline void* pixel_offset(void* base, usize x, usize y, usize z, usize bytes_per_pixel, usize row_pitch, usize slice_pitch)
	{
		usize r = (usize)base;
		r += z * slice_pitch + y * row_pitch + x * bytes_per_pixel;
		return (void*)r;
	}
	inline const void* pixel_offset(const void* base, usize x, usize y, usize z, usize bytes_per_pixel, usize row_pitch, usize slice_pitch)
	{
		usize r = (usize)base;
		r += z * slice_pitch + y * row_pitch + x * bytes_per_pixel;
		return (const void*)r;
	}

	//------------------------------------------------------------------------------------------------------
	//  Type Traits
	//------------------------------------------------------------------------------------------------------

	using std::integral_constant;
	using std::true_type;
	using std::false_type;

	using std::is_void;
	using std::is_null_pointer;
	using std::is_integral;
	using std::is_floating_point;
	using std::is_array;
	using std::is_enum;
	using std::is_union;
	using std::is_class;
	using std::is_function;
	using std::is_pointer;
	using std::is_lvalue_reference;
	using std::is_rvalue_reference;
	using std::is_member_object_pointer;
	using std::is_member_function_pointer;

	template<class _T> constexpr bool is_void_v = is_void<_T>::value;
	template<class _T> constexpr bool is_null_pointer_v = is_null_pointer<_T>::value;
	template<class _T> constexpr bool is_integral_v = is_integral<_T>::value;
	template<class _T> constexpr bool is_floating_point_v = is_floating_point<_T>::value;
	template<class _T> constexpr bool is_enum_v = is_enum<_T>::value;
	template<class _T> constexpr bool is_union_v = is_union<_T>::value;
	template<class _T> constexpr bool is_class_v = is_class<_T>::value;
	template<class _T> constexpr bool is_function_v = is_function<_T>::value;
	template<class _T> constexpr bool is_pointer_v = is_pointer<_T>::value;
	template<class _T> constexpr bool is_lvalue_reference_v = is_lvalue_reference<_T>::value;
	template<class _T> constexpr bool is_rvalue_reference_v = is_rvalue_reference<_T>::value;
	template<class _T> constexpr bool is_member_object_pointer_v = is_member_object_pointer<_T>::value;
	template<class _T> constexpr bool is_member_function_pointer_v = is_member_function_pointer<_T>::value;

	using std::is_fundamental;
	using std::is_arithmetic;
	using std::is_scalar;
	using std::is_object;
	using std::is_compound;
	using std::is_reference;
	using std::is_member_pointer;

	template<class _T> constexpr bool is_fundamental_v = is_fundamental<_T>::value;
	template<class _T> constexpr bool is_arithmetic_v = is_arithmetic<_T>::value;
	template<class _T> constexpr bool is_scalar_v = is_scalar<_T>::value;
	template<class _T> constexpr bool is_object_v = is_object<_T>::value;
	template<class _T> constexpr bool is_compound_v = is_compound<_T>::value;
	template<class _T> constexpr bool is_reference_v = is_reference<_T>::value;
	template<class _T> constexpr bool is_member_pointer_v = is_member_pointer<_T>::value;

	using std::is_const;
	using std::is_volatile;
	using std::is_trivial;
	using std::is_trivially_copyable;
	using std::is_standard_layout;
	using std::is_empty;
	using std::is_polymorphic;
	using std::is_abstract;
	using std::is_final;
	using std::is_signed;
	using std::is_unsigned;

	template<class _T> constexpr bool is_const_v = is_const<_T>::value;
	template<class _T> constexpr bool is_volatile_v = is_volatile<_T>::value;
	template<class _T> constexpr bool is_trivial_v = is_trivial<_T>::value;
	template<class _T> constexpr bool is_trivially_copyable_v = is_trivially_copyable<_T>::value;
	template<class _T> constexpr bool is_standard_layout_v = is_standard_layout<_T>::value;
	template<class _T> constexpr bool is_empty_v = is_empty<_T>::value;
	template<class _T> constexpr bool is_polymorphic_v = is_polymorphic<_T>::value;
	template<class _T> constexpr bool is_abstract_v = is_abstract<_T>::value;
	template<class _T> constexpr bool is_final_v = is_final<_T>::value;
	template<class _T> constexpr bool is_signed_v = is_signed<_T>::value;
	template<class _T> constexpr bool is_unsigned_v = is_unsigned<_T>::value;

	using std::is_constructible;
	using std::is_trivially_constructible;
	using std::is_nothrow_constructible;
	using std::is_default_constructible;
	using std::is_trivially_default_constructible;
	using std::is_nothrow_default_constructible;
	using std::is_copy_constructible;
	using std::is_trivially_copy_constructible;
	using std::is_nothrow_copy_constructible;
	using std::is_move_constructible;
	using std::is_trivially_move_constructible;
	using std::is_nothrow_move_constructible;
	using std::is_assignable;
	using std::is_trivially_assignable;
	using std::is_nothrow_assignable;
	using std::is_copy_assignable;
	using std::is_trivially_copy_assignable;
	using std::is_nothrow_copy_assignable;
	using std::is_move_assignable;
	using std::is_trivially_move_assignable;
	using std::is_nothrow_move_assignable;
	using std::is_destructible;
	using std::is_trivially_destructible;
	using std::is_nothrow_destructible;
	using std::has_virtual_destructor;

	template<class _T> constexpr bool is_constructible_v = is_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_constructible_v = is_trivially_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<_T>::value;
	template<class _T> constexpr bool is_default_constructible_v = is_default_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_default_constructible_v = is_trivially_default_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<_T>::value;
	template<class _T> constexpr bool is_copy_constructible_v = is_copy_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_copy_constructible_v = is_trivially_copy_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<_T>::value;
	template<class _T> constexpr bool is_move_constructible_v = is_move_constructible<_T>::value;
	template<class _T> constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<_T>::value;
	template<class _T, class _U> constexpr bool is_assignable_v = is_assignable<_T, _U>::value;
	template<class _T, class _U> constexpr bool is_trivially_assignable_v = is_trivially_assignable<_T, _U>::value;
	template<class _T, class _U> constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<_T, _U>::value;
	template<class _T> constexpr bool is_copy_assignable_v = is_copy_assignable<_T>::value;
	template<class _T> constexpr bool is_trivially_copy_assignable_v = is_trivially_copy_assignable<_T>::value;
	template<class _T> constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<_T>::value;
	template<class _T> constexpr bool is_move_assignable_v = is_move_assignable<_T>::value;
	template<class _T> constexpr bool is_trivially_move_assignable_v = is_trivially_move_assignable<_T>::value;
	template<class _T> constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<_T>::value;
	template<class _T> constexpr bool is_destructible_v = is_destructible<_T>::value;
	template<class _T> constexpr bool is_trivially_destructible_v = is_trivially_destructible<_T>::value;
	template<class _T> constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<_T>::value;
	template<class _T> constexpr bool has_virtual_destructor_v = has_virtual_destructor<_T>::value;

	using std::is_same;
	using std::is_base_of;
	using std::is_convertible;
	template<class _T, class _U> constexpr bool is_same_v = is_same<_T, _U>::value;
	template<class _Base, class _Derived> constexpr bool is_base_of_v = is_base_of<_Base, _Derived>::value;
	template<class _From, class _To> constexpr bool is_convertible_v = is_convertible<_From, _To>::value;

	using std::remove_cv;
	using std::remove_const;
	using std::remove_volatile;
	using std::remove_cv_t;
	using std::remove_const_t;
	using std::remove_volatile_t;

	using std::add_cv;
	using std::add_const;
	using std::add_volatile;
	using std::add_cv_t;
	using std::add_const_t;
	using std::add_volatile_t;

	using std::remove_reference;
	using std::remove_reference_t;
	using std::add_lvalue_reference;
	using std::add_lvalue_reference_t;
	using std::add_rvalue_reference;
	using std::add_rvalue_reference_t;

	using std::remove_pointer;
	using std::remove_pointer_t;
	using std::add_pointer;
	using std::add_pointer_t;
	using std::make_signed;
	using std::make_signed_t;
	using std::make_unsigned;
	using std::make_unsigned_t;
	using std::remove_extent;
	using std::remove_extent_t;
	using std::remove_all_extents;
	using std::remove_all_extents_t;

	using std::aligned_storage;
	using std::aligned_storage_t;
	using std::aligned_union;
	using std::aligned_union_t;
	using std::decay;
	using std::decay_t;
	using std::enable_if;
	using std::enable_if_t;
	using std::conditional;
	using std::conditional_t;
	using std::common_type;
	using std::common_type_t;
	using std::underlying_type;
	using std::underlying_type_t;
	//using std::result_of;
	//using std::result_of_t;

	namespace impl
	{
		template <typename _Ty>
		inline constexpr bool is_over_aligned()
		{
			return alignof(_Ty) > max_align;
		}
	}

	//! Checks if the specified type's alignment requirement is greater than alignof(max_align_t).
	template <typename _Ty>
	struct is_over_aligned : integral_constant<bool, impl::is_over_aligned<_Ty>()> {};

	template<typename _Ty>
	constexpr bool is_over_aligned_v = is_over_aligned<_Ty>::value;

	//! Checks if the specified type can be trivially relocatable.
	//! One object is trivially relocatable if the data of one well-constructed instance of object can be copied to a new 
	//! uninitialized memory by bitwise copy (for example, `memcpy`, `memmove` or `realloc`) and the new copied object behaves
	//! the same as the original object, such copy is called a "relocating operation". After the operation, the original memory
	//! for the object is treated as uninitialized and does not have destructor called before it is freed.
	//! 
	//! If one object can be trivially relocated, then when the memory that holding the object needs to be reallocated (for example
	//! when the container needs to expand its capacity), it performs `memcpy`, `memmove` or any other bitwise copy algorithms 
	//! directly on the data without any move constructors and destructors being called. If one object cannot be trivially relocated,
	//! then the relocating operation will call the move constructor for the object on the new memory (passing the old object as rvalue 
	//! reference), then call the destructor for the old object to properly destruct itself before freeing the memory for the old object.
	//! 
	//! All objects are trivially relocatable unless the user explicitly creates a template specialization that evaluates this to 
	//! `false_type` for the object that cannot be trivially relocated. In fact, the case that one type cannot be trivially relocatable
	//! is very rare, this only happens if the type holds a pointer to `this` and needs to update the pointer if the object is relocated.
	template <typename _Ty>
	struct is_trivially_relocatable : integral_constant<bool, true> {};

	template <typename _Ty>
	constexpr bool is_trivially_relocatable_v = is_trivially_relocatable<_Ty>::value;

	// Introduced in C++17, but is very useful.

	using std::conjunction;
	using std::disjunction;
	using std::negation;

	template<typename... _B>
	constexpr bool conjunction_v = conjunction<_B...>::value;
	template<typename... _B>
	constexpr bool disjunction_v = disjunction<_B...>::value;
	template<typename _B>
	constexpr bool negation_v = negation<_B>::value;

	// Define forward and move.

	template <typename _Ty>
	constexpr _Ty&& forward(typename remove_reference<_Ty>::type& x)
	{
		return static_cast<_Ty&&>(x);
	}

	template <typename _Ty>
	constexpr _Ty&& forward(typename remove_reference<_Ty>::type&& x)
	{
		static_assert(!is_lvalue_reference<_Ty>::value, "forward _Ty isn't lvalue reference");
		return static_cast<_Ty&&>(x);
	}

	template <typename _Ty>
	constexpr typename remove_reference<_Ty>::type&& move(_Ty&& x)
	{
		return ((typename remove_reference<_Ty>::type&&)x);
	}

	//------------------------------------------------------------------------------------------------------
	//  Miscellaneous
	//------------------------------------------------------------------------------------------------------
	
	//! @struct Guid
	//! `Guid` is a shortcut for Globally Unique Identifier. It is a 128-bit unsigned integer that is usually randomly
	//! generated and is used to identify one instance across multiple devices or domains.
	//! 
	//! `Guid` can be constructed from a literal string that is formatted in its canonical textual representation, which 
	//! appears like "xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx". One open branket and one close branket may be added to the 
	//! string, so both "123e4567-e89b-12d3-a456-426614174000" and "{123e4567-e89b-12d3-a456-426614174000}" are valid 
	//! canonical textural representations of one `Guid`.
	//! 
	//! See https://en.wikipedia.org/wiki/Universally_unique_identifier for details about GUID and its canonical textual 
	//! representation.
	struct Guid
	{
	public:
		u64 high;
		u64 low;
	private:
		constexpr u64 parse_high(const c8* s)
		{
			s = (*s == '{') ? s + 1 : s;
			return (u64)(atohex(s[0])) << 60 |
				(u64)(atohex(s[1])) << 56 |
				(u64)(atohex(s[2])) << 52 |
				(u64)(atohex(s[3])) << 48 |
				(u64)(atohex(s[4])) << 44 |
				(u64)(atohex(s[5])) << 40 |
				(u64)(atohex(s[6])) << 36 |
				(u64)(atohex(s[7])) << 32 |
				(u64)(atohex(s[9])) << 28 |
				(u64)(atohex(s[10])) << 24 |
				(u64)(atohex(s[11])) << 20 |
				(u64)(atohex(s[12])) << 16 |
				(u64)(atohex(s[14])) << 12 |
				(u64)(atohex(s[15])) << 8 |
				(u64)(atohex(s[16])) << 4 |
				(u64)(atohex(s[17]));
		}

		constexpr u64 parse_low(const c8* s)
		{
			s = (*s == '{') ? s + 1 : s;
			return (u64)(atohex(s[19])) << 60 |
				(u64)(atohex(s[20])) << 56 |
				(u64)(atohex(s[21])) << 52 |
				(u64)(atohex(s[22])) << 48 |
				(u64)(atohex(s[24])) << 44 |
				(u64)(atohex(s[25])) << 40 |
				(u64)(atohex(s[26])) << 36 |
				(u64)(atohex(s[27])) << 32 |
				(u64)(atohex(s[28])) << 28 |
				(u64)(atohex(s[29])) << 24 |
				(u64)(atohex(s[30])) << 20 |
				(u64)(atohex(s[31])) << 16 |
				(u64)(atohex(s[32])) << 12 |
				(u64)(atohex(s[33])) << 8 |
				(u64)(atohex(s[34])) << 4 |
				(u64)(atohex(s[35]));
		}
	public:
		constexpr Guid() : high(0), low(0) {}
		constexpr Guid(u64 h, u64 l) : high(h), low(l) {}
		constexpr Guid(const c8* s) :
			high(parse_high(s)), low(parse_low(s)) {}
		constexpr Guid(const Guid& rhs) : high(rhs.high), low(rhs.low) {}

		constexpr bool operator==(const Guid& rhs) const
		{
			return low == rhs.low && high == rhs.high;
		}
		constexpr bool operator!=(const Guid& rhs) const
		{
			return low != rhs.low || high != rhs.high;
		}
		constexpr bool operator<(const Guid& rhs) const
		{
			return (high != rhs.high) ? (high < rhs.high) : (low < rhs.low);
		}
		constexpr Guid& operator&=(const Guid& rhs)
		{
			low &= rhs.low;
			high &= rhs.high;
			return *this;
		}
		constexpr Guid& operator|=(const Guid& rhs)
		{
			low |= rhs.low;
			high |= rhs.high;
			return *this;
		}
		constexpr Guid& operator^=(const Guid& rhs)
		{
			low ^= rhs.low;
			high ^= rhs.high;
			return *this;
		}
	};

	constexpr Guid operator&(const Guid& lhs, const Guid& rhs)
	{
		return Guid(lhs.high & rhs.high, lhs.low & rhs.low);
	}

	constexpr Guid operator|(const Guid& lhs, const Guid& rhs)
	{
		return Guid(lhs.high | rhs.high, lhs.low | rhs.low);
	}

	constexpr Guid operator^(const Guid& lhs, const Guid& rhs)
	{
		return Guid(lhs.high ^ rhs.high, lhs.low ^ rhs.low);
	}

	static_assert(sizeof(Guid) == 16, "Wrong Guid size");

	// Grants bitwise operations for all enum class types. The operations may be invalid for non-bitwise enum class types,
	// but the users will not mess up with that.

	template <typename _Ty>
	constexpr auto operator|(_Ty const left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(static_cast<underlying_type_t<_Ty>>(left) | static_cast<underlying_type_t<_Ty>>(right));
	}

	template <typename _Ty>
	constexpr auto operator|=(_Ty& left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		left = left | right;
		return left;
	}

	template <typename _Ty>
	constexpr auto operator&(_Ty const left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(static_cast<underlying_type_t<_Ty>>(left) & static_cast<underlying_type_t<_Ty>>(right));
	}

	template <typename _Ty>
	constexpr auto operator&=(_Ty& left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		left = left & right;
		return left;
	}

	template <typename _Ty>
	constexpr auto operator~(_Ty const value) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(~static_cast<underlying_type_t<_Ty>>(value));
	}

	template <typename _Ty>
	constexpr auto operator^(_Ty const left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		return static_cast<_Ty>(static_cast<underlying_type_t<_Ty>>(left) ^ static_cast<underlying_type_t<_Ty>>(right));
	}

	template <typename _Ty>
	constexpr auto operator^=(_Ty& left, _Ty const right) noexcept -> enable_if_t<is_enum_v<_Ty>, _Ty>
	{
		left = left ^ right;
		return left;
	}

	//! @struct Pair
	//! Stores a pair of values. This struct is ABI-compatible and can be used as parameters for interface methods, whether by value, by pointer
	//! or by reference.
	template <typename _Ty1, typename _Ty2>
	struct Pair
	{
		using first_type = _Ty1;
		using second_type = _Ty2;
		// -------------------- Begin of ABI compatible part --------------------
		first_type first;
		second_type second;
		// --------------------  End of ABI compatible part  --------------------

		constexpr Pair() = default;

		constexpr Pair(const _Ty1& x, const _Ty2& y) :
			first(x), second(y) {}

		template <typename _U1, typename _U2>
		constexpr Pair(_U1&& x, _U2&& y) :
			first(forward<_U1>(x)), second(forward<_U2>(y)) {}

		template <typename _U1, typename _U2>
		constexpr Pair(const Pair<_U1, _U2>& p) :
			first(p.first), second(p.second) {}

		template <typename _U1, typename _U2>
		constexpr Pair(Pair<_U1, _U2>&& p) :
			first(move(p.first)), second(move(p.second)) {}

		Pair(const Pair&) = default;
		Pair(Pair&&) = default;
		constexpr Pair& operator=(const Pair& rhs)
		{
			first = rhs.first;
			second = rhs.second;
			return *this;
		}
		template <typename _U1, typename _U2>
		constexpr Pair& operator=(const Pair<_U1, _U2>& rhs)
		{
			first = rhs.first;
			second = rhs.second;
			return *this;
		}
		constexpr Pair& operator=(Pair&& rhs)
		{
			first = move(rhs.first);
			second = move(rhs.second);
			return *this;
		}
		template <typename _U1, typename _U2>
		constexpr Pair& operator=(Pair<_U1, _U2>&& rhs)
		{
			first = move(rhs.first);
			second = move(rhs.second);
			return *this;
		}
		constexpr void swap(Pair& rhs)
		{
			Pair tmp(move(*this));
			*this = move(rhs);
			rhs = move(tmp);
		}
	};

	template <typename _Ty1, typename _Ty2>
	constexpr Pair<decay_t<_Ty1>, decay_t<_Ty2>> make_pair(_Ty1&& first, _Ty2&& second)
	{
		return Pair<decay_t<_Ty1>, decay_t<_Ty2>>(forward<_Ty1>(first), forward<_Ty2>(second));
	}


}