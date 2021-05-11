// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Vfs.cpp
* @author JXMaster
* @date 2020/1/23
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "Vfs.hpp"
#include "../Core.hpp"

namespace Luna
{
	P<IMutex> m_lock;
	Unconstructed<Vector<MountPair>> m_mounts;

	void vfs_init()
	{
		m_lock = new_mutex();
		m_mounts.construct();
	}
	void vfs_deinit()
	{
		m_mounts.destruct();
		m_lock = nullptr;
	}

	LUNA_CORE_API P<IFileSystem> route_path(const Path& filename, Path& mount_point, Path& fs_path)
	{
		auto iter = m_mounts.get().rbegin();
		while (iter != m_mounts.get().rend())
		{
			if (filename.is_subpath_of(iter->m_path))
			{
				// matched.
				auto fs = iter->m_fs;
				mount_point = iter->m_path;
				fs_path = Path();
				fs_path.assign_relative(mount_point, filename);
				return fs;
			}
			else
			{
				++iter;
			}
		}
		// Not found.
		return nullptr;
	}
	LUNA_CORE_API RV copy_file_between_fs(IFileSystem* from, IFileSystem* to, const Path& from_path, const Path& to_path, bool fail_if_exists)
	{
		// try open file.
		P<IFile> from_file;
		P<IFile> to_file;
		u64 file_sz;
		lutry
		{
			luset(from_file, from->open_file(from_path, EFileOpenFlag::read, EFileCreationMode::open_existing));
			if (fail_if_exists)
			{
				luset(to_file, to->open_file(to_path, EFileOpenFlag::write, EFileCreationMode::create_new));
			}
			else
			{
				luset(to_file, to->open_file(to_path, EFileOpenFlag::write, EFileCreationMode::create_always));
			}
			file_sz = from_file->size();
			luexp(to_file->set_size(file_sz));
		}
			lucatch
		{
			from_file = nullptr;
			to_file = nullptr;
			auto _ = to->delete_file(to_path);
			return lures;
		}

			// preparing buffer.
		void* buf = nullptr;
		if (file_sz >= 16_mb)
		{
			buf = memalloc((usize)16_mb);
		}
		else
		{
			buf = memalloc((usize)file_sz);
		}
		if (!buf)
		{
			from_file = nullptr;
			to_file = nullptr;
			auto _ = to->delete_file(to_path);
			return BasicError::bad_memory_alloc();
		}

		// copying.
		lutry2
		{
			if (file_sz >= 16_mb)
			{
				for (u64 i = 0; i < file_sz; i += 16_mb)
				{
					usize bytes_to_read = (usize)min(16_mb, file_sz - i);
					usize bytes_read;
					luexp2(from_file->read(buf, bytes_to_read, &bytes_read));
					luassert(bytes_to_read == bytes_read);
					luexp2(to_file->write(buf, bytes_to_read, &bytes_read));
					luassert(bytes_to_read == bytes_read);
				}
			}
			else
			{
				usize bytes_to_read = (usize)file_sz;
				usize bytes_read;
				luexp2(from_file->read(buf, bytes_to_read, &bytes_read));
				luassert(bytes_to_read == bytes_read);
				luexp2(to_file->write(buf, bytes_to_read, &bytes_read));
				luassert(bytes_to_read == bytes_read);
			}
		}
			lucatch2
		{
			// read or write failed.
			memfree(buf);
			from_file = nullptr;
			to_file = nullptr;
			auto _ = to->delete_file(to_path);
			return lures2;
		}
		memfree(buf);
		return RV();
	}
	LUNA_CORE_API RP<IFile> open_file(const Path& filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		MutexGuard _guard(m_lock.get());
		Path mount_path;
		Path fs_path;
		auto fs = route_path(filename, mount_path, fs_path);
		if (!fs)
		{
			return BasicError::not_found();
		}
		return fs->open_file(fs_path, flags, creation);
	}

	LUNA_CORE_API R<FileAttribute> file_attribute(const Path& filename)
	{
		MutexGuard _guard(m_lock.get());
		Path mount_path;
		Path fs_path;
		auto fs = route_path(filename, mount_path, fs_path);
		if (!fs)
		{
			return BasicError::not_found();
		}
		return fs->file_attribute(fs_path);
	}

	LUNA_CORE_API RV copy_file(const Path& from_filename, const Path& to_filename, bool fail_if_exists)
	{
		MutexGuard _guard(m_lock.get());
		Path from_path;
		Path to_path;
		Path from_mount_path;
		Path to_mount_path;
		auto from = route_path(from_filename, from_mount_path, from_path);
		auto to = route_path(to_filename, to_mount_path, to_path);
		if (!from || !to)
		{
			return BasicError::not_found();
		}
		if (from == to)
		{
			return from->copy_file(from_path, to_path, fail_if_exists);
		}
		// Force copy.
		return copy_file_between_fs(from, to, from_path, to_path, fail_if_exists);
	}

