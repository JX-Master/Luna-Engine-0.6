// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IBuffer.hpp
* @author JXMaster
* @date 2020/1/9
*/
#pragma once
#include "IBlob.hpp"
#include <Base/IAllocator.hpp>

namespace luna
{
	//! @interface IBuffer.
	//! IBuffer is the "default" implementation of `IBlob` interface, it only encapsulates a memory buffer block and the allocator
	//! that is used to allocate the buffer. By passing a buffer object, the user is granted to operate on the buffer in any way 
	//! with the provided memory allocator, including reallocating buffers to change its size.
	//! 
	//! The buffer knows nothing about the data in it, it is user's decision that how the data is interpreted and used.
	struct IBuffer : public IBlob
	{
		luiid("{6ec4063a-2de1-4600-b545-045b2189a55f}");

		//! Gets a pointer to the underlying buffer.
		virtual void* buffer() = 0;

		//! Gets the size of the buffer.
		virtual size_t buffer_size() = 0;

		//! Gets the alignment requirement for the buffer. This is exactly the one the user specified when creating 
		//! or reseting buffer.
		virtual size_t buffer_alignment() = 0;

		//! Sets the offset and size of the data region. Calls to `data` will return the pointer to the first byte of the data, and calls to 
		//! `size` will return the size of the data.
		//! The default data offset is 0 and the default data size equals to the size of the buffer.
		virtual void set_data_region(size_t offset, size_t sz) = 0;

		//! Gets the offset of the data.
		virtual size_t data_offset() = 0;

		//! Gets the allocator for this buffer. This cannot be changed after the buffer object is created.
		//! Do not free the buffer using the allocator directly, always use `reset` to reallocate the buffer.
		virtual IAllocator* allocator() = 0;

		//! Allocates a new buffer memory block and attaches the new buffer memory to this object, then 
		//! creates and returns a new buffer object with the old buffer memory block attached.
		//! 
		//! The data offset will be reset to 0 and the data size will be reset to the size of the buffer after this operation.
		virtual P<IBuffer> reset(size_t new_size, size_t new_alignment = 0) = 0;
	};
}