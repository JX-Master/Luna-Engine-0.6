// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file File.cpp
* @author JXMaster
* @date 2019/9/29
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "File.hpp"
#include <Runtime/Unicode.hpp>

namespace Luna
{
	RV File::read(void* buffer, usize size, usize* read_bytes)
	{
		lutsassert();
		luassert(m_file);
		return Platform::read_file(m_file, buffer, size, read_bytes);
	}
	RV File::write(const void* buffer, usize size, usize* write_bytes)
	{
		lutsassert();
		luassert(m_file);
		return Platform::write_file(m_file, buffer, size, write_bytes);
	}
	u64 File::size()
	{
		lutsassert();
		luassert(m_file);
		auto r = Platform::get_file_size(m_file);
		return failed(r) ? 0 : r.get();
	}
	RV File::set_size(u64 sz)
	{
		lutsassert();
		luassert(m_file);
		return Platform::set_file_size(m_file, sz);
	}
	R<u64> File::tell()
	{
		lutsassert();
		luassert(m_file);
		return Platform::get_file_cursor(m_file);
	}
	RV File::seek(i64 offset, ESeekMode mode)
	{
		lutsassert();
		luassert(m_file);
		return Platform::set_file_cursor(m_file, offset, (Platform::SeekMode)mode);
	}
	void File::flush()
	{
		lutsassert();
		luassert(m_file);
		auto _ = Platform::flush_file(m_file);
	}
	RV BufferedFile::read(void* buffer, usize size, usize* read_bytes)
	{
		lutsassert();
		luassert(m_file);
		return Platform::read_buffered_file(m_file, buffer, size, read_bytes);
	}
	RV BufferedFile::write(const void* buffer, usize size, usize* write_bytes)
	{
		lutsassert();
		luassert(m_file);
		return Platform::write_buffered_file(m_file, buffer, size, write_bytes);
	}
	u64 BufferedFile::size()
	{
		lutsassert();
		luassert(m_file);
		auto r = Platform::get_buffered_file_size(m_file);
		return failed(r) ? 0 : r.get();
	}
	RV BufferedFile::set_size(u64 sz)
	{
		lutsassert();
		luassert(m_file);
		return Platform::set_buffered_file_size(m_file, sz);
	}
	R<u64> BufferedFile::tell()
	{
		lutsassert();
		luassert(m_file);
		return Platform::get_buffered_file_cursor(m_file);
	}
	RV BufferedFile::seek(i64 offset, ESeekMode mode)
	{
		lutsassert();
		luassert(m_file);
		return Platform::set_buffered_file_cursor(m_file, offset, (Platform::SeekMode)mode);
	}
	void BufferedFile::flush()
	{
		lutsassert();
		luassert(m_file);
		auto _ = Platform::flush_buffered_file(m_file);
	}
	LUNA_CORE_API RP<IFile>	platform_open_file(const c8* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		lucheck(filename);

		if ((flags & EFileOpenFlag::user_buffering) != EFileOpenFlag::none)
		{
			auto h = Platform::open_buffered_file(filename, (Platform::FileOpenFlag)flags, (Platform::FileCreationMode)creation);
			if (failed(h))
			{
				return h.errcode();
			}
			auto f = newobj<BufferedFile>();
			f->m_file = h.get();
			if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
			{
				f->m_flags |= EStreamFlag::readable;
			}
			if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
			{
				f->m_flags |= EStreamFlag::writable;
				f->m_flags |= EStreamFlag::resizable;
			}
			f->m_flags |= EStreamFlag::seekable;
			f->m_flags |= EStreamFlag::buffered;
			return f;
		}
		else
		{
			auto h = Platform::open_file(filename, (Platform::FileOpenFlag)flags, (Platform::FileCreationMode)creation);
			if (failed(h))
			{
				return h.errcode();
			}
			auto f = newobj<File>();
			f->m_file = h.get();
			if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
			{
				f->m_flags |= EStreamFlag::readable;
			}
			if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
			{
				f->m_flags |= EStreamFlag::writable;
				f->m_flags |= EStreamFlag::resizable;
			}
			f->m_flags |= EStreamFlag::seekable;
			f->m_flags |= EStreamFlag::buffered;
			return f;
		}
	}
	LUNA_CORE_API R<FileAttribute> platform_file_attribute(const c8* filename)
	{
		lucheck(filename);
		auto r = Platform::file_attribute(filename);
		if (failed(r))
		{
			return r.errcode();
		}
		FileAttribute attr;
		attr.attributes = (EFileAttributeFlag)r.get().attributes;
		attr.creation_time = r.get().creation_time;
		attr.last_access_time = r.get().last_access_time;
		attr.last_write_time = r.get().last_write_time;
		attr.size = r.get().size;
		return attr;
	}
	LUNA_CORE_API RV	platform_copy_file(const c8* from_filename, const c8* to_filename, bool fail_if_exists)
	{
		lucheck(from_filename && to_filename);
		return Platform::copy_file(from_filename, to_filename, fail_if_exists);
	}
	LUNA_CORE_API RV	platform_move_file(const c8* from_filename, const c8* to_filename, bool allow_copy, bool fail_if_exists)
	{
		lucheck(from_filename && to_filename);
		return Platform::move_file(from_filename, to_filename, allow_copy, fail_if_exists);
	}
	LUNA_CORE_API RV	platform_delete_file(const c8* filename)
	{
		lucheck(filename);
		return Platform::delete_file(filename);
	}
	LUNA_CORE_API RP<IFileIterator> platform_open_dir(const c8* dir_path)
	{
		lucheck(dir_path);
		auto r = Platform::open_dir(dir_path);
		if (failed(r))
		{
			return r.errcode();
		}
		auto iter = newobj<FileIterator>();
		iter->m_iter = r.get();
		return iter;
	}
	LUNA_CORE_API RV	platform_create_dir(const c8* pathname)
	{
		return Platform::create_dir(pathname);
	}
	LUNA_CORE_API RV	platform_remove_dir(const c8* pathname)
	{
		return Platform::remove_dir(pathname);
	}
	LUNA_CORE_API u32	get_current_dir(u32 buffer_length, c8* buffer)
	{
		return Platform::get_current_dir(buffer_length, buffer);
	}
	LUNA_CORE_API RV set_current_dir(const c8* path)
	{
		return Platform::set_current_dir(path);
	}
	LUNA_CORE_API const c8* get_process_path()
	{
		return Platform::get_process_path();
	}
}