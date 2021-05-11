// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Functional.hpp
* @author JXMaster
* @date 2020/2/14
*/
#pragma once
#include "Hash.hpp"
#include "MemoryUtils.hpp"
namespace Luna
{
	template <typename _Ty>
	struct equal_to
	{
		constexpr bool operator()(const _Ty& lhs, const _Ty& rhs) const
		{
			return lhs == rhs;
		}
	};

	template <typename _Ty>
	struct less
	{
		constexpr bool operator()(const _Ty& lhs, const _Ty& rhs) const
		{
			return lhs < rhs;
		}
	};

	//! hash class is a function object that hashes the specified type into a usize 
	//! hash code that can be used in hash map and hash set.
	template <typename _Ty> struct hash;

	template <> struct hash<bool>
	{
		usize operator()(bool val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<char16_t>
	{
		usize operator()(char16_t val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<char32_t>
	{
		usize operator()(char32_t val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<wchar_t>
	{
		usize operator()(wchar_t val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<u8>
	{
		usize operator()(u8 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<i8>
	{
		usize operator()(i8 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<u16>
	{
		usize operator()(u16 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<i16>
	{
		usize operator()(i16 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<u32>
	{
		usize operator()(u32 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<i32>
	{
		usize operator()(i32 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<u64>
	{
		usize operator()(u64 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<i64>
	{
		usize operator()(i64 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<f32>
	{
		usize operator()(f32 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<f64>
	{
		usize operator()(f64 val) const { return static_cast<usize>(val); }
	};

	template <> struct hash<c8*>
	{
		usize operator()(const c8* p) const
		{
			return strhash<usize>(p);
		}
	};

	template <> struct hash<const c8*>
	{
		usize operator()(const c8* p) const
		{
			return strhash<usize>(p);
		}
	};

	template <> struct hash<Guid>
	{
		usize operator()(const Guid& guid) const
		{
#ifdef LUNA_PLATFORM_64BIT
			return (usize)(guid.low ^ guid.high);
#else
			return (usize)((u32)guid.low ^ ((u32)(guid.low >> 32)) ^ (u32)guid.high ^ ((u32)(guid.high >> 32)));
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
