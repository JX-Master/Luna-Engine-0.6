// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformFileSystem.hpp
* @author JXMaster
* @date 2020/2/11
*/
#pragma once
#include "../CoreHeader.hpp"

namespace luna
{
	class PlatformFileSystem : public IFileSystem
	{
	public:
		lucid("{6d6472bc-c58d-445d-9483-b29705254614}");
		luiimpl(PlatformFileSystem, IFileSystem, IObject);

		P<IPath> m_platform_path;

		PlatformFileSystem(IAllocator* alloc) :
			luibind(alloc) {}

		P<IMutPath> make_native_path(IPath* path)
		{
			P<IMutPath> ret;
			ret = new_path();
			ret->assign(m_platform_path);
			ret->append(path);
			return ret;
		}

		P<IString> make_native_path_str(IPath* path)
		{
			auto ret_path = make_native_path(path);
			auto ret = ret_path->encode();
			return ret;
		}

		virtual RP<IPath> native_path(IPath* filename) override
		{
			auto path = make_native_path(filename);
			return path;
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