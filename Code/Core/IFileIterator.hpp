// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFileIterator.hpp
* @author JXMaster
* @date 2018/12/24
 */
#pragma once
#include "IFile.hpp"
namespace  Luna
{
	//! @interface IFileIterator
	//! Represents a directory stream that can be used to iterate all files and directories
	//! in the specified directory. See `open_dir` for details.
    struct IFileIterator : public IObject
    {
		luiid("{97643c3c-4681-4b24-9aee-320f88379edc}");

		//! Checks if this iterator points to a valid file item in the directory stream.
		virtual bool valid() = 0;

		//! Returns the name of the file the iterator currently points to.
		//! Returns `nullptr` if the file iterator is invalid.
		virtual const c8* filename() = 0;

		//! Returns the file attribute of the file the iterator currently points to.
		//! Returns EFileAttributeFlag::none if the file iterator is invalid.
		virtual EFileAttributeFlag attribute() = 0;

		//! Moves to the next file in the directory.
		//! @return Returns `true` if the file iterator is valid after this call, 
		//! returns `false` otherwise.
		//! If the return value is `false`, then the iterator has reached the end of the 
		//! directory stream.
		virtual bool move_next() = 0;
    };
}