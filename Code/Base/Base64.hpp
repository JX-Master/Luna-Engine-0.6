// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Base64.hpp
* @author JXMaster
* @date 2019/8/2
*/
#pragma once
#include "BaseDefines.hpp"

namespace luna
{
	//! Get the encoded base64 string size from the raw data size.
	//! The string size does not include the null-terminator.
	//! @param[in] raw_size The size of the raw binary data in bytes.
	//! @return The size of the encoded string in bytes.
	constexpr size_t base64_get_encoded_size(size_t raw_size)
	{
		return (raw_size + 2) / 3 * 4;
	}

	//! Get the decoded binary size from the encoded base64 string size.
	//! This value may be greater than the actual data size if the size of raw data is not times of 3.
	//! You should use this value to allocate memory, and use the size value returned by the actual decode 
	//! function as the size information, since that is precise.
	//! @param[in] encoded_size The size of the encoded string, in bytes, and not including the null
	//! terminator.
	//! @return The size of the decoded raw data, in bytes.
	constexpr size_t base64_get_decoded_size(size_t encoded_size)
	{
		return encoded_size / 4 * 3;
	}

	//! Encode a binary data to a base64 string.
	//! @param[in] dest The character buffer used to hold the encoded base64 string.
	//! @param[in] dest_max_chars The maximum characters the `dest` buffer can hold, including the null-terminator.
	//! @param[in] src The source binary data.
	//! @param[in] src_size_bytes The size of the source binary data in bytes.
	//! @return Returned the number of characters outputted into `dest` buffer.
	size_t base64_encode(char* dest, size_t dest_max_chars, const void* src, size_t src_size_bytes);

	//! Decode a base64 string to binary data. The system assumes the string passed in is a valid base64 string.
	//! @param[in] dest The binary buffer used to hold the decoded data.
	//! @param[in] dest_max_bytes The maximum bytes the `dest` buffer can hold.
	//! @param[in] src The null-terminated base64 source string.
	//! @param[in] src_size_chars The maximum characters to read in the `src` string. Specify `size_t_max_v` to read
	//! the full string.
	//! @return Returns the number of bytes decoded into the `dest` buffer.
	size_t base64_decode(void* dest, size_t dest_max_bytes, const char* src, size_t src_size_chars = size_t_max_v);


}