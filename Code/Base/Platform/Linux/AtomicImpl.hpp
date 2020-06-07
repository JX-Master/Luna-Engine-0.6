// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Atomic.inl
* @author JXMaster
* @date 2019/4/9
*/
#pragma once
#include "../../BaseDefines.hpp"

#ifdef LUNA_PLATFORM_LINUX

namespace luna
{
	//! Atomically increase the value of the variable by 1. This operation cannot be interrupted by system thread switching.
	//! @param[in] value The pointer to the variable that needs to be changed.
	//! @return Returns the value of the variable after this operation.
	inline int32 atom_inc_i32(int32 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline uint32 atom_inc_u32(uint32 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline int64 atom_inc_i64(int64 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline uint64 atom_inc_u64(uint64 volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}

	//! Atomically decrease the value of the variable by 1. This operation cannot be interrupted by system thread switching.
	//! @param[in] value The pointer to the variable that needs to be changed.
	//! @return Returns the value of the variable after this operation.
	inline int32 atom_dec_i32(int32 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline uint32 atom_dec_u32(uint32 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline int64 atom_dec_i64(int64 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline uint64 atom_dec_u64(uint64 volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}

	//! Atomically increase the value of the variable by the the value provided. This operation cannot be interrupted by system thread switching.
	//! @param[in] base The pointer to the variable that needs to be changed.
	//! @param[in] value The value that needs to be added to the variable.
	//! @return Returns the value of the variable before this operation.
	inline int32 atom_add_i32(int32 volatile* base, int32 v)
	{
		return __sync_fetch_and_add(base, v);
	}
	inline uint32 atom_add_u32(uint32 volatile* base, int32 v)
	{
		return __sync_fetch_and_add(base, v);
	}
	inline int64 atom_add_i64(int64 volatile* base, int64 v)
	{
		return __sync_fetch_and_add(base, v);
	}
	inline uint64 atom_add_u64(uint64 volatile* base, int64 v)
	{
		return __sync_fetch_and_add(base, v);
	}

	//! Atomically replace the value of the variable with the value provided. This operation cannot be interrupted by system thread switching.
	//! @param[in] base The pointer to the variable that needs to be changed.
	//! @param[in] value The value that needs to be set to the variable.
	//! @return Returns the value of the variable before this operation took place.
	inline int32 atom_exchange_i32(int32 volatile* dest, int32 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline uint32 atom_exchange_u32(uint32 volatile* dest, uint32 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline int64 atom_exchange_i64(int64 volatile* dest, int64 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline uint64 atom_exchange_u64(uint64 volatile* dest, uint64 v)
	{
		return __sync_lock_test_and_set(dest, v);
	}

	//! Atomically compare the value of the variable with the value provided by `comperand`. This operation cannot be interrupted by system thread switching.
	//! If the value of the variable equals to the value provided by `comperand`, the value of the variable will be replaced by the value provided by `exchange`, and the old value of the variable will be returned.
	//! If the value of the variable does not equal to the value provided by `comperand`, the value of the variable will not change, and this value it will be returned.
	inline int32 atom_compare_exchange_i32(int32 volatile* dest, int32 exchange, int32 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
	inline uint32 atom_compare_exchange_u32(uint32 volatile* dest, uint32 exchange, uint32 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}

#ifdef LUNA_PLATFORM_64BIT
	//! Atomically increase the value of the variable by 1. This operation cannot be interrupted by system thread switching.
	//! @param[in] value The pointer to the variable that needs to be changed.
	//! @return Returns the value of the variable after this operation.
	inline int64 atom_compare_exchange_i64(int64 volatile* dest, int64 exchange, int64 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
	inline uint64 atom_compare_exchange_u64(uint64 volatile* dest, uint64 exchange, uint64 comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
#endif

	inline size_t atom_inc_sizet(size_t volatile* v)
	{
		return __sync_fetch_and_add(v, 1) + 1;
	}
	inline size_t atom_dec_sizet(size_t volatile* v)
	{
		return __sync_fetch_and_sub(v, 1) - 1;
	}
	inline size_t atom_add_sizet(size_t volatile* base, ptrdiff_t v)
	{
		return __sync_fetch_and_add(base, v) + v;
	}
	inline size_t atom_exchange_sizet(size_t volatile* dest, size_t v)
	{
		return __sync_lock_test_and_set(dest, v);
	}
	inline size_t atom_compare_exchange_sizet(size_t volatile* dest, size_t exchange, size_t comperand)
	{
		return __sync_val_compare_and_swap(dest, comperand, exchange);
	}
}

#endif
