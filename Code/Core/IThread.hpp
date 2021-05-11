// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IThread.hpp
* @author JXMaster
* @date 2019/3/14
* @brief IThread interface represents a system thread context.
*/
#pragma once
#include "IWaitable.hpp"
namespace Luna
{
	enum class EThreadPriority : i32
	{
		low = 0,
		normal = 1,
		high = 2,
		critical = 3
	};
	struct IFiber;
	//! @interface IThread
	//! @threadsafe
	//! Represents a system thread object.
	struct IThread : public IWaitable
	{
		luiid("{9e4fbbb8-0751-4703-bfb5-246dd1cf8b04}");

		//! Set thread priority.
		virtual RV set_priority(EThreadPriority priority) = 0;
	};
}