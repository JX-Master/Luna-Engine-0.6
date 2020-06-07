// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAllocator.hpp
* @author JXMaster
* @date 2018/12/8
*/
#pragma once
#include "IObject.hpp"

namespace luna
{
	//! @interface ICloneable
	//! A general-purpose memory allocator interface for heap-based dynamic memory allocation.
	struct IAllocator : public IObject
	{
		luiid("{a2093d88-9037-4ff8-b32a-74349a711786}");
		//! Allocates memory blocks. The returns memory block is 8-byte aligned in 32-bit version and 16-byte aligned in 64-bit version.
		//! @param[in] size The size of bytes you want to allocate. Is this is 0, no memory will be allocated and return value is `nullptr`.
		//! @return Returns a pointer to the first available memory address allocated, or `nullptr` if failed to allocate.
		//! @remark The usage hint string is used to provide additional type or usage info to the system, so when a memory leak or a large memory
		//! allocation happens, the system can generate human-readable log to notify the developer.
		//! 
		//! For a heap created by system, the usage_hint string will be passed to all hooks that are monitoring the memory change. 
		//! In debug build of the engine, heaps will also intern the usage string, and print detailed warning info if a memory leak is detected.
		virtual void* allocate(size_t s) = 0;
		//! Frees memory blocks allocated by `allocate` or `reallocate`.
		//! @param[in] addr The pointer returned by `allocate` or `reallocate` method.
		//! @remark Allocate memory from one OS context and Free from another OS context (say in different modules) is not
		//! allowed and may lead to incorrect memory allocation profile.
		virtual void free(void* ptr) = 0;
		//! Reallocates memory blocks allocated from this allocator.
		//! @param[in] old_ptr The pointer to the former allocated memory allocated by `allocate` or `reallocate`. This will be freed if new memory 
		//�� is successfully allocated. If this parameter is `nullptr`, this method behaves the same as `allocate`.
		//! @param[in] new_size The new size of bytes to allocate.
		//! @return Returns a pointer to the first available memory address reallocated. If the allocation is failed, this method returns `nullptr` and
		//! origin memory block is not changed.
		virtual void* reallocate(void* old_ptr, size_t new_size) = 0;
		//! Gets the allocated size of a memory block allocated from this allocator.
		//! Note that the allocated size may be bigger than the size required to specify alignment and padding requirements.
		//! @param[in] ptr The pointer returned by `allocate` or `reallocate` method.
		//! @return Returns the size of bytes actually allocated from system.
		virtual size_t size(void* ptr) = 0;

		//! Allocates memory blocks with custom alignment requirement.
		//! @param[in] size The size of bytes you want to allocate. Is this is 0, no memory will be allocated and return value is `nullptr`.
		//! @param[in] alignment The alignment boundary the new allocated memory should specify. This value must by power of 2.
		//! @return Returns a pointer to the first available memory address allocated, or `nullptr` if failed to allocate.
		virtual void* allocate_aligned(size_t size, size_t alignment) = 0;
		//! Frees memory blocks allocated by `allocate_aligned` or `reallocate_aligned`.
		//! @param[in] ptr The pointer returned by `allocate_aligned` or `reallocate_aligned` method.
		//! @param[in] alignment The alignment you specify when calling `allocate_aligned` or `reallocate_aligned` method.
		virtual void free_aligned(void* ptr, size_t alignment) = 0;
		//! Reallocates memory blocks allocated by `allocate_aligned` or `reallocate_aligned`.
		//! @param[in] old_ptr The pointer returned by `allocate_aligned` or `reallocate_aligned` method.
		//! @param[in] old_alignment The alignment you specify when calling `allocate_aligned` or `reallocate_aligned` method.
		//! @param[in] new_size The new size of bytes to allocate.
		//! @param[in] new_alignment The new alignment that new allocated blocks should specify. This value must be power of 2.
		//! @return Returns a pointer to the first available memory address reallocated. If the allocation is failed, this method returns `nullptr` and
		//! origin memory block is not changed.
		virtual void* reallocate_aligned(void* old_ptr, size_t old_alignment, size_t new_size, size_t new_alignment) = 0;
		//! Get the allocated size of a memory block allocated by `allocate_aligned` or `reallocate_aligned`.
		//! @param[in] ptr The pointer returned by `allocate_aligned` or `reallocate_aligned` method.
		//! @param[in] alignment The alignment you specify when calling `allocate_aligned` or `reallocate_aligned` method.
		//! @return Returns the size of bytes actually allocated from system.
		virtual size_t size_aligned(void* ptr, size_t alignment) = 0;

