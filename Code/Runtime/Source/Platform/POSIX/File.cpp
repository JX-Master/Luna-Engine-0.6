// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file File.cpp
* @author JXMaster
* @date 2020/9/27
*/
#include <Runtime/PlatformDefines.hpp>
#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"
#include <Runtime/Unicode.hpp>
#include <Runtime/Algorithm.hpp>

#include <libgen.h>
#include <errno.h>

#ifdef LUNA_PLATFORM_MACOS
#include <libproc.h>
#endif

namespace Luna
{
	namespace OS
	{
		R<handle_t> open_file(const c8* path, FileOpenFlag flags, FileCreationMode creation)
		{
			lucheck(path);
			int f = 0;
			if ((flags & FileOpenFlag::read) != FileOpenFlag::none)
			{
				if ((flags & FileOpenFlag::write) != FileOpenFlag::none)
				{
					f = O_RDWR;
				}
				else
				{
					f = O_RDONLY;
				}
			}
			else
			{
				if ((flags & FileOpenFlag::write) != FileOpenFlag::none)
				{
					f = O_WRONLY;
				}
				else
				{
					return BasicError::bad_arguments();
				}
			}
			int fd;
			switch (creation)
			{
			case FileCreationMode::create_always:
				f |= O_CREAT | O_TRUNC;
				fd = open(path, f, 0666);
				break;
			case FileCreationMode::create_new:
				if (file_attribute(path).valid())
				{
					return BasicError::already_exists();
				}
				f |= O_CREAT;
				fd = open(path, f, 0666);
				break;
			case FileCreationMode::open_always:
				f |= O_CREAT;
				fd = open(path, f, 0666);
				break;
			case FileCreationMode::open_existing:
				fd = open(path, f, 0);
				break;
			case FileCreationMode::open_existing_as_new:
				f |= O_TRUNC;
				fd = open(path, f, 0);
				break;
			}
			if (fd == -1)
			{
				errno_t err = errno;
				switch (err)
				{
				case EPERM:
					return BasicError::access_denied();
				case ENOENT:
					return BasicError::not_found();
				default:
					return BasicError::bad_system_call();
				}
			}
			return R<handle_t>::success((handle_t)fd);
		}
		void close_file(handle_t file)
		{
			int fd = (int)file;
			::close(fd);
		}
		RV read_file(handle_t file, void* buffer, usize size, usize* read_bytes)
		{
			int fd = (int)file;
			usize sz = ::read(fd, buffer, size);
			if (sz == -1)
			{
				if (read_bytes)
				{
					*read_bytes = 0;
				}
				return BasicError::bad_system_call();
			}
			if (read_bytes)
			{
				*read_bytes = sz;
			}
			return RV();
		}
		RV write_file(handle_t file, const void* buffer, usize size, usize* write_bytes)
		{
			int fd = (int)file;
			usize sz = ::write(fd, buffer, size);
			if (sz == -1)
			{
				if (write_bytes)
				{
					*write_bytes = 0;
				}
				return BasicError::bad_system_call();
			}
			if (write_bytes)
			{
				*write_bytes = sz;
			}
			return RV();
		}
		R<u64> get_file_size(handle_t file)
		{
			int fd = (int)file;
			struct stat st;
			if (!fstat(fd, &st))
			{
				return R<u64>::success(st.st_size);
			}
			return R<u64>::failure(BasicError::bad_system_call());
		}
		RV set_file_size(handle_t file, u64 sz)
		{
			int fd = (int)file;
			return ftruncate(fd, sz) ? BasicError::bad_system_call() : RV();
		}
		R<u64> get_file_cursor(handle_t file)
		{
			int fd = (int)file;
			off_t r = lseek(fd, 0, SEEK_CUR);
			if (r == (off_t)-1)
			{
				return R<u64>::failure(BasicError::bad_system_call());
			}
			return R<u64>::success((u64)r);
		}
		RV set_file_cursor(handle_t file, i64 offset, SeekMode mode)
		{
			int fd = (int)file;
			int origin;
			switch (mode)
			{
			case SeekMode::begin:
				origin = SEEK_SET;
				break;
			case SeekMode::current:
				origin = SEEK_CUR;
				break;
			case SeekMode::end:
				origin = SEEK_END;
				break;
			}
			off_t r = lseek(fd, offset, origin);
			if (r == (off_t)-1)
			{
				return BasicError::bad_system_call();
			}
			return RV();
		}
		RV flush_file(handle_t file)
		{
			int fd = (int)file;
			int r = fsync(fd);
			return r == 0 ? RV() : BasicError::bad_system_call();
		}
		R<handle_t> open_buffered_file(const c8* path, FileOpenFlag flags, FileCreationMode creation)
		{
			// use buffered version.
			const char* mode;
			FILE* f = NULL;
			errno_t err;
			if (((flags & FileOpenFlag::read) != FileOpenFlag::none) && ((flags & FileOpenFlag::write) != FileOpenFlag::none))
			{
				// update mode.
				switch (creation)
				{
				case FileCreationMode::create_always:
					mode = "w+b";
					f = fopen(path, mode);
					break;
				case FileCreationMode::create_new:
					if (file_attribute(path).valid())
					{
						return BasicError::already_exists();
					}
					mode = "w+b";
					f = fopen(path, mode);
					break;
				case FileCreationMode::open_always:
					if (file_attribute(path).valid())
					{
						mode = "r+b";
						f = fopen(path, mode);
					}
					else
					{
						mode = "w+b";
						f = fopen(path, mode);
					}
					break;
				case FileCreationMode::open_existing:
					mode = "r+b";
					f = fopen(path, mode);
					break;
				case FileCreationMode::open_existing_as_new:
					if (file_attribute(path).valid())
					{
						mode = "w+b";
						f = fopen(path, mode);
					}
					else
					{
						return BasicError::not_found();
					}
					break;
				default:
					lupanic();
					break;
				}
			}
			else if (((flags & FileOpenFlag::read) != FileOpenFlag::none))
			{
				// read only mode,
				switch (creation)
				{
				case FileCreationMode::create_always:
				case FileCreationMode::create_new:
				case FileCreationMode::open_existing_as_new:
				case FileCreationMode::open_always:
					return BasicError::bad_arguments();    // Creates a new empty file and read-only from it has no meaning.
					break;
				case FileCreationMode::open_existing:
					mode = "rb";
					f = fopen(path, mode);
					break;
				default:
					lupanic();
					break;
				}
			}
			else if (((flags & FileOpenFlag::write) != FileOpenFlag::none))
			{
				// write only mode.
				switch (creation)
				{
				case FileCreationMode::create_always:
					mode = "wb";
					f = fopen(path, mode);
					break;
				case FileCreationMode::create_new:
					if (platform_file_attribute(path).valid())
					{
						return BasicError::already_exists();
					}
					mode = "wb";
					f = fopen(path, mode);
					break;
				case FileCreationMode::open_always:
					if (platform_file_attribute(path).valid())
					{
						mode = "r+b";
						f = fopen(path, mode);
					}
					else
					{
						mode = "wb";
						f = fopen(path, mode);
					}
					break;
				case FileCreationMode::open_existing:
					mode = "r+b";
					f = fopen(path, mode);
					break;
				case FileCreationMode::open_existing_as_new:
					if (platform_file_attribute(path).valid())
					{
						mode = "wb";
						f = fopen(path, mode);
					}
					else
					{
						return BasicError::not_found();
					}
					break;
				default:
					lupanic();
					break;
				}
			}
			if (!f)
			{
				err = errno;
				switch (err)
				{
				case EPERM:
					return BasicError::access_denied();
				case ENOENT:
					return BasicError::not_found();
				default:
					//get_error().set(e_bad_system_call, "fopen failed with err code %d", err);
					return BasicError::bad_system_call();
				}
			}
			return R<handle_t>::success(f);
		}
		void close_buffered_file(handle_t file)
		{
			FILE* f = (FILE*)file;
			fclose(f);
		}
		RV read_buffered_file(handle_t file, void* buffer, usize size, usize* read_bytes)
		{
			FILE* f = (FILE*)file;
			usize sz = fread(buffer, 1, size, f);
			if (read_bytes)
			{
				*read_bytes = sz;
			}
			if (sz != size)
			{
				if (feof(f))
				{
					clearerr(f);
					return RV();
				}
				clearerr(f);
				return BasicError::bad_system_call();
			}
			return RV();
		}
		RV write_buffered_file(handle_t file, const void* buffer, usize size, usize* write_bytes)
		{
			FILE* f = (FILE*)file;
			usize sz = fwrite(buffer, 1, size, f);
			if (write_bytes)
			{
				*write_bytes = sz;
			}
			if (sz != size)
			{
				clearerr(f);
				return BasicError::bad_system_call();
			}
			return RV();
		}
		R<u64> get_buffered_file_size(handle_t file)
		{
			FILE* f = (FILE*)file;
			int fd = fileno(f);
			struct stat st;
			if (!fstat(fd, &st))
			{
				return R<u64>::success(st.st_size);
			}
			return R<u64>::failure(BasicError::bad_system_call());
		}
		RV set_buffered_file_size(handle_t file, u64 sz)
		{
			FILE* f = (FILE*)file;
			int fd = fileno(f);
			return ftruncate(fd, sz) ? BasicError::bad_system_call() : RV();
		}
		R<u64> get_buffered_file_cursor(handle_t file)
		{
			FILE* f = (FILE*)file;
			long r = ftell(f);
			if (r < 0)
			{
				clearerr(f);
				return R<u64>::failure(BasicError::bad_system_call());
			}
			return R<u64>::success((u64)r);
		}
		RV set_buffered_file_cursor(handle_t file, i64 offset, SeekMode mode)
		{
			FILE* f = (FILE*)file;
			int origin;
			switch (mode)
			{
			case SeekMode::begin:
				origin = SEEK_SET;
				break;
			case SeekMode::current:
				origin = SEEK_CUR;
				break;
			case SeekMode::end:
				origin = SEEK_END;
				break;
			}
			if (fseek(f, offset, origin))
			{
				clearerr(f);
				return BasicError::bad_system_call();
			}
			return RV();
		}
		RV flush_buffered_file(handle_t file)
		{
			FILE* f = (FILE*)file;
			if (fflush(f))
			{
				clearerr(f);
				return BasicError::bad_system_call();
			}
			return RV();
		}

