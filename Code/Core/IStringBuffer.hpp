// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IStringBuffer.hpp
* @author JXMaster
* @date 2019/11/15
*/
#pragma once
#include "IString.hpp"
#include <Base/ICloneable.hpp>
#include <Base/Interface.hpp>
namespace luna
{
	//! @interface IStringBuffer
	//! IStringBuffer represents a dynamic sized mutable string. Unlike `IString`, the string buffer is not thread safe,
	//! its underlying memory buffer may be relocated as the string are appended.
	//! 
	//! The string buffer itself cannot be used as `IString`, it must be converted to a `IString` object in order to use it as 
	//! a string. There are two ways to convert a string buffer to a string, the first way is calling `new_string` using the data
	//! stored in this buffer, this way a new string object is created and the data is copied to the new string object, while the 
	//! buffer still holds the string data. Another way is to call `promote` of this object to transfer the data of this buffer to 
	//! a new string object, the buffer then is empty. The second way is efficient since there is no memory allocation and data copy,
	//! only the ownership of the string memory is transfered from buffer to string object.
	struct IStringBuffer : public IString, public ICloneable
	{
		luiid("{a3e4fb5e-6953-4f13-abf5-111edd1278d1}");
		luiprimary(IString);

		//! Discards the original data of this string buffer and assigns a new UTF-8 string to this data.
		//! @param[in] s The string to assign.
		//! @param[in] len The number of `char` characters to assign. Specify `size_t_max` to use all characters.
		virtual void assign(const char* s, size_t len = size_t_max_v) = 0;

		//! Gets a pointer to the string buffer so the user can modify the content in the buffer.
		//! For UTF-8 cases, you should prefer `replace` instead of writing to characters directly, since one UTF-8 
		//! character may be represented by multiple characters in the buffer.
		//! The returned pointer will not be `nullptr` even if the string buffer is empty. If the string is empty, the 
		//! first character in the returned buffer is '\0'.
		virtual char* data() = 0;

		//! Checks if this string buffer is empty, that is, contains no characters.
		virtual bool empty() = 0;

		//! Reserves the internal memory space so that it can hold at least `new_cap` `char` characters.
		virtual void reserve(size_t new_cap) = 0;
		//! Gets the number of `char` characters the internal buffer can hold without expanding.
		virtual size_t capacity() = 0;
		//! Reduces `capacity` to `size`, removes all unused memory.
		virtual void shrink_to_fit() = 0;

		//! Clears all characters in this buffer.
		virtual void clear() = 0;

		//! Inserts a new string into the specified position of this buffer.
		//! @param[in] pos The place to insert the new string to. The first character will be inserted as
		//! the `pos`th `char` character in the buffer.
		//! @param[in] s The UTf-8 string to insert to.
		//! @param[in] len The number of `char` characters to insert. Specify `size_t_max` to use all characters.
		virtual void insert(size_t pos, const char* s, size_t len = size_t_max_v) = 0;
		virtual void insert_char(size_t pos, size_t count, char ch) = 0;

		//! Erases `count` `char` characters starts from `pos`'th `char` character from the buffer.
		//! @param[in] pos The position of the first `char` character to erase.
		//! @param[in] count The number of `char` characters to erase.
		virtual void erase(size_t pos = 0, size_t count = size_t_max_v) = 0;

		//! Pushes one `char character to the back of the string.
		//! @param[in] ch The character to push.
		virtual void push_back(char ch) = 0;
		//! Pops one `char` character from the back of the string.
		virtual void pop_back() = 0;

		//! Appends one string to the end of the string.
		virtual void append(const char* s, size_t len = size_t_max_v) = 0;
		virtual void append_char(size_t count, char ch) = 0;

		//! Replace parts of the string with the string provided.
		//! @param[in] pos The index of the first `char` character to be replaced.
		//! @param[in] count Number of `char` characters to be replaced.
		//! @param[in] s The string used for replacement.
		//! @param[in] len Number of `char` characters to be used in the string `s`. Specify `size_t_max`
		//! to use the full string.
		virtual void replace(size_t pos, size_t count, const char* s, size_t len = size_t_max_v) = 0;
		virtual void replace_char(size_t pos, size_t count, size_t count2, char ch) = 0;

		//! Creates a new string buffer by copying parts of this string.
		//! @param[in] pos The index of the first `char` character to be copied.
		//! @param[in] count The number of `char` characters to be copied.
		virtual P<IStringBuffer> substr(size_t pos, size_t count = size_t_max_v) = 0;

		//! Resizes the string to contain `count` characters.
		//! If the current size is less than `count`, additional characters are appended.
		//! If the current size is greater than `count`, the string is reduced to its first count elements.
		virtual void resize(size_t count, char ch = char()) = 0;

		//! Swaps the data of this string buffer with the data in the `rhs` string buffer.
		virtual void swap(IStringBuffer* rhs) = 0;
	};
}