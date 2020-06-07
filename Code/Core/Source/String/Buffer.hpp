// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Buffer.hpp
* @author JXMaster
* @date 2020/1/15
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	class Buffer : public IBuffer
	{
	public:
		lucid("{250a1e6b-84b8-4060-8d61-a19bf14159ea}");
		luiimpl(Buffer, IBuffer, IBlob, IObject);
		lutsassert_lock();

		void* m_buffer;
		size_t m_size;
		size_t m_alignment;
		size_t m_data_size;
		size_t m_data_offset;

		Buffer(IAllocator* alloc) :
			luibind(alloc) {}

		~Buffer()
		{
			if (m_buffer)
			{
				if (m_alignment)
				{
					m_rc.m_alloc->free_aligned(m_buffer, m_alignment);
				}
				else
				{
					m_rc.m_alloc->free(m_buffer);
				}
				m_buffer = nullptr;
				m_size = 0;
			}
		}

		virtual const void* data() override
		{
			lutsassert();
			return (void*)((size_t)m_buffer + m_data_offset);
		}

		virtual size_t size() override
		{
			lutsassert();
			return m_data_size;
		}

		virtual void* buffer() override
		{
			lutsassert();
			return m_buffer;
		}

		virtual size_t buffer_size() override
		{
			lutsassert();
			return m_size;
		}

		virtual size_t buffer_alignment() override
		{
			lutsassert();
			return m_alignment;
		}

		virtual void set_data_region(size_t offset, size_t sz) override
		{
			lutsassert();
			m_data_size = sz;
			m_data_offset = offset;
		}

		virtual size_t data_offset() override
		{
			lutsassert();
			return m_data_offset;
		}

		virtual IAllocator* allocator() override
		{
			lutsassert();
			return m_rc.m_alloc;
		}

		virtual P<IBuffer> reset(size_t new_size, size_t new_alignment = 0)
		{
			lutsassert();
			void* buf;
			if (new_alignment)
			{
				buf = m_rc.m_alloc->allocate_aligned(new_size, new_alignment);
			}
			else
			{
				buf = m_rc.m_alloc->allocate(new_size);
			}
			Buffer* b = m_rc.m_alloc->new_obj<Buffer>(m_rc.m_alloc);
			b->m_buffer = m_buffer;
			b->m_size = m_size;
			b->m_alignment = m_alignment;
			b->m_data_offset = m_data_offset;
			b->m_data_size = m_data_size;
			m_buffer = buf;
			m_size = new_size;
			m_alignment = new_alignment;
			m_data_offset = 0;
			m_data_size = new_size;
			return box_ptr<IBuffer>(b);
		}
	};
}