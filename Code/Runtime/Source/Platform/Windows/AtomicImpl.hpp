// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Atomic.inl
* @author JXMaster
* @date 2018/12/7
 */
#pragma once
#include "../../../Base.hpp"

#include <atomic>

#ifdef LUNA_PLATFORM_WINDOWS
#include "MiniWin.hpp"

namespace Luna
{
	inline i32 atom_inc_i32(i32 volatile* v)
	{
		return InterlockedIncrement((LONG volatile *)v);
	}
	inline u32 atom_inc_u32(u32 volatile* v)
	{
		return InterlockedIncrement((LONG volatile *)v);
	}
	inline i64 atom_inc_i64(i64 volatile* v)
	{
		return InterlockedIncrement64((LONGLONG volatile *)v);
	}
	inline u64 atom_inc_u64(u64 volatile* v)
	{
		return InterlockedIncrement64((LONGLONG volatile *)v);
	}

	inline i32 atom_dec_i32(i32 volatile* v)
	{
		return InterlockedDecrement((LONG volatile *)v);
	}
	inline u32 atom_dec_u32(u32 volatile* v)
	{
		return InterlockedDecrement((LONG volatile *)v);
	}
	inline i64 atom_dec_i64(i64 volatile* v)
	{
		return InterlockedDecrement64((LONGLONG volatile *)v);
	}
	inline u64 atom_dec_u64(u64 volatile* v)
	{
		return InterlockedDecrement64((LONGLONG volatile *)v);
	}

	inline i32 atom_add_i32(i32 volatile* base, i32 v)
	{
		return InterlockedExchangeAdd((LONG volatile*)base, (LONG)v);
	}
	inline u32 atom_add_u32(u32 volatile* base, i32 v)
	{
		return InterlockedExchangeAdd((LONG volatile*)base, (LONG)v);
	}
	inline i64 atom_add_i64(i64 volatile* base, i64 v)
	{
		return InterlockedExchangeAdd64((LONGLONG volatile*)base, (LONGLONG)v);
	}
	inline u64 atom_add_u64(u64 volatile* base, i64 v)
	{
		return InterlockedExchangeAdd64((LONGLONG volatile*)base, (LONGLONG)v);
	}

	inline i32 atom_exchange_i32(i32 volatile* dest, i32 v)
	{
		return InterlockedExchange((LONG volatile*)dest, (LONG)v);
	}
	inline u32 atom_exchange_u32(u32 volatile* dest, u32 v)
	{
		return InterlockedExchange((LONG volatile*)dest, (LONG)v);
	}
	inline i64 atom_exchange_i64(i64 volatile* dest, i64 v)
	{
		return InterlockedExchange64((LONGLONG volatile*)dest, (LONGLONG)v);
	}
	inline u64 atom_exchange_u64(u64 volatile* dest, u64 v)
	{
		return InterlockedExchange64((LONGLONG volatile*)dest, (LONGLONG)v);
	}
	inline void* atom_exchange_pointer(void* volatile* target, void* value)
	{
		return (void*)InterlockedExchangePointer((PVOID volatile *)target, (PVOID)value);
	}

	inline i32 atom_compare_exchange_i32(i32 volatile* dest, i32 exchange, i32 comperand)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)exchange, (LONG)comperand);
	}
	inline u32 atom_compare_exchange_u32(u32 volatile* dest, u32 exchange, u32 comperand)
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
	inline i64 atom_compare_exchange_i64(i64 volatile* dest, i64 exchange, i64 comperand)
	{
		return InterlockedCompareExchange64((LONGLONG volatile*)dest, (LONGLONG)exchange, (LONGLONG)comperand);
	}
	inline u64 atom_compare_exchange_u64(u64 volatile* dest, u64 exchange, u64 comperand)
	{
		return InterlockedCompareExchange64((LONGLONG volatile*)dest, (LONGLONG)exchange, (LONGLONG)comperand);
	}
#endif

#ifdef LUNA_PLATFORM_64BIT
	inline usize atom_inc_usize(usize volatile* v)
	{
		return InterlockedIncrement64((__int64 volatile*)v);
	}
	inline usize atom_dec_usize(usize volatile* v)
	{
		return InterlockedDecrement64((__int64 volatile*)v);
	}
	inline usize atom_add_usize(usize volatile* base, isize v)
	{
		return InterlockedAdd64((__int64 volatile*)base, v);
	}
	inline usize atom_exchange_usize(usize volatile* dest, usize v)
	{
		return InterlockedExchange64((__int64 volatile*)dest, v);
	}
	inline usize atom_compare_exchange_usize(usize volatile* dest, usize exchange, usize comperand)
	{
		return InterlockedCompareExchange64((__int64 volatile*)dest, (__int64)exchange, (__int64)comperand);
	}
#else
	inline usize atom_inc_usize(usize volatile* v)
	{
		return InterlockedIncrement((LONG volatile*)v);
	}
	inline usize atom_dec_usize(usize volatile* v)
	{
		return InterlockedDecrement((LONG volatile*)v);
	}
	inline usize atom_add_usize(usize volatile* base, isize v)
	{
		return InterlockedAdd((LONG volatile*)base, (LONG)v);
	}
	inline usize atom_exchange_usize(usize volatile* dest, usize v)
	{
		return InterlockedExchange((LONG volatile*)dest, (LONG)v);
	}
	inline usize atom_compare_exchange_usize(usize volatile* dest, usize exchange, usize comperand)
	{
		return InterlockedCompareExchange((LONG volatile*)dest, (LONG)exchange, (LONG)comperand);
	}
#endif

}
#endif