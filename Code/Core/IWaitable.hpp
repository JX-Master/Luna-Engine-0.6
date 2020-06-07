// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IWaitable.hpp
* @author JXMaster
* @date 2018/12/8
*/
#pragma once
#include <Base/IObject.hpp>

namespace luna
{
	//! @interface IWaitable
	//! Represents a waitable object used for multi-thread synchronization. The object that implements
	//! `IWaitable` cannot be used cross process.
	struct IWaitable : public IObject
	{
		luiid("{3dcaabdc-f4d5-4aa6-bc30-904f7875964a}");

		//! Wait for this object to be signaled. This will suspend current thread until the wait
		//! condition is satisfied.
		virtual void wait() = 0;

		//! Try to wait for this object to be signaled. This will not suspend the current thread, if 
		//! the condition is not satisfied, this call returns immediately with an error code `e_pending`.
		virtual RV try_wait() = 0;
	};
}