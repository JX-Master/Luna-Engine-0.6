// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file VirtualFileSystem.cpp
* @author JXMaster
* @date 2020/3/2
*/
#include "VirtualFileSystem.hpp"

namespace luna
{
	RP<IFile> VirtualFileSystem::open_file(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		auto path = make_native_path(filename);
		return open_file(path, flags, creation);
	}

	RP<IAsyncFile> VirtualFileSystem::open_file_async(IPath* filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		auto path = make_native_path(filename);
		return open_file_async(path, flags, creation);
	}

	RV VirtualFileSystem::file_attribute(IPath* filename, FileAttribute& attribute)
	{
		auto path = make_native_path(filename);
		return file_attribute(path, attribute);
	}

	RV VirtualFileSystem::file_exists(IPath* filename)
	{
		auto path = make_native_path(filename);
		return file_exists(path);
	}

	RV VirtualFileSystem::dir_exists(IPath* dir_name)
	{
		auto path = make_native_path(dir_name);
		return dir_exists(path);
	}

	R<uint64> VirtualFileSystem::file_size(IPath* filename)
	{
		auto path = make_native_path(filename);
		return file_size(path);
	}

	RV VirtualFileSystem::copy_file(IPath* from_filename, IPath* to_filename, bool fail_if_exists)
	{
		auto from = make_native_path(from_filename);
		auto to = make_native_path(to_filename);
		return copy_file(from, to, fail_if_exists);
	}

	RV VirtualFileSystem::move_file(IPath* from_filename, IPath* to_filename, bool allow_copy, bool fail_if_exists)
	{
		auto from = make_native_path(from_filename);
		auto to = make_native_path(to_filename);
		return move_file(from, to, allow_copy, fail_if_exists);
	}

	RV VirtualFileSystem::delete_file(IPath* filename)
	{
		auto& path = make_native_path(filename);
		return delete_file(path);
	}

	RP<IFileIterator> VirtualFileSystem::open_dir(IPath* dir_path)
	{
		auto& path = make_native_path(dir_path);
		return open_dir(path);
	}

	RV VirtualFileSystem::create_dir(IPath* pathname)
	{
		auto& path = make_native_path(pathname);
		return create_dir(path);
	}

	RV VirtualFileSystem::remove_dir(IPath* pathname)
	{
		auto& path = make_native_path(pathname);
		return remove_dir(path);
	}
}