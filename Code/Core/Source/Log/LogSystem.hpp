// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file LogSystem.hpp
* @author JXMaster
* @date 2019/12/9
*/
#pragma once
#include "../CoreHeader.hpp"
#include <Base/Vector.hpp>
namespace luna
{
	class Logger;
	class LogHandler;

	namespace log_sys
	{
		extern P<IMutex> g_log_mtx;
		extern Unconstructed<Vector<LogHandler*>> g_log_handlers;

		inline void init(IAllocator* alloc)
		{
			g_log_handlers.construct(alloc);
			g_log_mtx = new_mutex().get();
		}
		inline void deinit()
		{
			g_log_mtx = nullptr;
			g_log_handlers.destruct();
		}

		void accept_log(Logger* logger);
		P<ILogger> new_logger(IName* channel);
		P<ILogHandler> new_log_handler(IName* name, ILogCallback* callback);
		RP<ILogHandler> get_log_handler(IName* name);
	}
}