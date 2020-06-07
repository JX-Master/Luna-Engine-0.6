// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformFile.cpp
* @author JXMaster
* @date 2019/12/26
*/
#include "../../Platform.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include "File.hpp"
#include "AsyncFile.hpp"
#include "Heap.hpp"
#include <Base/Unicode.hpp>
#include <malloc.h>
namespace luna
{
	namespace platform
	{
		using namespace luna::win;

		char g_process_path[1024];

		void platform_file_init()
		{
			wchar_t pathbuffer[1024];
			DWORD size = ::GetModuleFileNameW(NULL, pathbuffer, 1024);
			utf16_to_utf8(g_process_path, 1024, (char16_t*)pathbuffer);
		}

		RP<IFile> open_file(const char* filename, EFileOpenFlag flags, EFileCreationMode creation)
		{
			luassert_usr(filename);

			size_t buffer_size = utf8_to_utf16_len(filename) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);

			if ((flags & EFileOpenFlag::user_buffering) != EFileOpenFlag::none)
			{
				// use buffered version.
				const wchar_t* mode;
				FILE* f = NULL;
				errno_t err;
				if (((flags & EFileOpenFlag::read) != EFileOpenFlag::none) && ((flags & EFileOpenFlag::write) != EFileOpenFlag::none))
				{
					// update mode.
					switch (creation)
					{
					case EFileCreationMode::create_always:
						mode = L"w+b";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					case EFileCreationMode::create_new:
						if (succeeded(file_exists(filename)))
						{
							return e_file_already_exists;
						}
						mode = L"w+b";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					case EFileCreationMode::open_always:
						if (succeeded(file_exists(filename)))
						{
							mode = L"r+b";
							err = _wfopen_s(&f, pathbuffer, mode);
						}
						else
						{
							mode = L"w+b";
							err = _wfopen_s(&f, pathbuffer, mode);
						}
						break;
					case EFileCreationMode::open_existing:
						mode = L"r+b";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					case EFileCreationMode::open_existing_as_new:
						if (succeeded(file_exists(filename)))
						{
							mode = L"w+b";
							err = _wfopen_s(&f, pathbuffer, mode);
						}
						else
						{
							return e_file_not_exist;
						}
						break;
					default:
						lupanic();
						break;
					}
				}
				else if (((flags & EFileOpenFlag::read) != EFileOpenFlag::none))
				{
					// read only mode,
					switch (creation)
					{
					case EFileCreationMode::create_always:
					case EFileCreationMode::create_new:
					case EFileCreationMode::open_existing_as_new:
					case EFileCreationMode::open_always:
						return e_not_available;	// Creates a new empty file and read-only from it has no meaning.
						break;
					case EFileCreationMode::open_existing:
						mode = L"rb";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					default:
						lupanic();
						break;
					}
				}
				else if (((flags & EFileOpenFlag::write) != EFileOpenFlag::none))
				{
					// write only mode.
					switch (creation)
					{
					case EFileCreationMode::create_always:
						mode = L"wb";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					case EFileCreationMode::create_new:
						if (succeeded(file_exists(filename)))
						{
							return e_file_already_exists;
						}
						mode = L"wb";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					case EFileCreationMode::open_always:
						if (succeeded(file_exists(filename)))
						{
							mode = L"r+b";
							err = _wfopen_s(&f, pathbuffer, mode);
						}
						else
						{
							mode = L"wb";
							err = _wfopen_s(&f, pathbuffer, mode);
						}
						break;
					case EFileCreationMode::open_existing:
						mode = L"r+b";
						err = _wfopen_s(&f, pathbuffer, mode);
						break;
					case EFileCreationMode::open_existing_as_new:
						if (succeeded(file_exists(filename)))
						{
							mode = L"wb";
							err = _wfopen_s(&f, pathbuffer, mode);
						}
						else
						{
							return e_file_not_exist;
						}
						break;
					default:
						lupanic();
						break;
					}
				}
				if (!f || err)
				{
					switch (err)
					{
					case EPERM:
						return e_access_denied;
					case ENOENT:
						return e_file_not_exist;
					default:
						set_err(e_bad_system_call, "fopen_s failed with err code %d", err);
						return e_user_failure;
					}
				}
				BufferedFile* file = new_obj<BufferedFile>(get_global_heap());
				file->m_file = f;
				if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
				{
					file->m_flags |= EStreamFlag::readable;
				}
				if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
				{
					file->m_flags |= EStreamFlag::writable;
					file->m_flags |= EStreamFlag::resizable;
				}
				file->m_flags |= EStreamFlag::seekable;
				file->m_flags |= EStreamFlag::buffered;
				return box_ptr<IFile>(file);
			}
			else
			{
				DWORD dw_access = 0;
				DWORD dw_creation = 0;
				if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
				{
					dw_access |= GENERIC_READ;
				}
				if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
				{
					dw_access |= GENERIC_WRITE;
				}
				switch (creation)
				{
				case EFileCreationMode::create_always:
					dw_creation = CREATE_ALWAYS;
					break;
				case EFileCreationMode::create_new:
					dw_creation = CREATE_NEW;
					break;
				case EFileCreationMode::open_always:
					dw_creation = OPEN_ALWAYS;
					break;
				case EFileCreationMode::open_existing:
					dw_creation = OPEN_EXISTING;
					break;
				case EFileCreationMode::open_existing_as_new:
					dw_creation = TRUNCATE_EXISTING;
					break;
				default:
					lupanic();
					break;
				}
				P<File> file;
				file.attach(get_global_heap()->new_obj<File>(get_global_heap()));
				
				HANDLE fileHandle = ::CreateFileW(pathbuffer, dw_access, FILE_SHARE_READ, nullptr, dw_creation, FILE_ATTRIBUTE_NORMAL, nullptr);
				if (fileHandle == INVALID_HANDLE_VALUE)
				{
					file = nullptr;
					DWORD dw = ::GetLastError();
					if (dw == ERROR_ACCESS_DENIED)
					{
						return e_access_denied;
					}
					else if (dw == ERROR_FILE_NOT_FOUND)
					{
						return e_file_not_exist;
					}
					else
					{
						return e_bad_system_call;
					}
				}
				file->m_file = fileHandle;
				if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
				{
					file->m_flags |= EStreamFlag::readable;
				}
				if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
				{
					file->m_flags |= EStreamFlag::writable;
					file->m_flags |= EStreamFlag::resizable;
				}
				file->m_flags |= EStreamFlag::seekable;
				file->m_flags |= EStreamFlag::buffered;
				return file;
			}
		}

