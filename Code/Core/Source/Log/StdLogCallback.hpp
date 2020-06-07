// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StdLogCallback.hpp
* @author JXMaster
* @date 2020/1/10
*/
#pragma once
#include "../CoreHeader.hpp"
namespace luna
{
	class StdLogCallback : public ILogCallback
	{
	public:
		lucid("{0a06f822-abc6-4716-93c0-86f37560161f}");
		luiimpl_static(StdLogCallback, ILogCallback, IObject);
		
		virtual void on_log(ILog* log, ILogger* sender) override;
	};

	extern Unconstructed<StdLogCallback> g_std_log_callback;
}