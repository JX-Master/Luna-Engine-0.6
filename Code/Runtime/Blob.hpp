// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Blob.hpp
* @author JXMaster
* @date 2020/8/17
*/
#pragma once
#include "Memory.hpp"
namespace Luna
{
	//! @class Blob
	//! Blob encapsulates one block of memory.
	class Blob
	{
		void* m_buffer;
		usize m_size;
	public:
		//! Creates the blob object without allocating any data.
		Blob() :
			m_buffer(nullptr),
			m_size(0) {}
		//! Creates the blob object and allocated the specified size of bytes.
		Blob(usize sz) :
			m_size(sz)
		{
			m_buffer = memalloc(sz);
		}
		//! Creates the blob object with initial data.
		Blob(const void* blob_data, usize data_sz) :
			m_size(data_sz)
		{
			m_buffer = memalloc(data_sz);
			memcpy(m_buffer, blob_data, data_sz);
		}
		Blob(const Blob& rhs) :
			m_size(rhs.m_size),
			m_buffer(nullptr)
		{
			if (m_size)
			{
				m_buffer = memalloc(m_size);
				memcpy(m_buffer, rhs.m_buffer, m_size);
			}
		}
		Blob(Blob&& rhs) :
			m_size(rhs.m_size),
			m_buffer(rhs.m_buffer)
		{
			rhs.m_size = 0;
			rhs.m_buffer = nullptr;
		}
		Blob& operator=(const Blob& rhs)
		{
			if (m_buffer)
			{
				memfree(m_buffer);
				m_buffer = nullptr;
			}
			m_size = rhs.m_size;
			if (m_size)
			{
				m_buffer = memalloc(m_size);
				memcpy(m_buffer, rhs.m_buffer, m_size);
			}
			return *this;
		}
		Blob& operator=(Blob&& rhs)
		{
			if (m_buffer)
			{
				memfree(m_buffer);
				m_buffer = nullptr;
			}
			m_size = rhs.m_size;
			m_buffer = rhs.m_buffer;
			rhs.m_size = 0;
			rhs.m_buffer = nullptr;
			return *this;
		}
		~Blob()
		{
			if (m_buffer)
			{
				memfree(m_buffer);
				m_buffer = nullptr;
			}
		}
		const void* data() const
		{
			return m_buffer;
		}
		void* data()
		{
			return m_buffer;
		}
		usize size() const
		{
			return m_size;
		}
		void resize(usize sz)
		{
			m_buffer = memrealloc(m_buffer, sz);
			m_size = sz;
		}
	};
}