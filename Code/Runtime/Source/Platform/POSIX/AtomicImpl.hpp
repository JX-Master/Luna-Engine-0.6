// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file AtomicImpl.hpp
* @author JXMaster
* @date 2020/9/22
 */
#pragma once
#include "../../../Base.hpp"

#ifdef LUNA_PLATFORM_POSIX

namespace Luna
{
	inline i32 atom_inc_i32(i32 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline u32 atom_inc_u32(u32 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline i64 atom_inc_i64(i64 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline u64 atom_inc_u64(u64 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}

	inline i32 atom_dec_i32(i32 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline u32 atom_dec_u32(u32 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline i64 atom_dec_i64(i64 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline u64 atom_dec_u64(u64 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}

	inline i32 atom_add_i32(i32 volatile* base, i32 v)
	{
		return __sync_fetch_and_add(base, v);
	}
	inline u32 atom_add_u32(u32 volatile* base, i32 v)
	{
		return __sync_fetch_and_add(base, v);
	}
	inline i64 atom_add_i64(i64 volatile* base, i64 v)
	{
		return __sync_fetch_and_add(base, v);
	}
	inline u64 atom_add_u64(u64 volatile* base, i64 v)
	{
		return __sync_fetch_and_add(base, v);
	}

	inline i32 atom_exchange_i32(i32 volatile* dest, i32 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline u32 atom_exchange_u32(u32 volatile* dest, u32 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline i64 atom_exchange_i64(i64 volatile* dest, i64 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline u64 atom_exchange_u64(u64 volatile* dest, u64 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline void* atom_exchange_pointer(void* volatile* target, void* value)
	{
		return __sync_lock_test_and_set(target, value);
	}

	inline i32 atom_compare_exchange_i32(i32 volatile* dest, i32 exchange, i32 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
	inline u32 atom_compare_exchange_u32(u32 volatile* dest, u32 exchange, u32 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
	inline void* atom_compare_exchange_pointer(void* volatile* dest, void* exchange, void* comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}

#ifdef LUNA_PLATFORM_64BIT
	//! Atomically compare the value of the variable with the value provided by `comperand`. This operation cannot be interrupted by system thread switching.
	//! If the value of the variable equals to the value provided by `comperand`, the value of the variable will be replaced by the value provided by `exchange`, and the old value of the variable will be returned.
	//! If the value of the variable does not equal to the value provided by `comperand`, the value of the variable will not change, and this value it will be returned.
	inline i64 atom_compare_exchange_i64(i64 volatile* dest, i64 exchange, i64 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
	inline u64 atom_compare_exchange_u64(u64 volatile* dest, u64 exchange, u64 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
#endif

	inline usize atom_inc_usize(usize volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline usize atom_dec_usize(usize volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline usize atom_add_usize(usize volatile* base, isize v)
	{
		return __sync_fetch_and_add(base, v) + v;
	}
	inline usize atom_exchange_usize(usize volatile* dest, usize v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline usize atom_compare_exchange_usize(usize volatile* dest, usize exchange, usize comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}

}
#endif
