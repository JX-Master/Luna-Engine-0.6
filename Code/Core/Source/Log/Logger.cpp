// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Logger.cpp
* @author JXMaster
* @date 2019/4/9
*/
#pragma once
#include "Logger.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	Logger::Logger(IAllocator* alloc) :
		luibind(alloc),
		m_logging(false),
		m_pass_types(ELogType::all),
		m_buf_size(0)
	{
		m_mtx = new_mutex();
	}
	RV Logger::log(ELogType log_type, const char* msg)
	{
		if (m_logging)
		{
			return e_bad_calling_time;
		}
		if ((log_type & m_pass_types) == ELogType::none)
		{
			return s_ok;
		}
		size_t len = strlen(msg);
		if (len > g_log_buffer_size - m_buf_size - 1)
		{
			return e_failure;
		}
		MutexGuard g(m_mtx);
		m_logging = true;
		m_block = false;
		m_type = log_type;
		strcpy_s(m_buffer, msg);
		m_buf_size = len;	// ignore null terminator on purpose.
		m_logging = false;
		log_sys::accept_log(this);
		return s_ok;
	}
	RV Logger::vlogf(ELogType log_type, const char* fmt, va_list args)
	{
		if (m_logging)
		{
			return e_bad_calling_time;
		}
		if ((log_type & m_pass_types) == ELogType::none)
		{
			return s_ok;
		}
		MutexGuard guard(m_mtx);
		m_logging = true;
		m_block = false;
		m_type = log_type;
		int len = vsnprintf(m_buffer, g_log_buffer_size, fmt, args);
		m_buf_size = len;
		m_logging = false;
		log_sys::accept_log(this);

		return s_ok;
	}
	RV Logger::logf(ELogType log_type, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		result_t r = vlogf(log_type, fmt, args);
		va_end(args);
		return r;
	}
	RV Logger::begin_log(ELogType log_type, bool as_block)
	{
		if (m_logging)
		{
			return e_bad_calling_time;
		}
		if ((log_type & m_pass_types) == ELogType::none)
		{
			return e_failure;
		}
		m_mtx->wait();
		// clear context.
		m_block = as_block;
		m_type = log_type;
		m_buf_size = 0;
		m_logging = true;
		m_mtx->unlock();
		return s_ok;
	}
	RV Logger::append_msg(const char* msg)
	{
		if (!m_logging)
		{
			return e_bad_calling_time;
		}
		size_t len = strlen(msg);
		if (len > g_log_buffer_size - m_buf_size - 1)
		{
			len = g_log_buffer_size - m_buf_size - 1;
		}
		m_mtx->wait();
		memcpy(m_buffer + m_buf_size, msg, len);
		m_buf_size += len;	// ignore null terminator on purpose.
		m_buffer[g_log_buffer_size - 1] = 0;
		m_mtx->unlock();
		return s_ok;
	}
	RV Logger::append_vmsgf(const char* fmt, va_list args)
	{
		if (!m_logging)
		{
			return e_bad_calling_time;
		}
		m_mtx->wait();
		int len = vsnprintf(m_buffer + m_buf_size, g_log_buffer_size - m_buf_size, fmt, args);
		m_buf_size += len;
		m_buffer[g_log_buffer_size - 1] = 0;
		m_mtx->unlock();
		return s_ok;
	}
	RV Logger::append_msgf(const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		result_t r = append_vmsgf(fmt, args);
		va_end(args);
		return r;
	}
	RV Logger::end_log()
	{
		if (!m_logging)
		{
			return e_bad_calling_time;
		}
		MutexGuard g(m_mtx);
		// Send log.
		log_sys::accept_log(this);
		m_logging = false;
		return s_ok;
	}
}
#endif