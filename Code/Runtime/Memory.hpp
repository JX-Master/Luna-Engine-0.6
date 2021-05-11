// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Memory.hpp
* @author JXMaster
* @date 2020/7/22
* @brief Runtime memory management APIs.
*/
#pragma once
#include "Base.hpp"

#ifndef LUNA_RUNTIME_API
#define LUNA_RUNTIME_API
#endif

#ifdef LUNA_RUNTIME_CHECK_MEMORY_LEAK
#include <typeinfo>
#endif

namespace Luna
{
	//! Allocates memory blocks.
	//! @param[in] size The number of bytes to allocate. If this is 0, no memory will be allocated and the return value will be `nullptr`.
	//! @param[in] alignment Optional. The required alignment of the allocated memory block. 
	//! 
	//! If this is 0 (default), then the memory is allocated with no additional alignment requirement. In such case, the memory address is 
	//! aligned to times of 8 in 32-bit platform, and times of 16 in 64-bit platform. 
	//! 
	//! If this is not 0, then the memory is allocated with the specified alignment requirement satisfied. The alignment value must be powers
	//! of 2 (32, 64, 128, 256, etc).
	//! 
	//! Note that you shall use the same alignment value for the same memory block in `allocate`, `free`, `reallocate` and `size` function.
	//! 
	//! @return Returns a pointer to the first available memory address allocated, or `nullptr` if failed to allocate one from this allocator.
	LUNA_RUNTIME_API void* memalloc(usize size, usize alignment = 0);

	//! Frees memory blocks allocated by `memalloc` or `memrealloc`.
	//! @param[in] ptr The pointer returned by `OS::memalloc` or `OS::memrealloc` method. If this is `nullptr`, this function does nothing.
	//! @param[in] alignment Optional. The alignment requirement specified when allocating the memory block. Default is 0.
	LUNA_RUNTIME_API void memfree(void* ptr, usize alignment = 0);

	//! Reallocates memory blocks. This function allocates a new memory block with the specified size and alignment
	//! requirement, copies the data from the old memory block to the new one, and frees the old memory block.
	//! 
	//! The old memory block shall be the block allocated from `memalloc` or `memrealloc`.
	//! 
	//! @param[in] ptr The pointer to the former allocated memory allocated by `memalloc` or `memrealloc`. This will be freed if the new  
	//! memory block is successfully allocated. If this parameter is `nullptr`, this method behaves the same as `memalloc`.
	//! @param[in] size The new number of bytes to allocate.
	//! @param[in] alignment Optional. The alignment requirement for the old memory block. Default is 0. 
	//! 
	//! The new and old memory block use the same alignment requirement, which is decided when the old memory block is allocated.
	//! @return Returns a pointer to the reallocated memory block. If the allocation is failed, returns `nullptr` and
	//! the old memory block (if have) is not changed.
	LUNA_RUNTIME_API void* memrealloc(void* ptr, usize size, usize alignment = 0);

	//! Gets the allocated size of the memory block allocated by `memalloc` or `memrealloc`. 
	//! The returned size is the size that is available for the user to use. 
	//! Note that the allocated size may be bigger than the size required to specify alignment and padding requirements.
	//! @param[in] ptr The pointer returned by `OS::memalloc` or `OS::memrealloc`.
	//! @param[in] alignment Optional. The alignment requirement specified for the memory block during allocation. Default is 0. 
	//! @return Returns the size of bytes of the memory block. If `ptr` is `nullptr`, the returned value is 0.
	LUNA_RUNTIME_API usize memsize(void* ptr, usize alignment = 0);

	//! Counts the total memory allocated from heap in bytes.
	LUNA_RUNTIME_API usize get_allocated_memory();

#ifdef LUNA_RUNTIME_CHECK_MEMORY_LEAK

	//! Registers one memory block to the leak detection system.
	LUNA_RUNTIME_API void register_memory_block(void* blk, const c8* debug_name);

	//! Unregisters one memory block from the leak detection system.
	LUNA_RUNTIME_API void unregister_memory_block(void* blk);

#endif

	//! Global object creation function.
	template <typename _Ty, typename... _Args>
	_Ty* memnew(_Args&&... args)
	{
		_Ty* o = reinterpret_cast<_Ty*>(memalloc(sizeof(_Ty), alignof(_Ty)));
		if (o)
		{
			new (o) _Ty(forward<_Args>(args)...);
#ifdef LUNA_RUNTIME_CHECK_MEMORY_LEAK
			register_memory_block(o, typeid(_Ty).name());
#endif
			return o;
		}
		return nullptr;
	}

	//! Global object deleting function.
	template <typename _Ty>
	void memdelete(_Ty* o)
	{
		o->~_Ty();
		memfree(o, alignof(_Ty));
#ifdef LUNA_RUNTIME_CHECK_MEMORY_LEAK
		unregister_memory_block(o);
#endif
	}
}
