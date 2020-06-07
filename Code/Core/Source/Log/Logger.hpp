// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Logger.hpp
* @author JXMaster
* @date 2019/4/9
*/
#pragma once
#include "../CoreHeader.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
#include "LogSystem.hpp"
namespace luna
{
	constexpr size_t g_log_buffer_size = 1024;
	class Logger : public ILogger
	{
	public:
		lucid("{b42e2dcd-6b81-4a63-8408-5cda8c6a8514}");
		luiimpl(Logger, ILogger, IObject);
		

		P<IMutex> m_mtx;	// the mutex to access this logger.

		// logging context.
		char m_buffer[g_log_buffer_size];
		size_t m_buf_size;
		ELogType m_type;
		ELogType m_pass_types;
		bool m_logging;	// if it is currently logging.
		bool m_block;

		P<IName> m_channel;

		Logger(IAllocator* alloc);

		virtual RV set_channel(IName* channel) override
		{
			if (!channel)
			{
				return e_bad_arguments;
			}
			m_channel = channel;
			return s_ok;
		}
		virtual P<IName> channel() override
		{
			return m_channel;
		}
		virtual void set_pass_log_types(ELogType types) override
		{
			m_pass_types = types;
		}
		virtual ELogType pass_log_types() override
		{
			return m_pass_types;
		}

		virtual RV log(ELogType log_type, const char* msg) override;
		virtual RV vlogf(ELogType log_type, const char* fmt, va_list args) override;
		virtual RV logf(ELogType log_type, const char* fmt, ...) override;

		virtual RV begin_log(ELogType log_type, bool as_block = false) override;
		virtual RV append_msg(const char* msg) override;
		virtual RV append_vmsgf(const char* fmt, va_list args) override;
		virtual RV append_msgf(const char* fmt, ...) override;
		virtual RV end_log() override;
	};
}
#endif