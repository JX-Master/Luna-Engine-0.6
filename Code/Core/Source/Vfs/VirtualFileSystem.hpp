// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file VirtualFileSystem.hpp
* @author JXMaster
* @date 2020/3/2
*/
#pragma once
#include "../CoreHeader.hpp"

namespace luna
{
	class VirtualFileSystem : public IFileSystem
	{
	public:
		lucid("{c57960c5-422e-4767-a053-9b56690c532f}");
		luiimpl(VirtualFileSystem, IFileSystem, IObject);

		P<IPath> m_platform_path;

		VirtualFileSystem(IAllocator* alloc) :
			luibind(alloc) {}

		P<IMutPath> make_native_path(IPath* path)
		{
			P<IMutPath> ret = new_path();
			ret->assign(m_platform_path);
			ret->append(path);
			return ret;
		}

		virtual RP<IPath> native_path(IPath* filename) override
		{
			return make_native_path(filename);
		}
		virtual RP<IFile> open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation) override;
		virtual RP<IAsyncFile> open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation) override;
		virtual RV file_attribute(IPath* filename, FileAttribute& attribute) override;
		virtual RV file_exists(IPath* filename) override;
		virtual RV dir_exists(IPath* dir_name) override;
		virtual R<uint64> file_size(IPath* filename) override;
		virtual RV	copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists = false) override;
		virtual RV	move_file(IPath* from_filename, IPath* to_filename, bool allow_copy = true, bool fail_if_exists = false) override;
		virtual RV	delete_file(IPath* filename) override;
		virtual RP<IFileIterator> open_dir(IPath* dir_path) override;
		virtual RV	create_dir(IPath* pathname) override;
		virtual RV	remove_dir(IPath* pathname) override;
	};
}