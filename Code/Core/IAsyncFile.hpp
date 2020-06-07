// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAsyncFile.hpp
* @author JXMaster
* @date 2019/9/29
*/
#pragma once
#include <Base/IObject.hpp>
#include "IAsyncFileRequest.hpp"
#include "IString.hpp"

namespace luna
{
	//! @interface IAsyncFile
	//! @threadsafe
	//! Represents a file object implemented in system's asynchronous I/O model.
	struct IAsyncFile : public IObject
	{
		luiid("{ca5b5c06-0a45-4e71-a5aa-caef034323db}");

		virtual RP<IAsyncFileRequest> read(void* buffer, uint32 size, uint64 file_offset) = 0;
		virtual RP<IAsyncFileRequest> write(const void* buffer, uint32 size, uint64 file_offset) = 0;
		virtual R<uint64> size() = 0;
	};
}