		RP<IAsyncFile> open_file_async(const char* filename, EFileOpenFlag flags, EFileCreationMode creation)
		{
			if (!filename)
			{
				return { nullptr, e_bad_arguments };
			}
			if ((flags & EFileOpenFlag::user_buffering) != EFileOpenFlag::none)
			{
				return e_not_available;
			}
			DWORD dw_access = 0;
			DWORD dw_creation = 0;
			if ((flags & EFileOpenFlag::read) != EFileOpenFlag::none)
			{
				dw_access |= GENERIC_READ;
			}
			if ((flags & EFileOpenFlag::write) != EFileOpenFlag::none)
			{
				dw_access |= GENERIC_WRITE;
			}
			switch (creation)
			{
			case EFileCreationMode::create_always:
				dw_creation = CREATE_ALWAYS;
				break;
			case EFileCreationMode::create_new:
				dw_creation = CREATE_NEW;
				break;
			case EFileCreationMode::open_always:
				dw_creation = OPEN_ALWAYS;
				break;
			case EFileCreationMode::open_existing:
				dw_creation = OPEN_EXISTING;
				break;
			case EFileCreationMode::open_existing_as_new:
				dw_creation = TRUNCATE_EXISTING;
				break;
			default:
				lupanic();
				break;
			}
			P<AsyncFile> file;
			file.attach(get_global_heap()->new_obj<AsyncFile>(get_global_heap()));
			size_t buffer_size = utf8_to_utf16_len(filename) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);
			HANDLE fileHandle = ::CreateFileW(pathbuffer, dw_access, FILE_SHARE_READ, nullptr, dw_creation, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, nullptr);
			result_t res = s_ok;
			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				file->release();
				file = nullptr;
				DWORD dw = ::GetLastError();
				if (dw == ERROR_ACCESS_DENIED)
				{
					res = e_access_denied;
				}
				else if (dw == ERROR_FILE_NOT_FOUND)
				{
					res = e_file_not_exist;
				}
				else
				{
					res = e_bad_system_call;
				}
			}
			file->m_file = fileHandle;
			return { file, res };
		}

		uint64 file_time_to_timestamp(const FILETIME* filetime);

		RV file_attribute(const char* filename, FileAttribute& attribute)
		{
			if (!filename)
			{
				return e_bad_arguments;
			}
			size_t buffer_size = utf8_to_utf16_len(filename) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);
			WIN32_FILE_ATTRIBUTE_DATA d;
			if (!::GetFileAttributesExW(pathbuffer, GetFileExInfoStandard, &d))
			{
				return e_bad_system_call;
			}
			attribute.attributes = EFileAttributeFlag::none;
			if (d.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			{
				attribute.attributes |= EFileAttributeFlag::hidden;
			}
			if (d.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			{
				attribute.attributes |= EFileAttributeFlag::read_only;
			}
			if (d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				attribute.attributes |= EFileAttributeFlag::directory;
			}
			attribute.size = ((uint64)d.nFileSizeHigh << 32) + (uint64)d.nFileSizeLow;
			attribute.creation_time = file_time_to_timestamp(&d.ftCreationTime);
			attribute.last_access_time = file_time_to_timestamp(&d.ftLastAccessTime);
			attribute.last_write_time = file_time_to_timestamp(&d.ftLastWriteTime);
			return s_ok;
		}

		RV file_exists(const char* filename)
		{
			WIN32_FIND_DATA FindFileData;
			size_t buffer_size = utf8_to_utf16_len(filename) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);
			HANDLE handle = ::FindFirstFileW(pathbuffer, &FindFileData);
			if (handle != INVALID_HANDLE_VALUE)
			{
				::FindClose(handle);
				return s_ok;
			}
			DWORD err = ::GetLastError();
			if (err == ERROR_FILE_NOT_FOUND)
			{
				return e_file_not_exist;
			}
			return e_bad_system_call;
		}

		RV dir_exists(const char* dir_name)
		{
			WIN32_FIND_DATA FindFileData;
			size_t buffer_size = utf8_to_utf16_len(dir_name) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, dir_name);
			HANDLE handle = ::FindFirstFileW(pathbuffer, &FindFileData);
			if (handle != INVALID_HANDLE_VALUE)
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					::FindClose(handle);
					return s_ok;
				}
				::FindClose(handle);
				return e_dir_not_exist;
			}
			DWORD err = ::GetLastError();
			if (err == ERROR_FILE_NOT_FOUND)
			{
				return e_dir_not_exist;
			}
			return e_bad_system_call;
		}

		R<uint64> file_size(const char* filename)
		{
			if (!filename)
			{
				return { 0, e_bad_arguments };
			}
			DWORD access = GENERIC_READ;
			DWORD creattion = OPEN_EXISTING;
			size_t buffer_size = utf8_to_utf16_len(filename) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);
			HANDLE fileHandle = ::CreateFileW(pathbuffer, access, FILE_SHARE_READ, nullptr, creattion, FILE_ATTRIBUTE_NORMAL, nullptr);
			result_t res = s_ok;
			uint64 sz = 0;
			if (fileHandle == INVALID_HANDLE_VALUE)
			{
				DWORD dw = ::GetLastError();
				if (dw == ERROR_ACCESS_DENIED)
				{
					res = e_access_denied;
				}
				else if (dw == ERROR_FILE_NOT_FOUND)
				{
					res = e_file_not_exist;
				}
				else
				{
					res = e_bad_system_call;
				}
			}
			else
			{
				LARGE_INTEGER size;
				if (::GetFileSizeEx(fileHandle, &size) == TRUE)
				{
					sz = size.QuadPart;
				}
				::CloseHandle(fileHandle);
			}
			return { sz, res };
		}

		RV copy_file(const char* from_filename, const char* to_filename, bool fail_if_exists)
		{
			if (!from_filename || !to_filename)
			{
				return e_bad_arguments;
			}
			size_t from_size = utf8_to_utf16_len(from_filename) + 1;
			size_t to_size = utf8_to_utf16_len(to_filename) + 1;
			wchar_t* fromBuffer = (wchar_t*)alloca(sizeof(wchar_t) * from_size);
			wchar_t* toBuffer = (wchar_t*)alloca(sizeof(wchar_t) * to_size);
			utf8_to_utf16((char16_t*)fromBuffer, from_size, from_filename);
			utf8_to_utf16((char16_t*)toBuffer, to_size, to_filename);
			BOOL r = ::CopyFileW(fromBuffer, toBuffer, fail_if_exists);
			result_t res = s_ok;
			if (!r)
			{
				DWORD dw = ::GetLastError();
				if (dw == ERROR_ACCESS_DENIED)
				{
					res = e_access_denied;
				}
				else if (dw == ERROR_FILE_NOT_FOUND)
				{
					res = e_file_not_exist;
				}
				else if (dw == ERROR_ALREADY_EXISTS)
				{
					res = e_file_already_exists;
				}
				else
				{
					res = e_bad_system_call;
				}
			}
			return res;
		}

		RV move_file(const char* from_filename, const char* to_filename, bool allow_copy, bool fail_if_exists)
		{
			if (!from_filename || !to_filename)
			{
				return e_bad_arguments;
			}
			size_t from_size = utf8_to_utf16_len(from_filename) + 1;
			size_t to_size = utf8_to_utf16_len(to_filename) + 1;
			wchar_t* fromBuffer = (wchar_t*)alloca(sizeof(wchar_t) * from_size);
			wchar_t* toBuffer = (wchar_t*)alloca(sizeof(wchar_t) * to_size);
			utf8_to_utf16((char16_t*)fromBuffer, from_size, from_filename);
			utf8_to_utf16((char16_t*)toBuffer, to_size, to_filename);
			DWORD f{ 0 };
			f |= allow_copy ? MOVEFILE_COPY_ALLOWED : 0;
			f |= fail_if_exists ? MOVEFILE_REPLACE_EXISTING : 0;
			BOOL r = ::MoveFileExW(fromBuffer, toBuffer, f);
			result_t res = s_ok;
			if (!r)
			{
				DWORD dw = ::GetLastError();
				if (dw == ERROR_ACCESS_DENIED)
				{
					res = e_access_denied;
				}
				else if (dw == ERROR_FILE_NOT_FOUND)
				{
					res = e_file_not_exist;
				}
				else if (dw == ERROR_ALREADY_EXISTS)
				{
					res = e_file_already_exists;
				}
				else
				{
					res = e_bad_system_call;
				}
			}
			return res;
		}

		RV delete_file(const char* filename)
		{
			if (!filename)
			{
				return e_bad_arguments;
			}
			size_t buffer_size = utf8_to_utf16_len(filename) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);
			BOOL r = ::DeleteFileW(pathbuffer);
			result_t res = s_ok;
			if (r == 0)
			{
				DWORD dw = ::GetLastError();
				if (dw == ERROR_FILE_NOT_FOUND)
				{
					res = e_file_not_exist;
				}
				else if (dw == ERROR_ACCESS_DENIED)
				{
					res = e_access_denied;
				}
				else
				{
					res = e_bad_system_call;
				}
			}
			return res;
		}

		RP<IFileIterator> open_dir(const char* filename)
		{
			P<FileIterator> res = box_ptr(get_global_heap()->new_obj<FileIterator>(get_global_heap()));
			size_t buffer_size = utf8_to_utf16_len(filename) + 3;	// for possible "/*" and null terminator.
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, filename);
			// Append "\\*"
			if (pathbuffer[buffer_size - 4] == '/' || pathbuffer[buffer_size - 4] == '\\')
			{
				pathbuffer[buffer_size - 3] = '*';
				pathbuffer[buffer_size - 2] = 0;
			}
			else
			{
				pathbuffer[buffer_size - 3] = '/';
				pathbuffer[buffer_size - 2] = '*';
				pathbuffer[buffer_size - 1] = 0;
			}
			res->m_file_handle = ::FindFirstFileW(pathbuffer, &(res->m_data));
			if (res->m_file_handle == INVALID_HANDLE_VALUE)
			{
				DWORD err = ::GetLastError();
				if (err == ERROR_FILE_NOT_FOUND)
				{
					return e_file_not_exist;
				}
				else
				{
					return e_bad_system_call;
				}
			}
			utf16_to_utf8(res->m_file_name, 260, (char16_t*)res->m_data.cFileName);
			return res;
		}

		RV create_dir(const char* pathname)
		{
			size_t buffer_size = utf8_to_utf16_len(pathname) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, pathname);
			BOOL r = ::CreateDirectoryW(pathbuffer, 0);
			if (!r)
			{
				DWORD err = ::GetLastError();
				if (err == ERROR_ALREADY_EXISTS)
				{
					return e_dir_already_exists;
				}
				if (err == ERROR_PATH_NOT_FOUND)
				{
					return e_dir_not_exist;
				}
				return e_bad_system_call;
			}
			return s_ok;
		}

		RV remove_dir(const char* pathname)
		{
			size_t buffer_size = utf8_to_utf16_len(pathname) + 1;
			wchar_t* pathbuffer = (wchar_t*)alloca(sizeof(wchar_t) * buffer_size);
			utf8_to_utf16((char16_t*)pathbuffer, buffer_size, pathname);
			BOOL r = ::RemoveDirectoryW(pathbuffer);
			if (!r)
			{
				DWORD err = ::GetLastError();
				return e_bad_system_call;
			}
			return s_ok;
		}

		uint32 get_current_dir(uint32 buffer_length, char* buffer)
		{
			DWORD sz = ::GetCurrentDirectoryW(0, NULL);
			wchar_t* path = (wchar_t*)alloca(sizeof(wchar_t) * sz);
			::GetCurrentDirectoryW(sz, path);
			if (buffer && buffer_length)
			{
				utf16_to_utf8(buffer, buffer_length, (char16_t*)path);
			}
			return (uint32)utf16_to_utf8_len((char16_t*)path);
		}

		RV set_current_dir(const char* path)
		{
			size_t u16len = utf8_to_utf16_len(path);
			wchar_t* dpath = (wchar_t*)alloca(sizeof(wchar_t) * (u16len + 1));
			utf8_to_utf16((char16_t*)dpath, u16len + 1, path);
			if (FAILED(::SetCurrentDirectoryW(dpath)))
			{
				return e_failure;
			}
			return s_ok;
		}

		const char* get_process_path()
		{
			return g_process_path;
		}
	}
}

#endif