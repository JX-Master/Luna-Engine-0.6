// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file LogHandler.cpp
* @author JXMaster
* @date 2019/12/23
*/
#include "LogHandler.hpp"
#include "LogSystem.hpp"

namespace luna
{
	LogHandler::~LogHandler()
	{
		MutexGuard mtx(log_sys::g_log_mtx.get());
		for (auto iter = log_sys::g_log_handlers.get().begin(); iter != log_sys::g_log_handlers.get().end(); ++iter)
		{
			if (*iter == this)
			{
				log_sys::g_log_handlers.get().erase(iter);
				break;
			}
		}
	}

	R<Vector<P<IName>>> LogHandler::channels()
	{
		lutsassert();
		return m_channels;
	}

	void LogHandler::set_channels(const Vector<P<IName>>& channels)
	{
		lutsassert();
		m_channels = channels;
	}
}