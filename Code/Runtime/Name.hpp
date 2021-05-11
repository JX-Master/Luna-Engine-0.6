// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Name.hpp
* @author JXMaster
* @date 2020/8/7
* @brief Runtime name string APIs.
*/
#pragma once
#include "String.hpp"
#include "Functional.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

namespace Luna
{
	//! Interns one name string to the runtime and fetches the interned address for it.
	//! @param[in] name The name string to intern.
	//! @return Returns the interned address for the name string. If `name` is `nullptr` or points to an empty string (`""`), the returned 
	//! address is `nullptr` and the memory block is not interned.
	//! @remark The name string is saved in the runtime and is reference counted. The first call to `intern_name` with a new string
	//! allocates the memory block to store the string and returns the block address. Additional calls to `intern_name` with the same string
	//! will only increase the reference count of the memory block and returns the same address, so the user can simply compares
	//! two address (pointer) to check if they refer to the same string.
	//! 
	//! For each call to `intern_name`, one call to `release_name` is needed to finally release the internal name string block.
	//! 
	//! For end user, prefer using `Name` class instead of calling these APIs directly.
	LUNA_RUNTIME_API const c8* intern_name(const c8* name);

	//! Same as `intern_name`, but accepts a non-null-terminated string with manually specified length.
	//! @param[in] name The name string to intern.
	//! @param[in] count The number of characters to intern.
	//! @return Returns the string ID for the name string.
	LUNA_RUNTIME_API const c8* intern_name(const c8* name, usize count);

	//! Fetches the string ID of the specified string. For the same name string, the string ID will not change even between multiple processes,
	//! but the name memory block pointer may change.
	//! @param[in] name The name address returned by `intern_name` or `fetch_name`.
	//! @return Returns the string ID of the specified string. The string ID is 0 is `name` is `nullptr`.
	LUNA_RUNTIME_API u64 get_name_id(const c8* name);

	//! Fetches the string entry by its string ID.
	//! @param[in] id The string id returned by `intern_name` for the string.
	//! @return Returns a pointer to the buffer of the string. This pointer is valid before the name is released by calling `release_name`.
	//! Returns `nullptr` if `id` is 0 or does not exist in the runtime.
	LUNA_RUNTIME_API const c8* fetch_name(u64 id);

	//! Increases the reference count of the name string by 1.
	//! @param[in] name The string pointer returned by `intern_name` for the string. If this is `nullptr`, this call does nothing.
	LUNA_RUNTIME_API void retain_name(const c8* name);

	//! Releases one name string entry.
	//! This call decreases the reference count of the name string by 1, and eventually frees the name string when the 
	//! reference count goes to 0.
	//! @param[in] name The string pointer returned by `intern_name` for the string. If this is `nullptr`, this call does nothing.
	LUNA_RUNTIME_API void release_name(const c8* name);

	//! Name represents one string that is used to identify any kind instance in the engine.
	//! The name string is represented by one string id, which is calculated from the string. Any
	//! two different names will have different string IDs, and any two names that is compare-equal will
	//! have the same string ID.
	class Name
	{
	private:
		const c8* m_str;
	public:
		Name() :
			m_str(nullptr) {}
		Name(const c8* name) :
			m_str(intern_name(name)) {}
		Name(const c8* name, usize count) :
			m_str(intern_name(name, count)) {}
		Name(const String& str) :
			m_str(intern_name(str.c_str())) {}
		Name(const String& str, usize pos, usize count) :
			m_str(intern_name(str.c_str() + pos, count)) {}
		~Name()
		{
			release_name(m_str);
		}
		Name(const Name& rhs) :
			m_str(rhs.m_str)
		{
			retain_name(m_str);
		}
		Name(Name&& rhs) :
			m_str(rhs.m_str)
		{
			rhs.m_str = nullptr;
		}
		Name& operator=(const Name& rhs)
		{
			release_name(m_str);
			m_str = rhs.m_str;
			retain_name(m_str);
			return *this;
		}
		Name& operator=(Name&& rhs)
		{
			release_name(m_str);
			m_str = rhs.m_str;
			rhs.m_str = nullptr;
			return *this;
		}
		Name& operator=(const c8* name)
		{
			release_name(m_str);
			m_str = intern_name(name);
			return *this;
		}
		Name& operator=(const String& str)
		{
			release_name(m_str);
			m_str = intern_name(str.c_str());
			return *this;
		}

		u64 id() const
		{
			return get_name_id(m_str);
		}

		const c8* c_str() const
		{
			return m_str;
		}

		bool empty() const
		{
			return m_str == nullptr;
		}

		operator bool() const
		{
			return !empty();
		}

		void clear()
		{
			release_name(m_str);
			m_str = nullptr;
		}

		bool operator==(const Name& rhs) const
		{
			return m_str == rhs.m_str;
		}

		bool operator!=(const Name& rhs) const
		{
			return m_str != rhs.m_str;
		}

		operator String() const
		{
			return String(c_str());
		}
	};

	template <> struct hash<Name>
	{
		usize operator()(const Name& val) const { return reinterpret_cast<usize>(val.c_str()); }
	};
}