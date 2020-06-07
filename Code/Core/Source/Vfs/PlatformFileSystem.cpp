// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformFileSystem.cpp
* @author JXMaster
* @date 2020/2/11
*/
#include "PlatformFileSystem.hpp"

namespace luna
{
	RP<IFile> PlatformFileSystem::open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		auto path = make_native_path_str(filename);
		return platform_open_file(path->c_str(), flags, creation);
	}

	RP<IAsyncFile> PlatformFileSystem::open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		auto path = make_native_path_str(filename);
		return platform_open_file_async(path->c_str(), flags, creation);
	}

	RV PlatformFileSystem::file_attribute(IPath* filename, FileAttribute& attribute)
	{
		auto path = make_native_path_str(filename);
		return platform_file_attribute(path->c_str(), attribute);
	}

	RV PlatformFileSystem::file_exists(IPath* filename)
	{
		auto path = make_native_path_str(filename);
		return platform_file_exists(path->c_str());
	}

	RV PlatformFileSystem::dir_exists(IPath* dir_name)
	{
		auto path = make_native_path_str(dir_name);
		return platform_dir_exists(path->c_str());
	}

	R<uint64> PlatformFileSystem::file_size(IPath* filename)
	{
		auto path = make_native_path_str(filename);
		return platform_file_size(path->c_str());
	}

	RV PlatformFileSystem::copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists)
	{
		auto from = make_native_path_str(from_filename);
		auto to = make_native_path_str(to_filename);
		return platform_copy_file(from->c_str(), to->c_str(), fail_if_exists);
	}

	RV PlatformFileSystem::move_file(IPath* from_filename, IPath* to_filename, bool allow_copy, bool fail_if_exists)
	{
		auto from = make_native_path_str(from_filename);
		auto to = make_native_path_str(to_filename);
		return platform_move_file(from->c_str(), to->c_str(), allow_copy, fail_if_exists);
	}

	RV PlatformFileSystem::delete_file(IPath* filename)
	{
		auto path = make_native_path_str(filename);
		return platform_delete_file(path->c_str());
	}

	RP<IFileIterator> PlatformFileSystem::open_dir(IPath* dir_path)
	{
		auto path = make_native_path_str(dir_path);
		return platform_open_dir(path->c_str());
	}

	RV PlatformFileSystem::create_dir(IPath* pathname)
	{
		auto path = make_native_path_str(pathname);
		return platform_create_dir(path->c_str());
	}

	RV PlatformFileSystem::remove_dir(IPath* pathname)
	{
		auto path = make_native_path_str(pathname);
		return platform_remove_dir(path->c_str());
	}
}