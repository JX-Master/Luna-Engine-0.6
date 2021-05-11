// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MemoryStream.cpp
* @author JXMaster
* @date 2020/1/28
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include <Core/Core.hpp>
#include "MemoryStream.hpp"

namespace Luna
{
	RV MemoryStream::read(void* buffer, usize sz, usize* read_bytes)
	{
		if (m_cursor >= size())
		{
			if (read_bytes)
			{
				*read_bytes = 0;
			}
			return RV();
		}
		usize read_size = (usize)min(sz, (size() - m_cursor));
		if (read_size)
		{
			memcpy(buffer, (void*)((usize)get_data() + m_cursor), read_size);
			m_cursor += read_size;
		}
		if (read_bytes)
		{
			*read_bytes = read_size;
		}
		return RV();
	}

	RV MemoryStream::write(const void* buffer, usize sz, usize* write_bytes)
	{
		u64 llwrite_end = m_cursor + sz;
#ifdef LUNA_PLATFORM_32BIT
		if (llwrite_end >= (u64)usize_max)
		{
			return BasicError::overflow();
		}
#endif
		usize write_end = (usize)llwrite_end;
		// expand the buffer if needed.
		if (write_end > size())
		{
			if (write_end > (m_buf.size() - m_data_offset))
			{
				// Reallocate buffer.
				Blob new_blob(max((m_buf.size() - m_data_offset) * 2, write_end));
				memcpy(new_blob.data(), get_data(), (usize)size());
				m_buf = move(new_blob);
				m_data_offset = 0;
			}
			m_data_size = write_end;
		}
		memcpy((void*)((usize)get_data() + m_cursor), buffer, sz);
		m_cursor += sz;
		if (write_bytes)
		{
			*write_bytes = sz;
		}
		return RV();
	}

	RV MemoryStream::set_size(u64 sz)
	{
#ifdef LUNA_PLATFORM_32BIT
		if (sz > (u64)(usize_max))
		{
			return BasicError::overflow();
		}
#endif
		usize ssz = (usize)sz;
		if (ssz > (m_buf.size() - m_data_offset))
		{
			// Reallocate buffer.
			Blob new_blob(max((m_buf.size() - m_data_offset) * 2, ssz));
			memcpy(new_blob.data(), get_data(), (usize)size());
			m_buf = move(new_blob);
			m_data_offset = 0;
		}
		m_data_size = ssz;
		return RV();
	}
	RV MemoryStream::seek(i64 offset, ESeekMode mode)
	{
		switch (mode)
		{
		case ESeekMode::begin:
			if (offset < 0)
			{
				return BasicError::out_of_range();
			}
			m_cursor = (usize)offset;
			break;
		case ESeekMode::current:
			if (offset < 0 && ((u64)(-offset) > m_cursor))
			{
				return BasicError::out_of_range();
			}
			m_cursor += (usize)offset;
			break;
		case ESeekMode::end:
			if (offset < 0 && ((u64)(-offset) > size()))
			{
				return BasicError::out_of_range();
			}
			m_cursor = (usize)size() + (usize)offset;
			break;
		default:
			break;
		}
		return RV();
	}

	LUNA_CORE_API P<IMemoryStream> new_memory_stream()
	{
		return newobj<MemoryStream>();
	}

}