// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ISemaphore.hpp
* @author JXMaster
* @date 2019/3/14
* @brief ISemaphore interface represents a semaphore object.
*/
#pragma once
#include "IWaitable.hpp"
namespace luna
{
	struct ISemaphore : public IWaitable
	{
		luiid("{ef6a7782-0a6c-4a40-abc9-a6d2381a3397}");

		virtual void unacquire(int32 release_count = 1) = 0;
	};
}