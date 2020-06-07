// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file LogSystem.cpp
* @author JXMaster
* @date 2019/12/9
*/
#include "LogSystem.hpp"
#include "Logger.hpp"
#include "LogHandler.hpp"
#include "Log.hpp"
namespace luna
{
	namespace log_sys
	{
		P<IMutex> g_log_mtx;
		Unconstructed<Vector<LogHandler*>> g_log_handlers;
		void accept_log(Logger* logger)
		{
			MutexGuard g(g_log_mtx);
			P<Log> log = box_ptr<Log>(new_obj<Log>(get_global_heap()));
			log->m_timestamp = get_local_timestamp();
			auto msg = new_string(logger->m_buffer, logger->m_buf_size);
			log->m_message = msg;
			log->m_type = logger->m_type;
			log->m_channel = logger->m_channel;
			log->m_flags = ELogFlag::none;
			if ((logger->m_block))
			{
				log->m_flags |= ELogFlag::is_block;
			}
			auto& handlers = g_log_handlers.get();
			for (auto i : handlers)
			{
				ReadGuard mtx(i->m_mtx.get());
				if (!i->m_enabled || (logger->m_type & i->m_pass_types) == ELogType::none)
				{
					continue;
				}
				bool matched = false;
				if (i->m_listen_all)
				{
					matched = true;
				}
				else
				{
					for (auto& hid : i->m_channels)
					{
						if (logger->m_channel == hid)
						{
							matched = true;
							break;
						}
					}
				}
				if (matched)
				{
					// send message to this channel.
					P<ILogCallback> handler_callback = i->m_callback.lock();
					if (handler_callback)
					{
						handler_callback->on_log(log, logger);
					}
				}
			}
		}
		P<ILogger> new_logger(IName* channel)
		{
			luassert_usr(channel);
			Logger* logger = get_global_heap()->new_obj<Logger>(get_global_heap());
			P<ILogger> pl;
			pl.attach(logger);
			logger->m_channel = channel;
			return pl;
		}

		P<ILogHandler> new_log_handler(IName* name, ILogCallback* callback)
		{
			luassert_usr(name && callback);
			LogHandler* handler = get_global_heap()->new_obj<LogHandler>(get_global_heap());
			handler->m_mtx = new_read_write_lock().get();
			handler->m_callback = callback;
			handler->m_name = name;
			handler->m_enabled = true;
			handler->m_listen_all = false;
			handler->m_pass_types = ELogType::all;
			g_log_mtx->wait();
			g_log_handlers.get().push_back(handler);
			g_log_mtx->unlock();
			return box_ptr<ILogHandler>(handler);
		}

		RP<ILogHandler> get_log_handler(IName* name)
		{
			luassert_usr(name);
			MutexGuard mtx(g_log_mtx);
			for (auto& i : g_log_handlers.get())
			{
				if (i->m_name.get() == name)
				{
					return P<ILogHandler>(i);
				}
			}
			return e_item_not_exist;
		}
	}
}