// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IHeap.hpp
* @author JXMaster
* @date 2019/6/17
*/
#pragma once
#include <Base/IAllocator.hpp>

namespace luna
{
	//! @interface IHeap
	//! Heap is a segment of reserved memory address in process's memory address space, and is used to
	//! dynamically allocate memory. Memory in one heap will be mapped to physical memory when the memory
	//! is allocated, and may be swapped out to the page swap file when the physical memory is not enough.
	//! 
	//! Luna Engine maintains the default heap for the system called global heap, all system resources, 
	//! including the system instance itself, is allocated from the system heap. Then, you can create custom 
	//! dedicated heap with `IPlatform::new_heap`.
	//! 
	//! The global heap allocation is thread-safe. Custom heap allocation is also thread-safe unless you specify
	//! `EHeapCreationFlag::no_thread_safe` when creating the heap, in such case you guarantee that the heap will
	//! only be used by one thread at one time, so it may skip multi-thread serialization in order to improve 
	//! performance.
	struct IHeap : public IAllocator
	{
		luiid("{f5636785-82db-4050-8637-aecfc9720297}");

		//! Gets the debug name set_char to this allocator.
		//! The memory space for the debug name is allocated when the allocator is created and should always be valid if the allocator
		//! is valid. Its size is 64 bytes, including the null-terminator.
		virtual const char* name() = 0;
		//! Sets the debug name of this allocator. If the length of `name` is longer than 63 bytes, it will be truncated.
		virtual void set_name(const char* name) = 0;
		//! Gets the total size of the memory allocated from this heap in bytes.
		virtual size_t allocated() = 0;
	};
}