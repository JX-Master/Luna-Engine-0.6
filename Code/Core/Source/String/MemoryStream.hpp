// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MemoryStream.hpp
* @author JXMaster
* @date 2020/1/28
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/TSAssert.hpp>
namespace luna
{
	class MemoryStream : public IMemoryStream
	{
	public:
		lucid("{78195b24-98a1-4564-bf1b-885bde0ee87e}");
		luiimpl(MemoryStream, IMemoryStream, IStream, IObject);
		lutsassert_lock();

		uint64 m_cursor;
		P<IBuffer> m_buf;

		MemoryStream(IAllocator* alloc) :
			luibind(alloc),
			m_cursor(0) {}

		virtual EStreamFlag flags() override
		{
			return EStreamFlag::readable | EStreamFlag::writable | EStreamFlag::seekable | EStreamFlag::resizable;
		}
		virtual RV read(void* buffer, size_t size, size_t* read_bytes) override;
		virtual RV write(const void* buffer, size_t size, size_t* write_bytes) override;
		virtual R<uint64> size() override
		{
			return R<uint64>::success((uint64)m_buf->buffer_size());
		}
		virtual RV set_size(uint64 sz) override;
		virtual R<uint64> tell() override
		{
			return R<uint64>::success(m_cursor);
		}
		virtual RV seek(int64 offset, ESeekMode mode) override;
		virtual RV flush() override
		{
			return e_not_available;
		}
		virtual IBuffer* buffer() override
		{
			return m_buf;
		}
		virtual void set_buffer(IBuffer* buf) override
		{
			luassert_usr(buf);
			m_buf = buf;
			m_cursor = 0;
		}
		virtual P<IBuffer> reset(size_t initial_buffer_size = 0, size_t alignment = 0, IAllocator* alloc = nullptr) override;
	};
}