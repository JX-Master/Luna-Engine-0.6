// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file File.hpp
* @author JXMaster
* @date 2019/9/29
*/
#pragma once
#include "../../CoreHeader.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <Base/TSAssert.hpp>

namespace luna
{
	namespace win
	{
		class File final : public IFile
		{
		public:
			lucid("{915247e4-15b4-44ba-8781-dd7dcfd48f87}");
			luiimpl(File, IFile, IStream, IObject);
			lutsassert_lock();

			HANDLE m_file;
			EStreamFlag m_flags;

			File(IAllocator* alloc) :
				luibind(alloc),
				m_file(INVALID_HANDLE_VALUE),
				m_flags(EStreamFlag::none) {}
			~File()
			{
				if (m_file != INVALID_HANDLE_VALUE)
				{
					::CloseHandle(m_file);
					m_file = INVALID_HANDLE_VALUE;
				}
			}
			virtual EStreamFlag flags() override
			{
				lutsassert();
				return m_flags;
			}
			virtual RV read(void* buffer, size_t size, size_t* read_bytes) override;
			virtual RV write(const void* buffer, size_t size, size_t* write_bytes) override;
			virtual R<uint64> size() override;
			virtual RV set_size(uint64 sz) override;
			virtual R<uint64> tell() override;
			virtual RV seek(int64 offset, ESeekMode mode) override;
			virtual RV flush() override;
		};

		class BufferedFile : public IFile
		{
		public:
			lucid("{490c1258-d982-4d3f-b71b-bfd02b2c5c78}");
			luiimpl(BufferedFile, IFile, IStream, IObject);
			lutsassert_lock();

			FILE* m_file;
			EStreamFlag m_flags;

			BufferedFile(IAllocator* alloc) :
				luibind(alloc),
				m_file(NULL) {}
			~BufferedFile()
			{
				if (m_file)
				{
					fclose(m_file);
					m_file = NULL;
				}
			}
			virtual EStreamFlag flags() override
			{
				lutsassert();
				return m_flags;
			}
			virtual RV read(void* buffer, size_t size, size_t* read_bytes) override;
			virtual RV write(const void* buffer, size_t size, size_t* write_bytes) override;
			virtual R<uint64> size() override;
			virtual RV set_size(uint64 sz) override;
			virtual R<uint64> tell() override;
			virtual RV seek(int64 offset, ESeekMode mode) override;
			virtual RV flush() override;
		};

		class FileIterator final : public IFileIterator
		{
		public:
			lucid("{bd87c27c-34ed-4764-8417-6ef37c316ed3}");
			luiimpl(FileIterator, IFileIterator, IObject);
			lutsassert_lock();

			WIN32_FIND_DATAW m_data;
			HANDLE m_file_handle;
			const char* m_filename_ptr;
			char m_file_name[260];	// Buffer to store the file name in UTF-8 format.
			bool m_valid;

			FileIterator(IAllocator* alloc) :
				luibind(alloc),
				m_valid(true),
				m_file_handle(INVALID_HANDLE_VALUE)
			{
				m_file_name[0] = 0;
				m_filename_ptr = m_file_name;
			}
			~FileIterator()
			{
				if (m_file_handle != INVALID_HANDLE_VALUE)
				{
					::FindClose(m_file_handle);
					m_file_handle = INVALID_HANDLE_VALUE;
				}
			}

			virtual bool valid()
			{
				return m_valid;
			}

			virtual const char* filename() override;

			virtual EFileAttributeFlag attribute() override;

			virtual bool move_next() override;
		};
	}
}

#endif