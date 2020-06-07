// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Vfs.hpp
* @author JXMaster
* @date 2020/1/23
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/Vector.hpp>
#include "PlatformFileSystem.hpp"
#include "VirtualFileSystem.hpp"
namespace luna
{
	namespace vfs
	{
		// Mount path.
		struct MountPair
		{
			P<IPath> m_path;
			P<IFileSystem> m_fs;
		};

		extern P<IReadWriteLock> m_lock;
		extern Unconstructed<Vector<MountPair>> m_mounts;

		inline void init(IAllocator* alloc)
		{
			m_lock = new_read_write_lock();
			m_mounts.construct(alloc);
		}

		inline void deinit()
		{
			m_mounts.destruct();
			m_lock = nullptr;
		}

		void init();

		P<IFileSystem> route_path(IPath* filename, P<IPath>& mount_point, P<IMutPath>& fs_path);
		RV copy_file_between_fs(IFileSystem* from, IFileSystem* to, IPath* from_path, IPath* to_path, bool fail_if_exists);

		RP<IFile> open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation);
		RP<IAsyncFile> open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation);
		RV file_attribute(IPath* filename, FileAttribute& attribute);
		RV file_exists(IPath* filename);
		RV dir_exists(IPath* dir_name);
		R<uint64> file_size(IPath* filename);
		RV copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists = false);
		RV move_file(IPath* from_filename, IPath* to_filename, bool allow_copy = true, bool fail_if_exists = false);
		RV delete_file(IPath* filename);
		RP<IFileIterator> open_dir(IPath* dir_path);
		RV create_dir(IPath* pathname);
		RV remove_dir(IPath* pathname, bool recursive = false);
		RV mount_fs(IPath* mount_point, IFileSystem* fs);
		RV mount_virtual_path(IPath* mount_point, IPath* vfs_path);
		RV mount_platfrom_path(IPath* mount_point, IPath* platform_path);
		RV unmount_fs(IPath* mount_point);
		Vector<P<IString>> mount_points();
		RP<IFileSystem> get_fs(IPath* mount_point);
	}
}