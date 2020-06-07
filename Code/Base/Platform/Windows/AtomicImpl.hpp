// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Atomic.inl
* @author JXMaster
* @date 2018/12/7
 */
#pragma once
#include "../../BaseDefines.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include "MiniWin.hpp"

namespace luna
{
	inline int32 atom_inc_i32(int32 volatile* v)
	{
		return InterlockedIncrement((LONG volatile *)v);
	}
	inline uint32 atom_inc_u32(uint32 volatile* v)
	{
		return InterlockedIncrement((LONG volatile *)v);
	}
	inline int64 atom_inc_i64(int64 volatile* v)
	{
		return InterlockedIncrement64((LONGLONG volatile *)v);
	}
	inline uint64 atom_inc_u64(uint64 volatile* v)
	{
		return InterlockedIncrement64((LONGLONG volatile *)v);
	}

	inline int32 atom_dec_i32(int32 volatile* v)
	{
		return InterlockedDecrement((LONG volatile *)v);
	}
	inline uint32 atom_dec_u32(uint32 volatile* v)
	{
		return InterlockedDecrement((LONG volatile *)v);
	}
	inline int64 atom_dec_i64(int64 volatile* v)
	{
		return InterlockedDecrement64((LONGLONG volatile *)v);
	}
	inline uint64 atom_dec_u64(uint64 volatile* v)
	{
		return InterlockedDecrement64((LONGLONG volatile *)v);
	}

	inline int32 atom_add_i32(int32 volatile* base, int32 v)
	{
		return InterlockedExchangeAdd((LONG volatile*)base, (LONG)v);
	}
	inline uint32 atom_add_u32(uint32 volatile* base, int32 v)
	{
		return InterlockedExchangeAdd((LONG volatile*)base, (LONG)v);
	}
	inline int64 atom_add_i64(int64 volatile* base, int64 v)
	{
		return InterlockedExchangeAdd64((LONGLONG volatile*)base, (LONGLONG)v);
	}
	inline uint64 atom_add_u64(uint64 volatile* base, int64 v)
	{
		return InterlockedExchangeAdd64((LONGLONG volatile*)base, (LONGLONG)v);
	}

	inline int32 atom_exchange_i32(int32 volatile* dest, int32 v)
	{
		return InterlockedExchange((LONG volatile*)dest, (LONG)v);
	}
	inline uint32 atom_exchange_u32(uint32 volatile* dest, uint32 v)
	{
		return InterlockedExchange((LONG volatile*)dest, (LONG)v);
	}
	inline int64 atom_exchange_i64(int64 volatile* dest, int64 v)
	{
		return InterlockedExchange64((LONGLONG volatile*)dest, (LONGLONG)v);
	}
	inline uint64 atom_exchange_u64(uint64 volatile* dest, uint64 v)
	{
		return InterlockedExchange64((LONGLONG volatile*)dest, (LONGLONG)v);
	}
	inline void* atom_exchange_pointer(void* volatile* target, void* value)
	{
		return (void*)InterlockedExchangePointer((PVOID volatile *)target, (PVOID)value);
	}

	inline int32 atom_compare_exchange_i32(int32 volatile* dest, int32 exchange, int32 comperand)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)exchange, (LONG)comperand);
	}
	inline uint32 atom_compare_exchange_u32(uint32 volatile* dest, uint32 exchange, uint32 comperand)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)exchange, (LONG)comperand);
	}
	inline void* atom_compare_exchange_pointer(void* volatile* dest, void* exchange, void* comperand)
	{
		return (void*)InterlockedCompareExchangePointer((volatile PVOID*)dest, (PVOID)exchange, (PVOID)comperand);
	}

#ifdef LUNA_PLATFORM_64BIT
	//! Atomically compare the value of the variable with the value provided by `comperand`. This operation cannot be interrupted by system thread switching.
	//! If the value of the variable equals to the value provided by `comperand`, the value of the variable will be replaced by the value provided by `exchange`, and the old value of the variable will be returned.
	//! If the value of the variable does not equal to the value provided by `comperand`, the value of the variable will not change, and this value it will be returned.
	inline int64 atom_compare_exchange_i64(int64 volatile* dest, int64 exchange, int64 comperand)
	{
		return InterlockedCompareExchange64((LONGLONG volatile*)dest, (LONGLONG)exchange, (LONGLONG)comperand);
	}
	inline uint64 atom_compare_exchange_u64(uint64 volatile* dest, uint64 exchange, uint64 comperand)
	{
		return InterlockedCompareExchange64((LONGLONG volatile*)dest, (LONGLONG)exchange, (LONGLONG)comperand);
	}
#endif

#ifdef LUNA_PLATFORM_64BIT
	inline size_t atom_inc_sizet(size_t volatile* v)
	{
		return InterlockedIncrement64((__int64 volatile*)v);
	}
	inline size_t atom_dec_sizet(size_t volatile* v)
	{
		return InterlockedDecrement64((__int64 volatile*)v);
	}
	inline size_t atom_add_sizet(size_t volatile* base, ptrdiff_t v)
	{
		return InterlockedAdd64((__int64 volatile*)base, v);
	}
	inline size_t atom_exchange_sizet(size_t volatile* dest, size_t v)
	{
		return InterlockedExchange64((__int64 volatile*)dest, v);
	}
	inline size_t atom_compare_exchange_sizet(size_t volatile* dest, size_t exchange, size_t comperand)
	{
		return InterlockedCompareExchange64((__int64 volatile*)dest, (__int64)exchange, (__int64)comperand);
	}
#else
	inline size_t atom_inc_sizet(size_t volatile* v)
	{
		return InterlockedIncrement((LONG volatile*)v);
	}
	inline size_t atom_dec_sizet(size_t volatile* v)
	{
		return InterlockedDecrement((LONG volatile*)v);
	}
	inline size_t atom_add_sizet(size_t volatile* base, ptrdiff_t v)
	{
		return InterlockedAdd((LONG volatile*)base, (LONG)v);
	}
	inline size_t atom_exchange_sizet(size_t volatile* dest, size_t v)
	{
		return InterlockedExchange((LONG volatile*)dest, (LONG)v);
	}
	inline size_t atom_compare_exchange_sizet(size_t volatile* dest, size_t exchange, size_t comperand)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)exchange, (LONG)comperand);
	}
#endif

}
#endif