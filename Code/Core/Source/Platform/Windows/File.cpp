// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file File.cpp
* @author JXMaster
* @date 2019/9/29
*/
#include "File.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <io.h>
#include <Base/Unicode.hpp>

namespace luna
{
	namespace win
	{
		RV File::read(void* buffer, size_t size, size_t* read_bytes)
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			DWORD actual = 0;
			BOOL s = ::ReadFile(m_file, buffer, (DWORD)size, &actual, nullptr);
			if (read_bytes)
			{
				*read_bytes = actual;
			}
			if (s)
			{
				return s_ok;
			}
			DWORD err = ::GetLastError();
			return e_bad_system_call;
		}
		RV File::write(const void* buffer, size_t size, size_t* write_bytes)
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			DWORD actual;
			BOOL s = ::WriteFile(m_file, buffer, (DWORD)size, &actual, nullptr);
			if (write_bytes)
			{
				*write_bytes = actual;
			}
			if (s)
			{
				return s_ok;
			}
			DWORD err = ::GetLastError();
			return e_bad_system_call;
		}
		R<uint64> File::size()
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			LARGE_INTEGER size;
			if (::GetFileSizeEx(m_file, &size))
			{
				return R<uint64>::success(size.QuadPart, s_ok);
			}
			return R<uint64>::failure(e_bad_system_call);
		}
		RV File::set_size(uint64 sz)
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			LARGE_INTEGER old_cursor;
			LARGE_INTEGER cursor;
			LARGE_INTEGER end;
			end.QuadPart = (LONGLONG)sz;
			if (!::GetFileSizeEx(m_file, &old_cursor))
			{
				return e_bad_system_call;
			}
			if (!::SetFilePointerEx(m_file, end, &cursor, FILE_BEGIN))
			{
				return e_bad_system_call;
			}
			if (!::SetEndOfFile(m_file))
			{
				return e_bad_system_call;
			}
			if (!::SetFilePointerEx(m_file, old_cursor, &cursor, FILE_BEGIN))
			{
				return e_bad_system_call;
			}
			return s_ok;
		}
		R<uint64> File::tell()
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			LARGE_INTEGER cursor;
			LARGE_INTEGER movement;
			movement.QuadPart = 0;
			if (::SetFilePointerEx(m_file, movement, &cursor, FILE_CURRENT) == 0)
			{
				DWORD err = ::GetLastError();
				return R<uint64>::failure(e_bad_system_call);
			}
			return R<uint64>::success(cursor.QuadPart, s_ok);
		}
		RV File::seek(int64 offset, ESeekMode mode)
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			LARGE_INTEGER cursor;
			LARGE_INTEGER movement;
			movement.QuadPart = offset;
			DWORD method;
			switch (mode)
			{
			case ESeekMode::begin:
				method = FILE_BEGIN;
				break;
			case ESeekMode::current:
				method = FILE_CURRENT;
				break;
			case ESeekMode::end:
				method = FILE_END;
				break;
			}
			if (::SetFilePointerEx(m_file, movement, &cursor, method) == 0)
			{
				DWORD err = ::GetLastError();
				return e_bad_system_call;
			}
			return s_ok;
		}
		RV File::flush()
		{
			lutsassert();
			luassert(m_file != INVALID_HANDLE_VALUE);
			return ::FlushFileBuffers(m_file) ? s_ok : e_bad_system_call;
		}
		RV BufferedFile::read(void* buffer, size_t size, size_t* read_bytes)
		{
			lutsassert();
			size_t sz = _fread_nolock(buffer, 1, size, m_file);
			if (read_bytes)
			{
				*read_bytes = sz;
			}
			if (sz != size)
			{
				if (feof(m_file))
				{
					clearerr(m_file);
					return s_ok;
				}
				clearerr(m_file);
				return e_failure;
			}
			return s_ok;
		}
		RV BufferedFile::write(const void* buffer, size_t size, size_t* write_bytes)
		{
			lutsassert();
			size_t sz = _fwrite_nolock(buffer, 1, size, m_file);
			if (write_bytes)
			{
				*write_bytes = sz;
			}
			if (sz != size)
			{
				clearerr(m_file);
				return e_failure;
			}
			return s_ok;
		}
		R<uint64> BufferedFile::size()
		{
			lutsassert();
			HANDLE h = (HANDLE)_get_osfhandle(_fileno(m_file));
			LARGE_INTEGER size;
			if (::GetFileSizeEx(h, &size))
			{
				return R<uint64>::success(size.QuadPart, s_ok);
			}
			return R<uint64>::failure(e_bad_system_call);
		}
		RV BufferedFile::set_size(uint64 sz)
		{
			lutsassert();
			HANDLE h = (HANDLE)_get_osfhandle(_fileno(m_file));
			LARGE_INTEGER old_cursor;
			LARGE_INTEGER cursor;
			LARGE_INTEGER end;
			end.QuadPart = (LONGLONG)sz;
			if (!::GetFileSizeEx(h, &old_cursor))
			{
				return e_bad_system_call;
			}
			if (!::SetFilePointerEx(h, end, &cursor, FILE_BEGIN))
			{
				return e_bad_system_call;
			}
			if (!::SetEndOfFile(h))
			{
				return e_bad_system_call;
			}
			if (!::SetFilePointerEx(h, old_cursor, &cursor, FILE_BEGIN))
			{
				return e_bad_system_call;
			}
			return s_ok;
		}
		R<uint64> BufferedFile::tell()
		{
			lutsassert();
			__int64 cur = _ftelli64_nolock(m_file);
			if (cur < 0)
			{
				clearerr(m_file);
				return R<uint64>::failure(e_bad_system_call);
			}
			return R<uint64>::success((uint64)cur);
		}
		RV BufferedFile::seek(int64 offset, ESeekMode mode)
		{
			lutsassert();
			int origin;
			switch (mode)
			{
			case ESeekMode::begin:
				origin = SEEK_SET;
				break;
			case ESeekMode::current:
				origin = SEEK_CUR;
				break;
			case ESeekMode::end:
				origin = SEEK_END;
				break;
			default:
				lupanic();
				break;
			}
			if (_fseeki64_nolock(m_file, offset, origin))
			{
				clearerr(m_file);
				return e_bad_system_call;
			}
			return s_ok;
		}
		RV BufferedFile::flush()
		{
			lutsassert();
			if (_fflush_nolock(m_file))
			{
				clearerr(m_file);
				return e_bad_system_call;
			}
			return s_ok;
		}
		const char* FileIterator::filename()
		{
			lutsassert();
			if (m_valid)
			{
				return m_filename_ptr;
			}
			else
			{
				return nullptr;
			}
		}
		EFileAttributeFlag FileIterator::attribute()
		{
			lutsassert();
			if (!valid())
			{
				return EFileAttributeFlag::none;
			}
			DWORD attrs = m_data.dwFileAttributes;
			EFileAttributeFlag r = EFileAttributeFlag::none;
			if (attrs & FILE_ATTRIBUTE_HIDDEN)
			{
				r |= EFileAttributeFlag::hidden;
			}
			if (attrs & FILE_ATTRIBUTE_READONLY)
			{
				r |= EFileAttributeFlag::read_only;
			}
			if (attrs & FILE_ATTRIBUTE_DIRECTORY)
			{
				r |= EFileAttributeFlag::directory;
			}
			return r;
		}
		bool FileIterator::move_next()
		{
			lutsassert();
			if (!valid())
			{
				return false;
			}
			if (::FindNextFileW(m_file_handle, &m_data) == 0)
			{
				m_valid = false;
				return false;
			}
			utf16_to_utf8(m_file_name, 260, (char16_t*)m_data.cFileName);
			m_valid = true;
			return true;
		}
	}
}

#endif