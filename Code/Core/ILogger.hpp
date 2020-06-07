// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ILogger.hpp
* @author JXMaster
* @date 2019/4/9
*/
#pragma once
#include <Base/IObject.hpp>
#include <stdarg.h>
#include "IName.hpp"
namespace luna
{
	//! Defines the type of the log. Different log will be handled differently by log handlers.
	enum class ELogType : uint32
	{
		none = 0,
		fatal_error = 0x01,
		error = 0x02,
		warning = 0x04,
		message = 0x08,
		comment = 0x10,
		all = fatal_error | error | warning | message | comment,
	};

	//! @interface ILogger
	//! @threadsafe
	//! Represents a logger that sends log to the log system.
	struct ILogger : public IObject
	{
		luiid("{89ea433a-0a2d-4446-8684-060812be4a6a}");

		//! Sets the channel this logger will log to.
		virtual RV set_channel(IName* channel) = 0;

		//! Gets the current bound channel.
		virtual P<IName> channel() = 0;

		//! Sets the type of logs that will be logged.
		//! @param[in] types A bit-OR combination of `ELogType`s that identifying the type of logs that will be logged.
		virtual void set_pass_log_types(ELogType types) = 0;

		//! Gets the type of logs that are currently set_char to be logged.
		virtual ELogType pass_log_types() = 0;

		//! Send the message as log from this logger to all specified channels. The log is always in inlined
		//! format.
		//! 
		//! You can not send log by calling this when another log message is recording after `begin_log`
		//! and before `end_log`.
		virtual RV log(ELogType log_type, const char* msg) = 0;
		//! Format the message and then send the message as log from this logger to all specified channels.
		//! The log is always in inlined format.
		//! 
		//! You can not send log by calling this when another log message is recording after `begin_log`
		//! and before `end_log`.
		virtual RV vlogf(ELogType log_type, const char* fmt, va_list args) = 0;
		virtual RV logf(ELogType log_type, const char* fmt, ...) = 0;

		// Complex log method. Can be used to dump a log block.
		//! Prepare this logger to record a new log message.
		//! @param[in] log_type The type of the log, one of the ELogType items.
		//! @param[in] as_block If to format this log as a block. The implementation
		//! of formatting blocked / inlined log message is dependent to the log handler,
		//! but most of the handler will print the blocked log content in a new line.
		virtual RV begin_log(ELogType log_type, bool as_block = false) = 0;
		//! Append a new log part into the internal log buffer of this logger. 
		virtual RV append_msg(const char* msg) = 0;
		virtual RV append_vmsgf(const char* fmt, va_list args) = 0;
		virtual RV append_msgf(const char* fmt, ...) = 0;
		//! Finishes this message and send it as log to all specified channels.
		virtual RV end_log() = 0;
	};
}