// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file MemoryStream.hpp
* @author JXMaster
* @date 2020/1/28
*/
#pragma once
#include <Runtime/TSAssert.hpp>
#include "../IMemoryStream.hpp"
#include <Core/Interface.hpp>
namespace Luna
{
	class MemoryStream : public IMemoryStream
	{
	public:
		lucid("{78195b24-98a1-4564-bf1b-885bde0ee87e}");
		luiimpl(MemoryStream, IMemoryStream, IStream, IObject);
		lutsassert_lock();

		usize m_cursor;
		usize m_data_offset;
		usize m_data_size;
		Blob m_buf;

		MemoryStream() :
			m_cursor(0),
			m_data_offset(0),
			m_data_size(0) {}

		virtual EStreamFlag flags() override
		{
			return EStreamFlag::readable | EStreamFlag::writable | EStreamFlag::seekable | EStreamFlag::resizable;
		}
		virtual RV read(void* buffer, usize size, usize* read_bytes) override;
		virtual RV write(const void* buffer, usize size, usize* write_bytes) override;
		virtual u64 size() override
		{
			return m_data_size;
		}
		virtual RV set_size(u64 sz) override;
		virtual R<u64> tell() override
		{
			return R<u64>::success(m_cursor);
		}
		virtual RV seek(i64 offset, ESeekMode mode) override;
		virtual void flush() override {}
		virtual const Blob& get_blob() override
		{
			return m_buf;
		}
		virtual usize get_data_offset() override
		{
			return m_data_offset;
		}
		virtual void* get_data() override
		{
			return (void*)((usize)m_buf.data() + m_data_offset);
		}
		virtual Blob set_blob(const Blob& blob, usize offset, usize sz) override
		{
			Blob ori = move(m_buf);
			m_buf = blob;
			m_cursor = 0;
			m_data_offset = offset;
			m_data_size = (sz == usize_max) ? (blob.size() - offset) : sz;
			return ori;
		}
		virtual Blob set_blob(Blob&& blob, usize offset, usize sz) override
		{
			Blob ori = move(m_buf);
			m_buf = move(blob);
			m_cursor = 0;
			m_data_offset = offset;
			m_data_size = (sz == usize_max) ? (blob.size() - offset) : sz;
			return ori;
		}
	};
}