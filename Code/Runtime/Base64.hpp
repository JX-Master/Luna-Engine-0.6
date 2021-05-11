// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Base64.hpp
* @author JXMaster
* @date 2019/8/2
*/
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! Get the encoded base64 string size from the raw data size.
	//! The string size does not include the null-terminator.
	//! @param[in] raw_size The size of the raw binary data in bytes.
	//! @return The size of the encoded string in bytes.
	constexpr usize base64_get_encoded_size(usize raw_size)
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
	constexpr usize base64_get_decoded_size(usize encoded_size)
	{
		return encoded_size / 4 * 3;
	}

	//! Encode a binary data to a base64 string.
	//! @param[in] dest The character buffer used to hold the encoded base64 string.
	//! @param[in] dest_max_chars The maximum characters the `dest` buffer can hold, including the null-terminator.
	//! @param[in] src The source binary data.
	//! @param[in] src_size_bytes The size of the source binary data in bytes.
	//! @return Returned the number of characters outputted into `dest` buffer.
	LUNA_RUNTIME_API usize base64_encode(c8* dest, usize dest_max_chars, const void* src, usize src_size_bytes);

	//! Decode a base64 string to binary data. The system assumes the string passed in is a valid base64 string.
	//! @param[in] dest The binary buffer used to hold the decoded data.
	//! @param[in] dest_max_bytes The maximum bytes the `dest` buffer can hold.
	//! @param[in] src The null-terminated base64 source string.
	//! @param[in] src_size_chars The maximum characters to read in the `src` string. Specify `usize_max` to read
	//! the full string.
	//! @return Returns the number of bytes decoded into the `dest` buffer.
	LUNA_RUNTIME_API usize base64_decode(void* dest, usize dest_max_bytes, const c8* src, usize src_size_chars = usize_max);
}