		R<FileAttribute> file_attribute(const c8* path)
		{
			struct stat s;
			int r = stat(path, &s);
			if (r != 0)
			{
				return BasicError::bad_system_call();
			}
			FileAttribute attribute;
			attribute.size = s.st_size;
			attribute.last_access_time = s.st_atime;
			attribute.last_write_time = s.st_mtime;
#ifdef LUNA_PLATFORM_MACOS
			attribute.creation_time = s.st_birthtime;
#else
			attribute.creation_time = 0;
#endif
			attribute.attributes = EFileAttributeFlag::none;
			if (S_ISDIR(s.st_mode))
			{
				attribute.attributes |= EFileAttributeFlag::directory;
			}
			if (S_ISCHR(s.st_mode))
			{
				attribute.attributes |= EFileAttributeFlag::character_special;
			}
			if (S_ISBLK(s.st_mode))
			{
				attribute.attributes |= EFileAttributeFlag::block_special;
			}
			return attribute;
		}
		RV copy_file(const c8* src_path, const c8* dest_path, bool fail_if_exists)
		{
			lucheck(src_path && dest_path);
			constexpr u64 max_buffer_sz = 1_mb;
			u8* buf = (u8*)memalloc(max_buffer_sz);
			handle_t from_file = nullptr;
			handle_t to_file = nullptr;
			lutry
			{
				luset(from_file, open_file(src_path, FileOpenFlag::read, FileCreationMode::open_existing));
				if (fail_if_exists)
				{
					luset(to_file, open_file(dest_path, FileOpenFlag::write, FileCreationMode::create_new));
				}
				else
				{
					luset(to_file, open_file(dest_path, FileOpenFlag::write, FileCreationMode::create_always));
				}
				lulet(copy_size, get_file_size(from_file));
				u64 sz = copy_size;

				while (sz)
				{
					usize copy_size_onetime = min<usize>(sz, max_buffer_sz);
					luexp(read_file(from_file, buf, copy_size_onetime, nullptr));
					luexp(write_file(to_file, buf, copy_size_onetime, nullptr));
					sz -= copy_size_onetime;
				}
			}
				lucatch
			{
				if (buf)
				{
					memfree(buf);
					buf = nullptr;
				}
				if (from_file)
				{
					close_file(from_file);
				}
				if (to_file)
				{
					close_file(to_file);
				}
				return lures;
			}
				if (buf)
				{
					memfree(buf);
					buf = nullptr;
				}
			if (from_file)
			{
				close_file(from_file);
			}
			if (to_file)
			{
				close_file(to_file);
			}
			return RV();
		}
		RV move_file(const c8* src_path, const c8* dest_path, bool allow_copy, bool fail_if_exists)
		{
			if (fail_if_exists && file_attribute(dest_path).valid())
			{
				return BasicError::already_exists();
			}
			int res = ::rename(src_path, dest_path);
			if (res != 0 && allow_copy)
			{
				// Try to copy&delete.
				lutry
				{
					luexp(copy_file(src_path, dest_path, fail_if_exists));
					luexp(delete_file(src_path));
				}
				lucatchret;
			}
			else
			{
				return (res == 0) ? RV() : BasicError::bad_system_call();
			}
		}
		RV delete_file(const c8* path)
		{
			int res = ::remove(path);
			return (res == 0) ? RV() : BasicError::bad_system_call();
		}
		struct FileData
		{
			DIR* m_dir;
			struct dirent* m_dirent;
		}
		R<handle_t> open_dir(const c8* path)
		{
			DIR* dir = ::opendir(path);
			if (dir == NULL)
			{
				errno_t err = errno;
				switch (err)
				{
				case EACCES:
					return BasicError::access_denied();
				case EMFILE:
				case ENFILE:
					return BasicError::busy();
				case ENOENT:
					return BasicError::not_found();
				case ENOMEM:
					return BasicError::bad_memory_alloc();
				case ENOTDIR:
					return BasicError::not_directory();
				default:
					return BasicError::bad_system_call();
				}
			}
			FileData* iter = memnew<FileData>();
			iter->m_dir = dir;
			iter->m_dirent = ::readdir(dir);
			return R<handle_t>::success(iter);;
		}
		void close_dir(handle_t dir_iter)
		{
			FileData* data = (FileData*)dir_iter;
			closedir(data->m_dir);
			memdelete(data);
		}
		bool dir_iterator_valid(handle_t dir_iter)
		{
			FileData* data = (FileData*)dir_iter;
			return data->m_dirent != nullptr;
		}
		const c8* dir_iterator_filename(handle_t dir_iter)
		{
			FileData* data = (FileData*)dir_iter;
			if (data->m_dirent)
			{
				return data->m_dirent->d_name;
			}
			return nullptr;
		}
		EFileAttributeFlag dir_iterator_attribute(handle_t dir_iter)
		{
			FileData* data = (FileData*)dir_iter;
			EFileAttributeFlag flags = EFileAttributeFlag::none;
			if (data->m_dirent)
			{
				if (DT_BLK & data->m_dirent->d_type)
				{
					flags |= EFileAttributeFlag::block_special;
				}
				if (DT_CHR & data->m_dirent->d_type)
				{
					flags |= EFileAttributeFlag::character_special;
				}
				if (DT_DIR & data->m_dirent->d_type)
				{
					flags |= EFileAttributeFlag::directory;
				}	
			}
			return flags;
		}
		bool dir_iterator_move_next(handle_t dir_iter)
		{
			FileData* data = (FileData*)dir_iter;
			if (data->m_dirent)
			{
				data->m_dirent = ::readdir(data->m_dir);
			}
			return data->m_dirent != nullptr;
		}
		RV create_dir(const c8* path)
		{
			int r = mkdir(path, 0755);
			if (r != 0)
			{
				errno_t err = errno;
				switch (errno)
				{
				case EACCES:
					return BasicError::access_denied();
				case EEXIST:
					return BasicError::already_exists();
				case ENAMETOOLONG:
					return BasicError::data_too_long();
				case ENOENT:
					return BasicError::not_found();
				case ENOTDIR:
					return BasicError::not_directory();
				default:
					return BasicError::bad_system_call();
				}
			}
			return RV();
		}
		RV remove_dir(const c8* path)
		{
			int r = rmdir(path);
			if (r != 0)
			{
				errno_t err = errno;
				switch (errno)
				{
				case EACCES:
					return BasicError::access_denied();
				case EBUSY:
					return BasicError::busy();
				case ENAMETOOLONG:
					return BasicError::data_too_long();
				case ENOENT:
					return BasicError::not_found();
				case ENOTDIR:
					return BasicError::not_directory();
				default:
					return BasicError::bad_system_call();
				}
			}
			return RV();
		}
		u32 get_current_dir(u32 buffer_length, c8* buffer)
		{
			char* path = ::getcwd(nullptr, 0);
			u32 len = (u32)strlen(path);
			if (buffer && buffer_length)
			{
				strncpy(buffer, path, buffer_length);
			}
			::free(path);
			return len;
		}
		RV set_current_dir(const c8* path)
		{
			int r = ::chdir(path);
			if (r != 0)
			{
				errno_t err = errno;
				switch (errno)
				{
				case EACCES:
					return BasicError::access_denied();
				case ENAMETOOLONG:
					return BasicError::data_too_long();
				case ENOENT:
					return BasicError::not_found();
				case ENOTDIR:
					return BasicError::not_directory();
				default:
					return BasicError::bad_system_call();
				}
			}
			return RV();
		}
		c8 g_process_path[1024];

		void file_init()
		{
#ifdef LUNA_PLATFORM_LINUX
			char path[1024];
			luassert_always(readlink("/proc/self/exe", path, 1024) != -1);
			char* dir = dirname(path);
			strlcpy(g_process_path, dir, 1024);
#else
			pid_t pid = getpid();
			int ret = proc_pidpath(pid, g_process_path, sizeof(g_process_path));
			luassert_always(ret > 0);
#endif
		}

		const c8* get_process_path()
		{
			return g_process_path;
		}
    }
}
#endif