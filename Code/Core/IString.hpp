// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IString.hpp
* @author JXMaster
* @date 2019/3/4
* @brief IString interface represents a reference-counted null-terminated blocks of memory.
 */
#pragma once
#include <Base/IObject.hpp>

namespace luna
{
	//! @interface IString
	//! `IString` interface represents a reference-counted, null-terminated UTF-8 string.
	//! 
	//! You can create an immutable string by calling `new_string` function, or a mutable
	//! string by calling `new_string_buffer` function.
	struct IString : public IObject
	{
		luiid("{141ca7ce-9b3b-4fa3-93f9-ec5efa81e91f}");

		//! Gets the C-style buffer of the string. The buffer should be valid so long as the data of `IString` is not changed.
		//! It is suggested that you call this method again every time you want to use the pointer.
		virtual const char* c_str() = 0;
		//! Gets the number of `char` elements in this string, excluding the null terminator.
		//! The returned number is NOT the number of Unicode characters in this string, use `strlen_utf8` to fetch that.
		virtual size_t size() = 0;
	};
}