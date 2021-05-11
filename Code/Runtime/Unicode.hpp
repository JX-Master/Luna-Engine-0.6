// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Unicode.hpp
* @author JXMaster
* @date 2018/10/26
 */
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	namespace impl
	{
		/*
		UTF8 Encoding:

		U+00000000~U+0000007F	7	0	0XXXXXXX
		U+00000080~U+000007FF	11	C0	110XXXXX 10XXXXXX
		U+00000800~U+0000FFFF	16	E0	1110XXXX 10XXXXXX 10XXXXXX
		U+00010000~U+001FFFFF	21	F0	11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
		U+00200000~U+03FFFFFF	26	F8	111110XX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX
		U+04000000~U+7FFFFFFF	31	FC	1111110X 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX 10XXXXXX
		*/

		// Unicode range.
		constexpr c32 l_utf8_one_start = 0x0000;
		constexpr c32 l_utf8_one_end = 0x007F;
		constexpr c32 l_utf8_two_start = 0x0080;
		constexpr c32 l_utf8_two_end = 0x07FF;
		constexpr c32 l_utf8_three_start = 0x0800;
		constexpr c32 l_utf8_three_end = 0xFFFF;
		constexpr c32 l_utf8_four_start = 0x00010000;
		constexpr c32 l_utf8_four_end = 0x001FFFFF;
		constexpr c32 l_utf8_five_start = 0x00200000;
		constexpr c32 l_utf8_five_end = 0x03FFFFFF;
		constexpr c32 l_utf8_six_start = 0x04000000;
		constexpr c32 l_utf8_six_end = 0x7FFFFFFF;
	}

	//! Returns the number of `char` characters needed to store the Unicode char `ch` in UTF-8 encoding.
	inline constexpr usize utf8_charspan(c32 ch)
	{
		if (ch <= impl::l_utf8_one_end)
		{
			return 1;
		}
		if (ch <= impl::l_utf8_two_end)
		{
			return 2;
		}
		if (ch <= impl::l_utf8_three_end)
		{
			return 3;
		}
		if (ch <= impl::l_utf8_four_end)
		{
			return 4;
		}
		if (ch <= impl::l_utf8_five_end)
		{
			return 5;
		}
		// The maximum number is 0x7FFFFFFF.
		return 6;
	}

	//! Returns the number of `char` characters the next Unicode char takes in the specified UTF-8 string.
	inline constexpr usize utf8_charlen(const c8* src)
	{
		c8 ch = src[0];
		if ((u8)ch <= 127)	// 0XXXXXXXb
		{
			return 1;
		}
		if ((u8)ch <= 223)	// 110XXXXXb
		{
			return 2;
		}
		if ((u8)ch <= 239)	// 1110XXXXb
		{
			return 3;
		}
		if ((u8)ch <= 247)	// 11110XXXb
		{
			return 4;
		}
		if ((u8)ch <= 251)	// 111110XXb
		{
			return 5;
		}
		// 1111110Xb
		// The maximum number that may occur is 253(11111101b)
		// FE and FF never appear.
		return 6;
	}

	//! Checks the number of Unicode characters in a null-terminated utf-8 string, 
	//! not including the null terminator.
	inline constexpr usize utf8_strlen(const c8* src)
	{
		usize l{ 0 };
		while (*src)
		{
			src += utf8_charlen(src);
			++l;
		}
		return l;
	}

	//! Returns the position of the first `char` character for the `n`th Unicode character.
	inline constexpr usize utf8_index(const c8* str, usize n)
	{
		usize i{ 0 };
		usize p{ 0 };
		while (str[p] && i < n)
		{
			p += utf8_charlen(str + p);
			++i;
		}
		return p;
	}

	//! Encodes the Unicode character `ch` into 1~6 `char` bytes in the `dest` UTF-8 string.
	//! Returns the number of `char` characters printed to the `dest` buffer.
	LUNA_RUNTIME_API usize utf8_encode_char(c8* dest, c32 ch);

	//! Returns the Unicode character in the UTF-8 string `str`.
	LUNA_RUNTIME_API c32 utf8_decode_char(const c8* str);

	//! Returns the number of `Char16` characters needed to store the Unicode char `ch` in UTF-16 encoding.
	inline constexpr usize utf16_charspan(c32 ch)
	{
		if (ch <= 0xFFFF)
		{
			return 1;
		}
		return 2;
	}

	//! Returns the number of `Char16` characters the next Unicode char takes in the specified UTF-16 string.
	inline constexpr usize utf16_charlen(const c16* src)
	{
		if (src[0] >= 0xD800 && src[0] <= 0xDFFF)
		{
			return 2;
		}
		return 1;
	}

	//! Checks the number of Unicode characters in a null-terminated utf-16 string, 
	//! not including the null terminator.
	inline constexpr usize utf16_strlen(const c16* src)
	{
		usize l{ 0 };
		while (*src)
		{
			src += utf16_charlen(src);
			++l;
		}
		return l;
	}

	//! Returns the position of the first `Char16` character for the `n`th Unicode character.
	inline constexpr usize utf16_index(const c16* str, usize n)
	{
		usize i{ 0 };
		usize p{ 0 };
		while (str[p] && i < n)
		{
			p += utf16_charlen(str + p);
			++i;
		}
		return p;
	}

	//! Encodes the Unicode character `ch` into 1~2 `Char16` bytes in the `dest` UTF-16 string.
	//!  Returns the number of `Char16` characters printed to the `dest` buffer.
	//! @param[in] le If `true`, then the character is encoded using Little Endian, else the character
	//! is encoded using Big Endian.
	LUNA_RUNTIME_API usize utf16_encode_char(c16* dest, c32 ch, bool le = true);

	//! Returns the Unicode character in the UTF-16 string `str`. LE and BE are automatic detected.
	LUNA_RUNTIME_API c32 utf16_decode_char(const c16* str);

	//! Convert a utf-16 string to utf-8 string in destination buffer. LE and BE are automatic detected.
	//! @param[in] dest The buffer to hold the output string.
	//! @param[in] dest_max_chars The maximum characters the `dest` buffer can hold, 
	//! including the null-terminator.
	//! @param[in] src The null-terminated buffer holding the source string.
	//! @param[in] src_chars The maximum characters to read. Specify `USize_max_v` to read till the end of the 
	//! string.
	//! @return Returns the number of characters outputted to the `dest` buffer, 
	//! not including the null-terminator.
	LUNA_RUNTIME_API usize utf16_to_utf8(c8* dest, usize dest_max_chars, const c16* src, usize src_chars = usize_max);

	//! Determines the length of the corresponding utf-8 string for a utf-16 input string,
	//! not include the null-terminator.
	LUNA_RUNTIME_API usize utf16_to_utf8_len(const c16* src, usize src_chars = usize_max);

	//! Convert a utf-8 string to utf-16 string in destination buffer.
	//! @param[in] dest The buffer to hold the output string.
	//! @param[in] dest_max_chars The maximum characters the `dest` buffer can hold, 
	//! including the null-terminator.
	//! @param[in] src The null-terminated buffer holding the source string.
	//! @param[in] le If `true`, then the characters are encoded using Little Endian, else the characters
	//! are encoded using Big Endian.
	//! @param[in] src_chars The maximum characters to read. Specify `USize_max_v` to read till the end of the 
	//! string.
	//! @return Returns the number of characters outputted to the `dest` buffer, 
	//! not including the null-terminator. Returns (usize)-1 if the input string is not
	//! in utf-8 format.
	LUNA_RUNTIME_API usize utf8_to_utf16(c16* dest, usize dest_max_chars, const c8* src, bool le = true, usize src_chars = usize_max);

	//! Determines the length of the corresponding utf-16 string for a utf-8 input string,
	//! not include the null-terminator.
	//! @return Returns length of the `src` string in utf-16 format, not including the null-terminator. 
	//! Returns (usize)-1 if the `src` string is not in utf-8 format.
	LUNA_RUNTIME_API usize utf8_to_utf16_len(const c8* src, usize src_chars = usize_max);
}
