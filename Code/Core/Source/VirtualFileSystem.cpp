// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file VirtualFileSystem.cpp
* @author JXMaster
* @date 2020/3/2
*/
#include "VirtualFileSystem.hpp"

namespace Luna
{
	RP<IFile> VirtualFileSystem::open_file(const Path& filename, EFileOpenFlag flags, EFileCreationMode creation)
	{
		auto path = make_native_path(filename);
		return open_file(path, flags, creation);
	}

	R<FileAttribute> VirtualFileSystem::file_attribute(const Path& filename)
	{
		auto path = make_native_path(filename);
		return file_attribute(path);
	}

	RV VirtualFileSystem::copy_file(const Path& from_filename, const Path& to_filename, bool fail_if_exists)
	{
		auto from = make_native_path(from_filename);
		auto to = make_native_path(to_filename);
		return copy_file(from, to, fail_if_exists);
	}

	RV VirtualFileSystem::move_file(const Path& from_filename, const Path& to_filename, bool allow_copy, bool fail_if_exists)
	{
		auto from = make_native_path(from_filename);
		auto to = make_native_path(to_filename);
		return move_file(from, to, allow_copy, fail_if_exists);
	}

	RV VirtualFileSystem::delete_file(const Path& filename)
	{
		auto& path = make_native_path(filename);
		return delete_file(path);
	}

	RP<IFileIterator> VirtualFileSystem::open_dir(const Path& dir_path)
	{
		auto& path = make_native_path(dir_path);
		return open_dir(path);
	}

	RV VirtualFileSystem::create_dir(const Path& pathname)
	{
		auto& path = make_native_path(pathname);
		return create_dir(path);
	}

	RV VirtualFileSystem::remove_dir(const Path& pathname)
	{
		auto& path = make_native_path(pathname);
		return remove_dir(path);
	}
}