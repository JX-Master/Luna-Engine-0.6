// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFileSystem.hpp
* @author JXMaster
* @date 2019/1/23
*/
#pragma once
#include "IObject.hpp"
#include "IFile.hpp"
#include "IFileIterator.hpp"
#include <Runtime/Result.hpp>
#include <Runtime/Path.hpp>
namespace Luna
{
	//! @interface IFileSystem
	//! Represents a file system that can be mounted to the virtual file system.
	struct IFileSystem : public IObject
	{
		luiid("{41bf17df-80c1-4eee-b5ab-22f0446aa750}");

		//! Returns the native path for the given filename on this file system.
		//! Returns failure if the given file does not have a native path.
		virtual R<Path> native_path(const Path& filename) = 0;

		//! Opens one file.
		//! @param[in] filename The path of the file relative to the root path of the file system.
		//! @param[in] flags The file open flags.
		//! @param[in] creation Specify whether to create a file if the file does not exist.
		//! @return If succeeded, returns the new opened file object.
		virtual RP<IFile> open_file(const Path& filename, EFileOpenFlag flags, EFileCreationMode creation) = 0;

		//! Gets the file attribute.
		virtual R<FileAttribute> file_attribute(const Path& filename) = 0;

		//! Copies the file from the source path to the destination path.
		//! @param[in] from_filename Source file path.
		//! @param[in] to_filename Destination file path.
		//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
		//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
		//! already exists, the file named `to_filename` will be overwritten.
		virtual RV	copy_file(const Path& from_filename, const Path& to_filename, bool fail_if_exists = false) = 0;
		
		//! Moves the file from the source path to the destination path. This call can also be used to rename a file.
		//! @param[in] from_filename Source file path.
		//! @param[in] to_filename Destination file path.
		//! @param[in] allow_copy If the `from_filename` cannot be moved `to_filename` directly, set this to `true` allows 
		//! the system to copy the file from `from_filename` to `to_filename` and delete file in `from_filename`. If this 
		//! is `false` and the file cannot be moved, this call failed.
		//! @param[in] fail_if_exists If this is `true` and the file named `to_filename` already exists,
		//! this call fails with return value `e_item_already_exists`. If this is `false` and the file named `to_filename` 
		//! already exists, the file named `to_filename` will be overwritten.
		virtual RV	move_file(const Path& from_filename, const Path& to_filename, bool allow_copy = true, bool fail_if_exists = false) = 0;
		
		//! Deletes the specified file.
		//! @param[in] filename The file to delete.
		virtual RV	delete_file(const Path& filename) = 0;

		//! Creates a file iterator that can be used to iterate all files in the specified directory.
		//! @param[in] dir_path The directory path to open.
		//! @return Returns a file iterator object if succeeded.
		virtual RP<IFileIterator> open_dir(const Path& dir_path) = 0;

		//! Creates one directory.
		//! @param[in] pathname The path of the directory.
		virtual RV	create_dir(const Path& pathname) = 0;

		//! Removes one directory.
		//! @param[in] pathname The path of the directory.
		virtual RV	remove_dir(const Path& pathname) = 0;
	};
}