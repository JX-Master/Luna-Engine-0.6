// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMemoryStream.hpp
* @author JXMaster
* @date 2020/1/28
*/
#pragma once
#include "IStream.hpp"
#include "IBuffer.hpp"

namespace luna
{
	//! @interface IMemoryStream
	//! The memory stream implements a stream object that is saved in the memory.
	struct IMemoryStream : public IStream
	{
		luiid("{b2fee308-b292-4fd4-b2c5-ece036a35e4a}");

		//! Gets the underlying buffer.
		virtual IBuffer* buffer() = 0;

		//! Sets the underlying buffer.
		//! 
		//! The cursor position will be reset to 0.
		virtual void set_buffer(IBuffer* buf) = 0;

		//! Creates a new memory buffer for this stream, and returns the old memory buffer.
		//! 
		//! The cursor position will be reset to 0 after the stream has been reset.
		virtual P<IBuffer> reset(size_t initial_buffer_size = 0, size_t alignment = 0, IAllocator* alloc = nullptr) = 0;
	};
}