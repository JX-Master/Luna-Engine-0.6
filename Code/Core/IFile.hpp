// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFile.hpp
* @author JXMaster
* @date 2019/3/14
*/
#pragma once
#include "IStream.hpp"
namespace luna
{
	enum class EFileAttributeFlag : uint32
	{
		none = 0x00,
		read_only = 0x01,
		hidden = 0x02,
		directory = 0x04,			// This file is a directory.
		character_special = 0x08,	// This file is an character special file on UNIX/Linux systems.
	};

	enum class EFileOpenFlag : uint32
	{
		none = 0x00,
		//! Grants read access to the file so that `read` operations can be performed.
		read = 0x01,
		//! Grants write access to the file so that `write` operations can be performed.
		write = 0x02,
		//! Opens the file using user-mode buffering. The user-mode buffering allocates a user-mode buffer to 
		//! buffer the data read from file or to be written to file, so reduces system calls if lots of 
		//! small size reads/writes need to be performed.
		//! 
		//! When user-mode buffer is used, `flush` or `seeks` should be called after a write operation and before
		//! a read operation, and `seek` should be called after a read operation and before a write operation.
		//! 
		//! `IAsyncFile` does not support user-mode buffering, since its I/O model is request-based, not stream-based.
		user_buffering = 0x04,
	};

	enum class EFileCreationMode : uint32
	{
		//! Always creates a new file and opens it. If the file already exists, the old file content
		//! will be discarded and the file is treated as a new empty file.
		create_always = 1,
		//! Only creates a file and opens it when it does not exist. If the file already exists, the
		//! call fails with `e_file_already_exists`.
		create_new = 2,
		//! Always opens a file. If the file already exists, the file will be open with its data preserved,
		//! if the file does not exist, it will be created and opened.
		open_always = 3,
		//! Only opens a file when it does exists. If the file does not exist, the call fails with `e_file_not_exist`.
		open_existing = 4,
		//! Only opens a file when it does exists, and discards the file data so the file is treated as a new
		//! file.
		open_existing_as_new = 5
	};

	struct FileAttribute
	{
		uint64 size;
		uint64 creation_time;
		uint64 last_access_time;
		uint64 last_write_time;
		EFileAttributeFlag attributes;
	};

	//! @interface IFile
	//! `IFile` interface represents a opened file object of the platform or the virtual file system.
	struct IFile : public IStream
	{
		luiid("{c61fbf68-89dc-4776-8a99-cc855ff8289e}");

		
	};
}