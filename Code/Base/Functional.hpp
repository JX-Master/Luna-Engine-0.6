// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Functional.hpp
* @author JXMaster
* @date 2020/2/14
*/
#pragma once
#include "Hash.hpp"
#include "Memory.hpp"
namespace luna
{
	template <typename _Ty>
	struct EqualTo
	{
		constexpr bool operator()(const _Ty& lhs, const _Ty& rhs) const
		{
			return lhs == rhs;
		}
	};

	template <typename _Ty>
	struct Less
	{
		constexpr bool operator()(const _Ty& lhs, const _Ty& rhs) const
		{
			return lhs < rhs;
		}
	};

	//! Hash class is a function object that hashes the specified type into a size_t 
	//! hash code that can be used in hash map and hash set.
	template <typename _Ty> struct Hash;

	template <> struct Hash<bool>
	{
		size_t operator()(bool val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<char16_t>
	{
		size_t operator()(char16_t val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<char32_t>
	{
		size_t operator()(char32_t val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<wchar_t>
	{
		size_t operator()(wchar_t val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<uint8>
	{
		size_t operator()(uint8 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<int8>
	{
		size_t operator()(int8 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<uint16>
	{
		size_t operator()(uint16 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<int16>
	{
		size_t operator()(int16 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<uint32>
	{
		size_t operator()(uint32 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<int32>
	{
		size_t operator()(int32 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<uint64>
	{
		size_t operator()(uint64 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<int64>
	{
		size_t operator()(int64 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<float32>
	{
		size_t operator()(float32 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<float64>
	{
		size_t operator()(float64 val) const { return static_cast<size_t>(val); }
	};

	template <> struct Hash<char*>
	{
		size_t operator()(const char* p) const
		{
			return strhash<size_t>(p);
		}
	};

	template <> struct Hash<const char*>
	{
		size_t operator()(const char* p) const
		{
			return strhash<size_t>(p);
		}
	};

	template <> struct Hash<Guid>
	{
		size_t operator()(const Guid& guid) const
		{
#ifdef LUNA_PLATFORM_64BIT
			return (size_t)(guid.low ^ guid.high);
#else
			return (size_t)((uint32)guid.low ^ ((uint32)(guid.low >> 32)) ^ (uint32)guid.high ^ ((uint32)(guid.high >> 32)));
#endif
		}
	};

	template <typename _Ty>
	class ReferenceWrapper
	{
	private:
		_Ty* m_val;
	public:
		using type = _Ty;
		
		ReferenceWrapper(_Ty& v) :
			m_val(addressof(v)) {}
		ReferenceWrapper(_Ty&&) = delete;
		ReferenceWrapper(const ReferenceWrapper<_Ty>& v) :
			m_val(v.m_val) {}
		ReferenceWrapper& operator=(const ReferenceWrapper<_Ty>& v)
		{
			m_val = v.m_val;
			return *this;
		}
		operator _Ty& () const
		{
			return *m_val;
		}
		_Ty& get() const
		{
			return *m_val;
		}
	};
}