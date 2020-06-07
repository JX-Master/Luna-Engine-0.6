// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IStream.hpp
* @author JXMaster
* @date 2020/1/28
*/
#pragma once
#include <Base/IObject.hpp>
#include "../Base/Result.hpp"

namespace luna
{
	enum class ESeekMode : uint32
	{
		//! The offset is relative to the beginning of the stream.
		begin = 1,
		//! The offset is relative to the current position of the cursor.
		current = 2,
		//! The offset is relative to the end of the stream.
		end = 3
	};

	enum class EStreamFlag : uint32
	{
		none = 0x00,
		//! Set if this stream supports reading.
		readable = 0x01,
		//! Set if this stream supports writing.
		writable = 0x02,
		//! Set if this stream supports seeking.
		seekable = 0x04,
		//! Set if this stream is resizable. If the stream cannot be resized, it will not be extended
		//! when writing operation occurs, and `set_size` cannot be called.
		resizable = 0x08,
		//! Set if this stream is buffered and calls to `flush` is valid and maybe needed.
		buffered = 0x10,
	};

	//! @interface IStream
	//! Represents a memory block or a device that holds a sequence of bytes and supports read/write 
	//! operations on them. The main implementation of `IStream` is `IFile` and `IMemoryStream`, which
	//! inputs/outputs data into a file or a memory chunk.
	//! 
	//! This object is not thread safe and the I/O operations on this object is not asynchronous (will suspend
	//! the current thread until the operation is done or failed). For an asynchronous version of stream, using 
	//! `IAsyncStream` instead.
	struct IStream : public IObject
	{
		luiid("{0345f636-ca5c-4b4d-8416-29834377d239}");

		//! Gets the stream flags.
		virtual EStreamFlag flags() = 0;

		//! Reads data from the current position the cursor is pointing to and offsets the cursor back. If the data to be
		//! read is not ready, the platform suspends this thread until the data is ready.
		//! @param[in] buffer The buffer to accept the read data. The buffer should be large enough to hold
		//! at least `size` bytes of data.
		//! @param[in] size The size of the data to read in bytes.
		//! @param[out] read_bytes If not `nullptr`, the system sets the actual size of bytes being read to the buffer 
		//! to this parameter.
		//! The actual size of bytes being read may be smaller than the size of bytes required to be read if the cursor
		//! reaches the end of the stream, but this is NOT an error. Specially, if one read operation is performed when
		//! the cursor is beyond or at the end of the stream, the read operation succeeds with 0 bytes being read. This 
		//! can be considered as an EOF symbol in stdlib.
		virtual RV read(void* buffer, size_t size, size_t* read_bytes = nullptr) = 0;

		//! Writes data to the current position the cursor is pointing to and offsets the cursor back. This call returns after
		//! all data have been written.
		//! @param[in] buffer The buffer that holds the data to be written.
		//! @param[in] size The size of the data to write in bytes.
		//! @param[out] write_bytes If not `nullptr`, the system sets the actual size of bytes being written to this parameter.
		//! Mostly, if the cursor goes beyond the end of the stream buffer while writing data, the stream will be expanded so
		//! the succeeding data can be written, so unless an error occurs, the size of bytes written will always equal to the 
		//! size of bytes required by the user to write. However, if an error occurs while writing data, some of the data may have 
		//! already be written while others are not, in such case the `write_bytes` reported by system may not be equal to `size` 
		//! specified by the user.
		virtual RV write(const void* buffer, size_t size, size_t* write_bytes = nullptr) = 0;

		//! Gets the size of the stream in bytes. Returns 0 if the underlying stream buffer is invalid.
		virtual R<uint64> size() = 0;

		//! Sets the size of the stream in bytes.
		//! If the current file size is smaller than the size to set and this call succeeded, the stream will be extended to the size specified
		//! with data between the last size and current size be uninitialized. If the current file size is greater than the size to set and this 
		//! call succeeded, the stream will be truncated and the data between the last size and current size will be discarded.
		//! @param[in] length The size to set, in bytes.
		virtual RV set_size(uint64 sz) = 0;

		//! Gets the current position of the stream cursor. The position is number of bytes relative to the beginning of the 
		//! stream.
		virtual R<uint64> tell() = 0;

		//! Moves the cursor to a new position.
		//! @param[in] offset Number of bytes to move relative to the position specified by `mode`.
		//! @param[in] mode The seek mode to use.
		//! @remark The cursor position is not bound to the size of the stream. If you move the cursor beyond the end of the stream, the cursor
		//! is still valid with the following behaviors:
		//! 1. All read operations succeed with 0 byte being read, just like EOF behaviors.
		//! 2. All write operations extends the size of the stream before the actual write operation performed.
		//! 
		//! The cursor value cannot be negative, if the new cursor position goes below 0, the seek operation fails.
		virtual RV seek(int64 offset, ESeekMode mode) = 0;

		//! Clears all buffers for this stream and causes all buffered data to be written to the underlying device. 
		//! This call fails with `e_not_available` if this stream is not buffered.
		virtual RV flush() = 0;
	};
}