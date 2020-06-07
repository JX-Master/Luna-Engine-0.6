// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ILog.hpp
* @author JXMaster
* @date 2020/2/6
*/
#pragma once
#include "ILogger.hpp"

namespace luna
{
	enum class ELogFlag : uint32
	{
		none = 0x00,
		is_block = 0x01,
	};

	//! @interface ILog
	//! Represents one log message.
	struct ILog : public IObject
	{
		luiid("{0e139caf-c2a3-4bad-9a07-f6e4dcaa2319}");

		//! Gets the timestamp of the log message.
		virtual uint64 timestamp() = 0;

		//! Gets the log message.
		virtual IString* message() = 0;

		//! Gets the log channel.
		virtual IName* channel() = 0;

		//! Gets the type of the log.
		virtual ELogType type() = 0;

		//! Gets the log flags.
		virtual ELogFlag flags() = 0;
	};
}