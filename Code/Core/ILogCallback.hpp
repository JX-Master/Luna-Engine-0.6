// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ILogCallback.hpp
* @author JXMaster
* @date 2018/12/28
*/
#pragma once
#include "ILog.hpp"

namespace luna
{
	struct ILogCallback : public IObject
	{
		luiid("{a5a3da6b-feda-4773-8d70-b3b80a302620}");

		//! Called when the log message is sent to the bounding log handler.
		virtual void on_log(ILog* log, ILogger* sender) = 0;
	};
}