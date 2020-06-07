// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Log.hpp
* @author JXMaster
* @date 2020/2/6
*/
#pragma once
#include "../CoreHeader.hpp"

namespace luna
{
	class Log : public ILog
	{
	public:
		lucid("{5f7c74af-09b2-4145-91ed-21fad4c3da1b}");
		luiimpl(Log, ILog, IObject);

		uint64 m_timestamp;
		P<IString> m_message;
		P<IName> m_channel;
		ELogType m_type;
		ELogFlag m_flags;

		Log(IAllocator* alloc) :
			luibind(alloc) {}

		virtual uint64 timestamp() override
		{
			return m_timestamp;
		}

		virtual IString* message() override
		{
			return m_message.get();
		}

		virtual IName* channel() override
		{
			return m_channel.get();
		}

		virtual ELogType type() override
		{
			return m_type;
		}

		virtual ELogFlag flags() override
		{
			return m_flags;
		}
	};
}