// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IAsyncFileRequest.hpp
* @author JXMaster
* @date 2019/9/29
*/
#pragma once
#include "IWaitable.hpp"

namespace luna
{
	//! @interface IAsyncFileRequest
	//! @threadsafe
	//! Represents a asynchronous file I/O request.
	struct IAsyncFileRequest : public IWaitable
	{
		luiid("{0e052d77-a2ff-44e2-a1b8-da1855c24408}");

		virtual R<uint32> transferred_size() = 0;
	};
}