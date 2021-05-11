// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformFileSystem.cpp
* @author JXMaster
* @date 2020/2/11
*/
#include "PlatformFileSystem.hpp"
#include "../Core.hpp"

namespace Luna
{
	RP<IFile> PlatformFileSystem::open_file(const Path& filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		auto path = make_native_path_str(filename);
		return platform_open_file(path.c_str(), flags, creation);
	}

	R<FileAttribute> PlatformFileSystem::file_attribute(const Path& filename)
	{
		auto path = make_native_path_str(filename);
		return platform_file_attribute(path.c_str());
	}

	RV PlatformFileSystem::copy_file(const Path& from_filename, const Path& to_filename, bool fail_if_exists)
	{
		auto from = make_native_path_str(from_filename);
		auto to = make_native_path_str(to_filename);
		return platform_copy_file(from.c_str(), to.c_str(), fail_if_exists);
	}

	RV PlatformFileSystem::move_file(const Path& from_filename, const Path& to_filename, bool allow_copy, bool fail_if_exists)
	{
		auto from = make_native_path_str(from_filename);
		auto to = make_native_path_str(to_filename);
		return platform_move_file(from.c_str(), to.c_str(), allow_copy, fail_if_exists);
	}

	RV PlatformFileSystem::delete_file(const Path& filename)
	{
		auto path = make_native_path_str(filename);
		return platform_delete_file(path.c_str());
	}

	RP<IFileIterator> PlatformFileSystem::open_dir(const Path& dir_path)
	{
		auto path = make_native_path_str(dir_path);
		return platform_open_dir(path.c_str());
	}

	RV PlatformFileSystem::create_dir(const Path& pathname)
	{
		auto path = make_native_path_str(pathname);
		return platform_create_dir(path.c_str());
	}

	RV PlatformFileSystem::remove_dir(const Path& pathname)
	{
		auto path = make_native_path_str(pathname);
		return platform_remove_dir(path.c_str());
	}
}