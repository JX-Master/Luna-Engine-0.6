// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Unicode.hpp
* @author JXMaster
* @date 2020/2/4
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "../Unicode.hpp"

namespace Luna
{
	LUNA_RUNTIME_API usize utf8_encode_char(c8* dest, c32 ch)
	{
		if (ch <= impl::l_utf8_one_end)
		{
			dest[0] = (c8)ch;
			return 1;
		}
		if (ch <= impl::l_utf8_two_end)
		{
			dest[0] = (c8)(ch >> 6) + 0xC0;
			dest[1] = (c8)(ch & 0x3F) + 0x80;
			return 2;
		}
		if (ch <= impl::l_utf8_three_end)
		{
			dest[0] = (c8)(ch >> 12) + 0xE0;
			dest[1] = ((c8)(ch >> 6) & 0x3F) + 0x80;
			dest[2] = (c8)(ch & 0x3F) + 0x80;
			return 3;
		}
		if (ch <= impl::l_utf8_four_end)
		{
			dest[0] = (c8)(ch >> 18) + 0xF0;
			dest[1] = ((c8)(ch >> 12) & 0x3F) + 0x80;
			dest[2] = ((c8)(ch >> 6) & 0x3F) + 0x80;
			dest[3] = (c8)(ch & 0x3F) + 0x80;
			return 4;
		}
		if (ch <= impl::l_utf8_five_end)
		{
			dest[0] = (c8)(ch >> 24) + 0xF8;
			dest[1] = ((c8)(ch >> 18) & 0x3F) + 0x80;
			dest[2] = ((c8)(ch >> 12) & 0x3F) + 0x80;
			dest[3] = ((c8)(ch >> 6) & 0x3F) + 0x80;
			dest[4] = (c8)(ch & 0x3F) + 0x80;
			return 5;
		}
		// if (ch <= l_utf8_six_end)
		{
			dest[0] = (c8)(ch >> 30) + 0xFC;
			dest[1] = ((c8)(ch >> 24) & 0x3F) + 0x80;
			dest[2] = ((c8)(ch >> 18) & 0x3F) + 0x80;
			dest[3] = ((c8)(ch >> 12) & 0x3F) + 0x80;
			dest[4] = ((c8)(ch >> 6) & 0x3F) + 0x80;
			dest[5] = (c8)(ch & 0x3F) + 0x80;
			return 6;
		}
	}
	LUNA_RUNTIME_API c32 utf8_decode_char(const c8* str)
	{
		{
			u8 fc = (u8)str[0];
			if (fc <= 127)
			{
				return (c32)str[0];
			}
			if (fc <= 223)
			{
				return ((str[0] & 0x1F) << 6) + (str[1] & 0x3F);
			}
			if (fc <= 239)
			{
				return ((str[0] & 0x0F) << 12) + ((str[1] & 0x3F) << 6) + (str[2] & 0x3F);
			}
			if (fc <= 247)
			{
				return ((str[0] & 0x07) << 18) + ((str[1] & 0x3F) << 12) + ((str[2] & 0x3F) << 6) + (str[3] & 0x3F);
			}
			if (fc <= 251)
			{
				return ((str[0] & 0x03) << 24) + ((str[1] & 0x3F) << 18) + ((str[2] & 0x3F) << 12) + ((str[3] & 0x3F) << 6) + (str[4] & 0x3F);
			}
			return ((str[0] & 0x01) << 30) + ((str[1] & 0x3F) << 24) + ((str[2] & 0x3F) << 18) + ((str[3] & 0x3F) << 12) + ((str[4] & 0x3F) << 6) + (str[5] & 0x3F);
		}
	}

	LUNA_RUNTIME_API usize utf16_encode_char(c16* dest, c32 ch, bool le)
	{
		if (ch >= 0xFFFF)
		{
			if (le)
			{
				dest[0] = 0xDC00 + (c16)ch & 0x03FF;
				dest[1] = 0xD800 + (c16)(ch >> 10) & 0x03FF;
			}
			else
			{
				dest[0] = 0xD800 + (c16)(ch >> 10) & 0x03FF;
				dest[1] = 0xDC00 + (c16)ch & 0x03FF;
			}
			return 2;
		}
		else
		{
			dest[0] = (c16)ch;
			return 1;
		}
	}

	LUNA_RUNTIME_API c32 utf16_decode_char(const c16* str)
	{
		// Auto detect LE or BE.
		c16 fc = *str;
		if (fc >= 0xDC00 && fc <= 0xDFFF)
		{
			// LE
			return (fc - 0xDC00) + (c32)((str[1] - 0xD800) << 10);
		}
		if (fc >= 0xD800 && fc <= 0xDBFF)
		{
			// BE
			return (str[1] - 0xDC00) + (c32)((fc - 0xD800) << 10);
		}
		return (c32)fc;
	}

	LUNA_RUNTIME_API usize utf16_to_utf8(c8* dest, usize dest_max_chars, const c16* src, usize src_chars)
	{
		usize ri{ 0 };
		usize wi{ 0 };
		while ((src[ri]) && (ri < src_chars))
		{
			c32 ch = utf16_decode_char(src + ri);
			usize num_chars = utf8_charspan(ch);
			if (wi + num_chars > dest_max_chars - 1)
			{
				break;
			}
			ri += utf16_charspan(ch);
			wi += utf8_encode_char(dest + wi, ch);
		}
		dest[wi] = '\0';
		return wi;
	}

	LUNA_RUNTIME_API usize utf16_to_utf8_len(const c16* src, usize src_chars)
	{
		usize ri{ 0 };
		usize wi{ 0 };
		while (src[ri] && (ri < src_chars))
		{
			c32 ch = utf16_decode_char(src + ri);
			ri += utf16_charspan(ch);
			wi += utf8_charspan(ch);
		}
		return wi;
	}

	LUNA_RUNTIME_API usize utf8_to_utf16(c16* dest, usize dest_max_chars, const c8* src, bool le, usize src_chars)
	{
		usize ri{ 0 };
		usize wi{ 0 };
		while ((src[ri]) && (ri < src_chars))
		{
			c32 ch = utf8_decode_char(src + ri);
			usize num_chars = utf16_charspan(ch);
			if (wi + num_chars >= dest_max_chars)
			{
				break;
			}
			ri += utf8_charspan(ch);
			wi += utf16_encode_char(dest + wi, ch, le);
		}
		dest[wi] = '\0';
		return wi;
	}

	LUNA_RUNTIME_API usize utf8_to_utf16_len(const c8* src, usize src_max_chars)
	{
		usize ri{ 0 };
		usize wi{ 0 };
		while (src[ri] && (ri < src_max_chars))
		{
			c32 ch = utf8_decode_char(src + ri);
			ri += utf8_charspan(ch);
			wi += utf16_charspan(ch);
		}
		return wi;
	}
}