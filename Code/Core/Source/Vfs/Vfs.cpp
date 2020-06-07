// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Vfs.cpp
* @author JXMaster
* @date 2020/1/23
*/
#include "Vfs.hpp"

namespace luna
{
	namespace vfs
	{
		P<IReadWriteLock> m_lock;
		Unconstructed<Vector<MountPair>> m_mounts;
		P<IFileSystem> route_path(IPath* filename, P<IPath>& mount_point, P<IMutPath>& fs_path)
		{
			auto iter = m_mounts.get().rbegin();
			while (iter != m_mounts.get().rend())
			{
				if (filename->is_subpath_of(iter->m_path))
				{
					// matched.
					auto fs = iter->m_fs;
					lutry
					{
						mount_point = iter->m_path;
						fs_path = new_path();
						luexp(fs_path->assign_relative(mount_point, filename));
						return fs;
					}
					lucatch
					{
						return nullptr;
					}
				}
				else
				{
					++iter;
				}
			}
			// Not found.
			return nullptr;
		}
		RV copy_file_between_fs(IFileSystem* from, IFileSystem* to, IPath* from_path, IPath* to_path, bool fail_if_exists)
		{
			// try open file.
			P<IFile> from_file;
			P<IFile> to_file;
			uint64 file_sz;
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
				luset(file_sz, from_file->size());
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
				buf = get_global_heap()->allocate((size_t)16_mb);
			}
			else
			{
				buf = get_global_heap()->allocate((size_t)file_sz);
			}
			if (!buf)
			{
				from_file = nullptr;
				to_file = nullptr;
				auto _ = to->delete_file(to_path);
				return e_bad_memory_alloc;
			}

