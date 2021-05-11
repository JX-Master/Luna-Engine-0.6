// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformFileSystem.hpp
* @author JXMaster
* @date 2020/2/11
*/
#pragma once
#include "../IFileSystem.hpp"
#include "../Interface.hpp"

namespace Luna
{
	class PlatformFileSystem : public IFileSystem
	{
	public:
		lucid("{6d6472bc-c58d-445d-9483-b29705254614}");
		luiimpl(PlatformFileSystem, IFileSystem, IObject);

		Path m_platform_path;

		PlatformFileSystem() {}

		Path make_native_path(const Path& path)
		{
			Path ret = m_platform_path;
			ret.append(path);
			return ret;
		}

		String make_native_path_str(const Path& path)
		{
			auto ret_path = make_native_path(path);
			auto ret = ret_path.encode();
			return ret;
		}

		virtual R<Path> native_path(const Path& filename) override
		{
			auto path = make_native_path(filename);
			return path;
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