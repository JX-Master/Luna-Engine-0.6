// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file VirtualFileSystem.hpp
* @author JXMaster
* @date 2020/3/2
*/
#pragma once
#include "../IFileSystem.hpp"
#include "../Interface.hpp"

namespace Luna
{
	class VirtualFileSystem : public IFileSystem
	{
	public:
		lucid("{c57960c5-422e-4767-a053-9b56690c532f}");
		luiimpl(VirtualFileSystem, IFileSystem, IObject);

		Path m_platform_path;

		VirtualFileSystem() {}

		Path make_native_path(const Path& path)
		{
			Path ret = m_platform_path;
			ret.append(path);
			return ret;
		}

		virtual R<Path> native_path(const Path& filename) override
		{
			return make_native_path(filename);
		}
		virtual RP<IFile> open_file(const Path& filename, EFileOpenFlag flags, EFileCreationMode creation) override;
		virtual R<FileAttribute> file_attribute(const Path& filename) override;
		virtual RV	copy_file(const Path& from_filename, const Path& to_filename, bool fail_if_exists = false) override;
		virtual RV	move_file(const Path& from_filename, const Path& to_filename, bool allow_copy = true, bool fail_if_exists = false) override;
		virtual RV	delete_file(const Path& filename) override;
		virtual RP<IFileIterator> open_dir(const Path& dir_path) override;
		virtual RV	create_dir(const Path& pathname) override;
		virtual RV	remove_dir(const Path& pathname) override;
	};
}