			// copying.
			lutry2
			{
				if (file_sz >= 16_mb)
				{
					for (uint64 i = 0; i < file_sz; i += 16_mb)
					{
						size_t bytes_to_read = (size_t)min(16_mb, file_sz - i);
						size_t bytes_read;
						luexp2(from_file->read(buf, bytes_to_read, &bytes_read));
						luassert(bytes_to_read == bytes_read);
						luexp2(to_file->write(buf, bytes_to_read, &bytes_read));
						luassert(bytes_to_read == bytes_read);
					}
				}
				else
				{
					size_t bytes_to_read = (size_t)file_sz;
					size_t bytes_read;
					luexp2(from_file->read(buf, bytes_to_read, &bytes_read));
					luassert(bytes_to_read == bytes_read);
					luexp2(to_file->write(buf, bytes_to_read, &bytes_read));
					luassert(bytes_to_read == bytes_read);
				}
			}
				lucatch2
			{
				// read or write failed.
				get_global_heap()->free(buf);
				from_file = nullptr;
				to_file = nullptr;
				auto _ = to->delete_file(to_path);
				return lures2;
			}
			get_global_heap()->free(buf);
			return s_ok;
		}
		RP<IFile> open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
		{
			luassert_usr(filename);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(filename, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->open_file(fs_path, flags, creation);
		}

		RP<IAsyncFile> open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
		{
			luassert_usr(filename);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(filename, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->open_file_async(fs_path, flags, creation);
		}

		RV file_attribute(IPath* filename, FileAttribute& attribute)
		{
			luassert_usr(filename);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(filename, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->file_attribute(fs_path, attribute);
		}

		RV file_exists(IPath* filename)
		{
			luassert_usr(filename);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(filename, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->file_exists(fs_path);
		}

		RV dir_exists(IPath* dir_name)
		{
			luassert_usr(dir_name);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(dir_name, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->dir_exists(fs_path);
		}

		R<uint64> file_size(IPath* filename)
		{
			luassert_usr(filename);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(filename, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->file_size(fs_path);
		}
		RV copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists)
		{
			luassert_usr(from_filename && to_filename);
			ReadGuard _guard(m_lock.get());
			P<IMutPath> from_path;
			P<IMutPath> to_path;
			P<IPath> from_mount_path;
			P<IPath> to_mount_path;
			auto from = route_path(from_filename, from_mount_path, from_path);
			auto to = route_path(to_filename, to_mount_path, to_path);
			if (!from || !to)
			{
				return e_file_not_exist;
			}
			if (from == to)
			{
				return from->copy_file(from_path, to_path, fail_if_exists);
			}
			// Force copy.
			return copy_file_between_fs(from, to, from_path, to_path, fail_if_exists);
		}

		RV move_file(IPath* from_filename, IPath* to_filename, bool allow_copy, bool fail_if_exists)
		{
			luassert_usr(from_filename && to_filename);
			ReadGuard _guard(m_lock.get());
			P<IMutPath> from_path;
			P<IMutPath> to_path;
			P<IPath> from_mount_path;
			P<IPath> to_mount_path;
			auto from = route_path(from_filename, from_mount_path, from_path);
			auto to = route_path(to_filename, to_mount_path, to_path);
			if (!from || !to)
			{
				return e_file_not_exist;
			}
			if (from == to)
			{
				return from->move_file(from_path, to_path, allow_copy, fail_if_exists);
			}
			if (!allow_copy)
			{
				return e_not_available;
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
			return s_ok;
		}

		RV delete_file(IPath* filename)
		{
			luassert_usr(filename);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(filename, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->delete_file(fs_path);
		}

		RP<IFileIterator> open_dir(IPath* dir_path)
		{
			luassert_usr(dir_path);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(dir_path, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->open_dir(fs_path);
		}

		RV create_dir(IPath* pathname)
		{
			luassert_usr(pathname);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(pathname, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			return fs->create_dir(fs_path);
		}

		RV remove_dir(IPath* pathname, bool recursive)
		{
			luassert_usr(pathname);
			ReadGuard _guard(m_lock.get());
			P<IPath> mount_path;
			P<IMutPath> fs_path;
			auto fs = route_path(pathname, mount_path, fs_path);
			if (!fs)
			{
				return e_file_not_exist;
			}
			if (recursive)
			{
				auto riter = fs->open_dir(fs_path);
				if (failed(riter))
				{
					return riter.result();
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
						fs_path->push_back(name);
						if ((iter->attribute() & EFileAttributeFlag::directory) != EFileAttributeFlag::none)
						{
							// is directory.
							luexp(vfs::remove_dir(fs_path, true));
						}
						else
						{
							// is file.
							luexp(vfs::delete_file(fs_path))
						}
						fs_path->pop_back();
					}
					lucatchret;
					iter->move_next();
				}
				iter = nullptr;
			}
			return fs->remove_dir(fs_path);
		}

		RV mount_fs(IPath* mount_point, IFileSystem* fs)
		{
			luassert_usr(mount_point && fs);
			WriteGuard(m_lock.get());
			MountPair p;
			p.m_fs = fs;
			auto mp = new_path();
			mp->assign(mount_point);
			p.m_path = mp;

			// Check repeat.
			for (auto& i : m_mounts.get())
			{
				if (p.m_path->equal_to(i.m_path))
				{
					return e_item_already_exists;
				}
			}
			m_mounts.get().push_back(move(p));
			return s_ok;
		}

		RV mount_virtual_path(IPath* mount_point, IPath* vfs_path)
		{
			luassert_usr(mount_point && vfs_path);
			P<VirtualFileSystem> vfs;
			auto path = new_path();
			path->assign(vfs_path);
			VirtualFileSystem* fs = new_obj<VirtualFileSystem>(get_global_heap());
			vfs = box_ptr(fs);
			vfs->m_platform_path = path;

			auto r = vfs::mount_fs(mount_point, vfs);
			return r;
		}

		RV mount_platfrom_path(IPath* mount_point, IPath* platform_path)
		{
			luassert_usr(mount_point && platform_path);
			P<PlatformFileSystem> pfs;

			auto path = new_path();
			path->assign(platform_path);
			PlatformFileSystem* fs = new_obj<PlatformFileSystem>(get_global_heap());
			pfs = box_ptr(fs);
			pfs->m_platform_path = path;

			auto r = vfs::mount_fs(mount_point, pfs);
			return r;
		}

		RV unmount_fs(IPath* mount_point)
		{
			luassert_usr(mount_point);
			WriteGuard(m_lock.get());
			for (auto iter = m_mounts.get().begin(); iter != m_mounts.get().end(); ++iter)
			{
				if (mount_point->equal_to(iter->m_path))
				{
					m_mounts.get().erase(iter);
					return s_ok;
				}
			}
			return e_item_not_exist;
		}

		Vector<P<IString>> mount_points()
		{
			ReadGuard _guard(m_lock.get());
			Vector<P<IString>> strvec(get_global_heap());
			strvec.reserve(m_mounts.get().size());
			for (size_t i = 0; i < m_mounts.get().size(); ++i)
			{
				auto str = m_mounts.get()[i].m_path->encode();
				strvec.push_back(str);
			}
			return strvec;
		}

		RP<IFileSystem> get_fs(IPath* mount_point)
		{
			luassert_usr(mount_point);
			WriteGuard(m_lock.get());

			auto iter = m_mounts.get().rbegin();
			while (iter != m_mounts.get().rend())
			{
				if (mount_point->equal_to(iter->m_path))
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
			return e_item_not_exist;
		}
	}
}