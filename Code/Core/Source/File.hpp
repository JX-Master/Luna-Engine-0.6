// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file File.hpp
* @author JXMaster
* @date 2019/9/29
*/
#pragma once
#include "../IFile.hpp"
#include "../IFileIterator.hpp"
#include "../Interface.hpp"
#include <Runtime/TSAssert.hpp>
#include <Runtime/Platform.hpp>

namespace Luna
{
	class File final : public IFile
	{
	public:
		lucid("{915247e4-15b4-44ba-8781-dd7dcfd48f87}");
		luiimpl(File, IFile, IStream, IObject);
		lutsassert_lock();

		handle_t m_file;
		EStreamFlag m_flags;

		File() :
			m_file(nullptr),
			m_flags(EStreamFlag::none) {}
		~File()
		{
			if (m_file != nullptr)
			{
				Platform::close_file(m_file);
				m_file = nullptr;
			}
		}
		virtual EStreamFlag flags() override
		{
			lutsassert();
			return m_flags;
		}
		virtual RV read(void* buffer, usize size, usize* read_bytes) override;
		virtual RV write(const void* buffer, usize size, usize* write_bytes) override;
		virtual u64 size() override;
		virtual RV set_size(u64 sz) override;
		virtual R<u64> tell() override;
		virtual RV seek(i64 offset, ESeekMode mode) override;
		virtual void flush() override;
	};

	class BufferedFile : public IFile
	{
	public:
		lucid("{490c1258-d982-4d3f-b71b-bfd02b2c5c78}");
		luiimpl(BufferedFile, IFile, IStream, IObject);
		lutsassert_lock();

		handle_t m_file;
		EStreamFlag m_flags;

		BufferedFile() :
			m_file(nullptr),
			m_flags(EStreamFlag::none) {}
		~BufferedFile()
		{
			if (m_file)
			{
				Platform::close_buffered_file(m_file);
				m_file = nullptr;
			}
		}
		virtual EStreamFlag flags() override
		{
			lutsassert();
			return m_flags;
		}
		virtual RV read(void* buffer, usize size, usize* read_bytes) override;
		virtual RV write(const void* buffer, usize size, usize* write_bytes) override;
		virtual u64 size() override;
		virtual RV set_size(u64 sz) override;
		virtual R<u64> tell() override;
		virtual RV seek(i64 offset, ESeekMode mode) override;
		virtual void flush() override;
	};

	class FileIterator final : public IFileIterator
	{
	public:
		lucid("{bd87c27c-34ed-4764-8417-6ef37c316ed3}");
		luiimpl(FileIterator, IFileIterator, IObject);
		lutsassert_lock();

		handle_t m_iter;

		FileIterator() :
			m_iter(nullptr) {}
		~FileIterator()
		{
			if (m_iter)
			{
				Platform::close_dir(m_iter);
				m_iter = nullptr;
			}
		}
		virtual bool valid() override
		{
			lutsassert();
			return Platform::dir_iterator_valid(m_iter);
		}
		virtual const c8* filename() override
		{
			lutsassert();
			return Platform::dir_iterator_filename(m_iter);
		}
		virtual EFileAttributeFlag attribute() override
		{
			lutsassert();
			return (EFileAttributeFlag)Platform::dir_iterator_attribute(m_iter);
		}
		virtual bool move_next() override
		{
			lutsassert();
			return Platform::dir_iterator_move_next(m_iter);
		}
	};
}