// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Unicode.hpp
* @author JXMaster
* @date 2020/2/4
*/
#include "../Unicode.hpp"

namespace luna
{
	size_t utf8_encode_char(char* dest, char32_t ch)
	{
		if (ch <= impl::l_utf8_one_end)
		{
			dest[0] = (char)ch;
			return 1;
		}
		if (ch <= impl::l_utf8_two_end)
		{
			dest[0] = (char)(ch >> 6) + 0xC0;
			dest[1] = (char)(ch & 0x3F) + 0x80;
			return 2;
		}
		if (ch <= impl::l_utf8_three_end)
		{
			dest[0] = (char)(ch >> 12) + 0xE0;
			dest[1] = ((char)(ch >> 6) & 0x3F) + 0x80;
			dest[2] = (char)(ch & 0x3F) + 0x80;
			return 3;
		}
		if (ch <= impl::l_utf8_four_end)
		{
			dest[0] = (char)(ch >> 18) + 0xF0;
			dest[1] = ((char)(ch >> 12) & 0x3F) + 0x80;
			dest[2] = ((char)(ch >> 6) & 0x3F) + 0x80;
			dest[3] = (char)(ch & 0x3F) + 0x80;
			return 4;
		}
		if (ch <= impl::l_utf8_five_end)
		{
			dest[0] = (char)(ch >> 24) + 0xF8;
			dest[1] = ((char)(ch >> 18) & 0x3F) + 0x80;
			dest[2] = ((char)(ch >> 12) & 0x3F) + 0x80;
			dest[3] = ((char)(ch >> 6) & 0x3F) + 0x80;
			dest[4] = (char)(ch & 0x3F) + 0x80;
			return 5;
		}
		// if (ch <= l_utf8_six_end)
		{
			dest[0] = (char)(ch >> 30) + 0xFC;
			dest[1] = ((char)(ch >> 24) & 0x3F) + 0x80;
			dest[2] = ((char)(ch >> 18) & 0x3F) + 0x80;
			dest[3] = ((char)(ch >> 12) & 0x3F) + 0x80;
			dest[4] = ((char)(ch >> 6) & 0x3F) + 0x80;
			dest[5] = (char)(ch & 0x3F) + 0x80;
			return 6;
		}
	}
	char32_t utf8_decode_char(const char* str)
	{
		{
			unsigned char fc = (unsigned char)str[0];
			if (fc <= 127)
			{
				return (char32_t)str[0];
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

	size_t utf16_encode_char(char16_t* dest, char32_t ch, bool le)
	{
		if (ch >= 0xFFFF)
		{
			if (le)
			{
				dest[0] = 0xDC00 + (char16_t)ch & 0x03FF;
				dest[1] = 0xD800 + (char16_t)(ch >> 10) & 0x03FF;
			}
			else
			{
				dest[0] = 0xD800 + (char16_t)(ch >> 10) & 0x03FF;
				dest[1] = 0xDC00 + (char16_t)ch & 0x03FF;
			}
			return 2;
		}
		else
		{
			dest[0] = (char16_t)ch;
			return 1;
		}
	}

	char32_t utf16_decode_char(const char16_t* str)
	{
		// Auto detect LE or BE.
		char16_t fc = *str;
		if (fc >= 0xDC00 && fc <= 0xDFFF)
		{
			// LE
			return (fc - 0xDC00) + (char32_t)((str[1] - 0xD800) << 10);
		}
		if (fc >= 0xD800 && fc <= 0xDBFF)
		{
			// BE
			return (str[1] - 0xDC00) + (char32_t)((fc - 0xD800) << 10);
		}
		return (char32_t)fc;
	}

	size_t utf16_to_utf8(char* dest, size_t dest_max_chars, const char16_t* src, size_t src_chars)
	{
		size_t ri{ 0 };
		size_t wi{ 0 };
		while ((src[ri]) && (ri < src_chars))
		{
			char32_t ch = utf16_decode_char(src + ri);
			size_t num_chars = utf8_charspan(ch);
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

	size_t utf16_to_utf8_len(const char16_t* src, size_t src_chars)
	{
		size_t ri{ 0 };
		size_t wi{ 0 };
		while (src[ri] && (ri < src_chars))
		{
			char32_t ch = utf16_decode_char(src + ri);
			ri += utf16_charspan(ch);
			wi += utf8_charspan(ch);
		}
		return wi;
	}

	size_t utf8_to_utf16(char16_t* dest, size_t dest_max_chars, const char* src, bool le, size_t src_chars)
	{
		size_t ri{ 0 };
		size_t wi{ 0 };
		while ((src[ri]) && (ri < src_chars))
		{
			char32_t ch = utf8_decode_char(src + ri);
			size_t num_chars = utf16_charspan(ch);
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

	size_t utf8_to_utf16_len(const char* src, size_t src_max_chars)
	{
		size_t ri{ 0 };
		size_t wi{ 0 };
		while (src[ri] && (ri < src_max_chars))
		{
			char32_t ch = utf8_decode_char(src + ri);
			ri += utf8_charspan(ch);
			wi += utf16_charspan(ch);
		}
		return wi;
	}
}