	LUNA_CORE_API RV move_file(const Path& from_filename, const Path& to_filename, bool allow_copy, bool fail_if_exists)
	{
		MutexGuard _guard(m_lock.get());
		Path from_path;
		Path to_path;
		Path from_mount_path;
		Path to_mount_path;
		auto from = route_path(from_filename, from_mount_path, from_path);
		auto to = route_path(to_filename, to_mount_path, to_path);
		if (!from || !to)
		{
			return BasicError::not_found();
		}
		if (from == to)
		{
			return from->move_file(from_path, to_path, allow_copy, fail_if_exists);
		}
		if (!allow_copy)
		{
			return BasicError::not_supported();
		}
		RV r = copy_file_between_fs(from, to, from_path, to_path, fail_if_exists);
		if (failed(r))
		{
			return r;
		}
		r = from->delete_file(from_path);
		if (failed(r))
		{
			return r;
		}
		return RV();
	}

	LUNA_CORE_API RV delete_file(const Path& filename)
	{
		MutexGuard _guard(m_lock.get());
		Path mount_path;
		Path fs_path;
		auto fs = route_path(filename, mount_path, fs_path);
		if (!fs)
		{
			return BasicError::not_found();
		}
		return fs->delete_file(fs_path);
	}

	LUNA_CORE_API RP<IFileIterator> open_dir(const Path& dir_path)
	{
		MutexGuard _guard(m_lock.get());
		Path mount_path;
		Path fs_path;
		auto fs = route_path(dir_path, mount_path, fs_path);
		if (!fs)
		{
			return BasicError::not_found();
		}
		return fs->open_dir(fs_path);
	}

	LUNA_CORE_API RV create_dir(const Path& pathname)
	{
		MutexGuard _guard(m_lock.get());
		Path mount_path;
		Path fs_path;
		auto fs = route_path(pathname, mount_path, fs_path);
		if (!fs)
		{
			return BasicError::not_found();
		}
		return fs->create_dir(fs_path);
	}

	LUNA_CORE_API RV remove_dir(const Path& pathname, bool recursive)
	{
		MutexGuard _guard(m_lock.get());
		Path mount_path;
		Path fs_path;
		auto fs = route_path(pathname, mount_path, fs_path);
		if (!fs)
		{
			return BasicError::not_found();
		}
		if (recursive)
		{
			auto riter = fs->open_dir(fs_path);
			if (failed(riter))
			{
				return riter.errcode();
			}
			auto& iter = riter.get();
			while (iter->valid())
			{
				if ((!strcmp(iter->filename(), ".")) || (!strcmp(iter->filename(), "..")))
				{
					iter->move_next();
					continue;
				}
				lutry
				{
					auto name = intern_name(iter->filename());
					fs_path.push_back(name);
					if ((iter->attribute() & EFileAttributeFlag::directory) != EFileAttributeFlag::none)
					{
						// is directory.
						luexp(remove_dir(fs_path, true));
					}
					else
					{
						// is file.
						luexp(delete_file(fs_path))
					}
					fs_path.pop_back();
				}
				lucatchret;
				iter->move_next();
			}
			iter = nullptr;
		}
		return fs->remove_dir(fs_path);
	}

	LUNA_CORE_API RV mount_fs(const Path& mount_point, IFileSystem* fs)
	{
		lucheck(fs);
		MutexGuard _guard(m_lock.get());
		MountPair p;
		p.m_fs = fs;
		auto mp = mount_point;
		p.m_path = mp;

		// Check repeat.
		for (auto& i : m_mounts.get())
		{
			if (p.m_path.equal_to(i.m_path))
			{
				return BasicError::already_exists();
			}
		}
		m_mounts.get().push_back(move(p));
		return RV();
	}

	LUNA_CORE_API RV mount_virtual_path(const Path& mount_point, const Path& vfs_path)
	{
		auto path = vfs_path;
		P<VirtualFileSystem> vfs = newobj<VirtualFileSystem>();
		vfs->m_platform_path = path;
		return mount_fs(mount_point, vfs);
	}

	LUNA_CORE_API RV mount_platfrom_path(const Path& mount_point, const Path& platform_path)
	{
		auto path = platform_path;
		P<PlatformFileSystem> pfs = newobj<PlatformFileSystem>();
		pfs->m_platform_path = path;
		return mount_fs(mount_point, pfs);
	}

	LUNA_CORE_API RV unmount_fs(const Path& mount_point)
	{
		MutexGuard _guard(m_lock.get());
		for (auto iter = m_mounts.get().begin(); iter != m_mounts.get().end(); ++iter)
		{
			if (mount_point.equal_to(iter->m_path))
			{
				m_mounts.get().erase(iter);
				return RV();
			}
		}
		return BasicError::not_found();
	}

	LUNA_CORE_API Vector<String> mount_points()
	{
		MutexGuard _guard(m_lock.get());
		Vector<String> strvec;
		strvec.reserve(m_mounts.get().size());
		for (usize i = 0; i < m_mounts.get().size(); ++i)
		{
			auto str = m_mounts.get()[i].m_path.encode();
			strvec.push_back(str);
		}
		return strvec;
	}

	LUNA_CORE_API RP<IFileSystem> get_fs(const Path& mount_point)
	{
		MutexGuard _guard(m_lock.get());

		auto iter = m_mounts.get().rbegin();
		while (iter != m_mounts.get().rend())
		{
			if (mount_point.equal_to(iter->m_path))
			{
				// matched.
				return iter->m_fs;
			}
			else
			{
				++iter;
			}
		}
		// Not found.
		return BasicError::not_found();
	}
}