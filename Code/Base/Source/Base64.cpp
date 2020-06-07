// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Base64.cpp
* @author JXMaster
* @date 2020/2/4
*/
#include "../Base64.hpp"

namespace luna
{
	namespace impl
	{
		constexpr char base64_encode_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		constexpr bool base64_is_valid_char(char c)
		{
			return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '+' || c == '/') ? true : false;
		}

		constexpr uint8 base64_decode_char(char c)
		{
			if (c >= 'A' && c <= 'Z')
			{
				return c - 'A';
			}
			if (c >= 'a' && c <= 'z')
			{
				return c - 'a' + 26;
			}
			if (c >= '0' && c <= '9')
			{
				return c - '0' + 52;
			}
			if (c == '+')
			{
				return 62;
			}
			if (c == '/')
			{
				return 63;
			}
			// Should not be here.
			return 0;
		}
	}


	size_t base64_encode(char* dest, size_t dest_max_chars, const void* src, size_t src_size_bytes)
	{
		uint8 data_tuple[3];
		uint8 str_tuple[4];
		uint8 padding;
		size_t data_cur{ 0 };
		size_t str_cur{ 0 };
		const uint8* data_buf = reinterpret_cast<const uint8*>(src);
		while (data_cur < src_size_bytes)
		{
			// fetch data.
			data_tuple[0] = data_buf[data_cur];
			padding = (src_size_bytes - data_cur) > 2 ? 0 : (uint8)(3 + data_cur - src_size_bytes);
			data_tuple[1] = (padding == 2) ? 0 : data_buf[data_cur + 1];
			data_tuple[2] = (padding > 0) ? 0 : data_buf[data_cur + 2];
			// calculate
			str_tuple[0] = (data_tuple[0] & 0xfc) >> 2;
			str_tuple[1] = ((data_tuple[0] & 0x03) << 4) + ((data_tuple[1] & 0xf0) >> 4);
			str_tuple[2] = ((data_tuple[1] & 0x0f) << 2) + ((data_tuple[2] & 0xc0) >> 6);
			str_tuple[3] = data_tuple[2] & 0x3f;
			// encode
			if (str_cur < dest_max_chars)
			{
				dest[str_cur] = impl::base64_encode_chars[str_tuple[0]];
				str_cur++;
			}
			if (str_cur < dest_max_chars)
			{
				dest[str_cur] = impl::base64_encode_chars[str_tuple[1]];
				str_cur++;
			}
			if (str_cur < dest_max_chars)
			{
				dest[str_cur] = (padding == 2) ? '=' : impl::base64_encode_chars[str_tuple[2]];
				str_cur++;
			}
			if (str_cur < dest_max_chars)
			{
				dest[str_cur] = (padding > 0) ? '=' : impl::base64_encode_chars[str_tuple[3]];
				str_cur++;
			}
			data_cur += 3;
			if (str_cur >= dest_max_chars)
			{
				break;
			}
		}
		dest[str_cur] = 0;
		return str_cur;
	}

	size_t base64_decode(void* dest, size_t dest_max_bytes, const char* src, size_t src_size_chars)
	{
		uint8 str_tuple[4];
		uint8 padding;
		size_t data_cur{ 0 };
		size_t str_cur{ 0 };
		uint8* data_t = reinterpret_cast<uint8*>(dest);
		while ((data_cur < dest_max_bytes) && src[str_cur] && (str_cur < src_size_chars))
		{
			// decode
			padding = src[str_cur + 2] == '=' ? 2 : (src[str_cur + 3] == '=' ? 1 : 0);
			str_tuple[0] = impl::base64_decode_char(src[str_cur]);
			str_tuple[1] = impl::base64_decode_char(src[str_cur + 1]);
			str_tuple[2] = (padding == 2) ? 0 : impl::base64_decode_char(src[str_cur + 2]);
			str_tuple[3] = padding ? 0 : impl::base64_decode_char(src[str_cur + 3]);
			// construct data.
			data_t[data_cur] = (str_tuple[0] << 2) + ((str_tuple[1] & 0x30) >> 4);
			data_cur++;
			if (data_cur < dest_max_bytes && padding != 2)
			{
				data_t[data_cur] = ((str_tuple[1] & 0x0f) << 4) + ((str_tuple[2] & 0x3c) >> 2);
				data_cur++;
			}
			if (data_cur < dest_max_bytes && !padding)
			{
				data_t[data_cur] = ((str_tuple[2] & 0x03) << 6) + str_tuple[3];
				data_cur++;
			}
			str_cur += 4;
			if (padding)
			{
				break;
			}
		}
		return data_cur;
	}
}