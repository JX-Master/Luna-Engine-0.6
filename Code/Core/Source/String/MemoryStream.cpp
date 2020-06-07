// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MemoryStream.cpp
* @author JXMaster
* @date 2020/1/28
*/
#include "MemoryStream.hpp"

namespace luna
{
	RV MemoryStream::read(void* buffer, size_t size, size_t* read_bytes)
	{
#ifdef LUNA_PLATFORM_32BIT
		if (m_cursor >= (uint64)size_t_max_v)
		{
			return e_not_available;
		}
#endif
		if (m_cursor >= (uint64)m_buf->size())
		{
			if (read_bytes)
			{
				*read_bytes = 0;
			}
			return s_ok;
		}
		size_t read_size = (size_t)min((uint64)size, ((uint64)m_buf->size() - m_cursor));
		if (read_size)
		{
			memcpy(buffer, (void*)((size_t)m_buf->buffer() + m_cursor), read_size);
			m_cursor += read_size;
		}
		if (read_bytes)
		{
			*read_bytes = read_size;
		}
		return s_ok;
	}

	RV MemoryStream::write(const void* buffer, size_t size, size_t* write_bytes)
	{
		uint64 llwrite_end = m_cursor + size;
#ifdef LUNA_PLATFORM_32BIT
		if (llwrite_end >= (uint64)size_t_max_v)
		{
			return e_not_available;
		}
#endif
		size_t write_end = (size_t)llwrite_end;
		// expand the buffer if needed.
		if (write_end > m_buf->size())
		{
			if (write_end > m_buf->buffer_size())
			{
				auto old_buf = m_buf->reset(max(m_buf->buffer_size() * 2, write_end), m_buf->buffer_alignment());
				memcpy(m_buf->buffer(), old_buf.get()->data(), old_buf.get()->size());
			}
			m_buf->set_data_region(0, write_end);
		}
		memcpy((void*)((size_t)m_buf->buffer() + m_cursor), buffer, size);
		m_cursor += size;
		if (write_bytes)
		{
			*write_bytes = size;
		}
		return s_ok;
	}

	RV MemoryStream::set_size(uint64 sz)
	{
#ifdef LUNA_PLATFORM_32BIT
		if (sz > (uint64)(size_t_max_v))
		{
			return e_not_available;
		}
#endif
		size_t ssz = (size_t)sz;
		if (ssz > m_buf->buffer_size())
		{
			auto old_buf = m_buf->reset(max(m_buf->buffer_size() * 2, ssz), m_buf->buffer_alignment());
			memcpy(m_buf->buffer(), old_buf->data(), old_buf->size());
		}
		m_buf->set_data_region(0, ssz);
		return s_ok;
	}
	RV MemoryStream::seek(int64 offset, ESeekMode mode)
	{
		switch (mode)
		{
		case ESeekMode::begin:
			if (offset < 0)
			{
				return e_bad_arguments;
			}
			m_cursor = (uint64)offset;
			break;
		case ESeekMode::current:
			if (offset < 0 && ((uint64)(-offset) > m_cursor))
			{
				return e_bad_arguments;
			}
			m_cursor += offset;
			break;
		case ESeekMode::end:
			if (offset < 0 && ((uint64)(-offset) > m_buf->size()))
			{
				return e_bad_arguments;
			}
			m_cursor = m_buf->size() + offset;
			break;
		default:
			break;
		}
		return s_ok;
	}
	P<IBuffer> MemoryStream::reset(size_t initial_buffer_size, size_t alignment, IAllocator* alloc)
	{
		if (!initial_buffer_size)
		{
			initial_buffer_size = 32;
		}
		if (!alloc)
		{
			alloc = m_buf->allocator();
		}
		auto buf = new_buffer(initial_buffer_size, alignment, alloc);
		auto old_buf = m_buf;
		m_buf = buf;
		m_buf->set_data_region(0, 0);
		m_cursor = 0;
		return old_buf;
	}
}