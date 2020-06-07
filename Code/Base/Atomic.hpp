// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Atomic.hpp
* @author JXMaster
* @date 2018/12/7
 */
#pragma once
#include "PlatformDefines.hpp"

#if defined(LUNA_PLATFORM_WINDOWS)
#include "Platform/Windows/AtomicImpl.hpp"
#elif defined(LUNA_PLATFORM_LINUX)
#include "Platform/Linux/AtomicImpl.hpp"
#endif

namespace luna
{
	//! Atomically increase the value of the variable by 1. This operation cannot be interrupted by system thread switching.
	//! @param[in] value The pointer to the variable that needs to be changed.
	//! @return Returns the value of the variable after this operation.
	inline int32 atom_inc_i32(int32 volatile* v);
	inline uint32 atom_inc_u32(uint32 volatile* v);
	inline int64 atom_inc_i64(int64 volatile* v);
	inline uint64 atom_inc_u64(uint64 volatile* v);
	inline size_t atom_inc_sizet(size_t volatile* v);

	//! Atomically decrease the value of the variable by 1. This operation cannot be interrupted by system thread switching.
	//! @param[in] value The pointer to the variable that needs to be changed.
	//! @return Returns the value of the variable after this operation.
	inline int32 atom_dec_i32(int32 volatile* v);
	inline uint32 atom_dec_u32(uint32 volatile* v);
	inline int64 atom_dec_i64(int64 volatile* v);
	inline uint64 atom_dec_u64(uint64 volatile* v);
	inline size_t atom_dec_sizet(size_t volatile* v);

	//! Atomically increase the value of the variable by the the value provided. This operation cannot be interrupted by system thread switching.
	//! @param[in] base The pointer to the variable that needs to be changed.
	//! @param[in] value The value that needs to be added to the variable.
	//! @return Returns the value of the variable before this operation.
	inline int32 atom_add_i32(int32 volatile* base, int32 v);
	inline uint32 atom_add_u32(uint32 volatile* base, int32 v);
	inline int64 atom_add_i64(int64 volatile* base, int64 v);
	inline uint64 atom_add_u64(uint64 volatile* base, int64 v);
	inline size_t atom_add_sizet(size_t volatile* base, ptrdiff_t v);

	//! Atomically replace the value of the variable with the value provided. This operation cannot be interrupted by system thread switching.
	//! @param[in] base The pointer to the variable that needs to be changed.
	//! @param[in] value The value that needs to be set to the variable.
	//! @return Returns the value of the variable before this operation took place.
	inline int32 atom_exchange_i32(int32 volatile* dest, int32 v);
	inline uint32 atom_exchange_u32(uint32 volatile* dest, uint32 v);
	inline int64 atom_exchange_i64(int64 volatile* dest, int64 v);
	inline uint64 atom_exchange_u64(uint64 volatile* dest, uint64 v);
	inline void* atom_exchange_pointer(void* volatile* target, void* value);
	inline size_t atom_exchange_sizet(size_t volatile* dest, size_t v);

	//! Atomically compare the value of the variable with the value provided by `comperand`. This operation cannot be interrupted by system thread switching.
	//! If the value of the variable equals to the value provided by `comperand`, the value of the variable will be replaced by the value provided by `exchange`, and the old value of the variable will be returned.
	//! If the value of the variable does not equal to the value provided by `comperand`, the value of the variable will not change, and this value it will be returned.
	inline int32 atom_compare_exchange_i32(int32 volatile* dest, int32 exchange, int32 comperand);
	inline uint32 atom_compare_exchange_u32(uint32 volatile* dest, uint32 exchange, uint32 comperand);
	inline void* atom_compare_exchange_pointer(void* volatile* dest, void* exchange, void* comperand);
#ifdef LUNA_PLATFORM_64BIT
	inline int64 atom_compare_exchange_i64(int64 volatile* dest, int64 exchange, int64 comperand);
	inline uint64 atom_compare_exchange_u64(uint64 volatile* dest, uint64 exchange, uint64 comperand);
#endif
	inline size_t atom_compare_exchange_sizet(size_t volatile* dest, size_t exchange, size_t comperand);
}
