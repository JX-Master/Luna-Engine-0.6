// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFiber.hpp
* @author JXMaster
* @date 2019/3/31
*/
#pragma once
#include "IObject.hpp"
namespace Luna
{
	enum class EFiberState : u32
	{
		ready = 1,		// The fiber is created and ready to be executed for the first time.
		running = 2,	// The fiber is running on one thread.
		suspended = 3,	// The fiber is suspended by calling `IPlatformThread::yield_current_fiber`.
		waiting = 4,	// The fiber is waiting another fiber resumed by this fiber to suspend or finished.
		finished = 5,	// The fiber is finished to executing and can be released safely.
	};

	//! @interface IFiber
	//! @threadsafe
	//! IFiber interface represent a thread context (Fiber on Windows, ucontext on POSIX) 
	//! that can be run on any given system thread after converting the thread to fiber first.
	struct IFiber : public IObject
	{
		luiid("{7cb8c243-6d53-45a1-97d7-e393b942235e}");
		
		//! Gets the state of this fiber.
		virtual EFiberState state() = 0;
	};
}