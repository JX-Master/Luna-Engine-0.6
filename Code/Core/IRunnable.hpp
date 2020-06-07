// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IRunnable.hpp
* @author JXMaster
* @date 2018/12/24
 */
#pragma once
#include <Base/IObject.hpp>

namespace luna
{
	//! The callback that will be called by a thread or a fiber when it gets
	//! run.
	struct IRunnable : public IObject
	{
		luiid("{d96813d5-8ebb-46a3-ad55-1d116a91f80d}");

		//! Called when the thread or fiber gets run.
		//! @return Returns 0 if the thread successfully exits, returns nun-zero 
		//! otherwise. 
		//! The return value is passed to operation system if needed. The engine
		//! does not use this value.
		virtual int run() = 0;
	};
}