		//! Allocates memory from this allocator and creates an object of specified class. 
		//! This is similar to C++ `new` operator, but allocates memory using this allocator.
		//! @param[in] args The arguments passed to object's ctor.
		//! @return Returns a pointer to new constructed object, or `nullptr` if failed to create.
		//! @remark This function cannot grantee custom alignment boundary, use `NewAligned` to do that.
		template <typename _Ty, typename... _Args>
		_Ty* new_obj(_Args&&... args)
		{
			_Ty* o;
			if (alignof(_Ty) <= max_align_v)
			{
				o = reinterpret_cast<_Ty*>(allocate(sizeof(_Ty)));
			}
			else
			{
				o = reinterpret_cast<_Ty*>(allocate_aligned(sizeof(_Ty), alignof(_Ty)));
			}
			if (o)
			{
				new (o) _Ty(forward<_Args>(args)...);
				return o;
			}
			return nullptr;
		}

		//! Deletes an object created by this allocator's `new_obj` method. This is similar to C++ `delete` operator.
		//! @param[in] o A pointer to the object that needs to be deleted.
		template <typename _Ty>
		void delete_obj(_Ty* o)
		{
			o->~_Ty();
			if (alignof(_Ty) <= max_align_v)
			{
				free(o);
			}
			else
			{
				free_aligned(o, alignof(_Ty));
			}
		}
	};

	namespace impl
	{
		template <typename _Ty>
		inline _Ty* allocate_array(IAllocator* alloc, size_t count, true_type) // is_over_aligned = true
		{
			return reinterpret_cast<_Ty*>(alloc->allocate_aligned(count * sizeof(_Ty), alignof(_Ty)));
		}

		template <typename _Ty>
		inline _Ty* allocate_array(IAllocator* alloc, size_t count, false_type) // is_over_aligned = false
		{
			return reinterpret_cast<_Ty*>(alloc->allocate(count * sizeof(_Ty)));
		}

		template <typename _Ty>
		inline void deallocate_array(IAllocator* alloc, _Ty* ptr, true_type) // is_over_aligned = true
		{
			alloc->free_aligned(ptr, alignof(_Ty));
		}

		template <typename _Ty>
		inline void deallocate_array(IAllocator* alloc, _Ty* ptr, false_type) // is_over_aligned = false
		{
			alloc->free(ptr);
		}
	}

	//! Allocates memory for an array of `count` objects with alignment requirement satisfied.
	//! The allocated memory is not initialized.
	//! @param[in] alloc The allocator used to allocate the object.
	//! @param[in] count The number of objects to allocate.
	template <typename _Ty>
	inline _Ty* allocate_array(IAllocator* alloc, size_t count)
	{
		return impl::allocate_array<_Ty>(alloc, count, typename is_over_aligned<_Ty>::type());
	}

	//! Frees memory allocated from `allocate`. The destructors for the objects are not called 
	//! in this function.
	//! @param[in] alloc The allocator passed to `allocate_array`
	//! @param[in] ptr The pointer returned from `allocate_array`.
	template <typename _Ty>
	inline void deallocate_array(IAllocator* alloc, _Ty* ptr)
	{
		return impl::deallocate_array<_Ty>(alloc, ptr, typename is_over_aligned<_Ty>::type());
	}
}