// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file StdLogCallback.cpp
* @author JXMaster
* @date 2020/1/10
*/
#include "StdLogCallback.hpp"

namespace luna
{
	Unconstructed<StdLogCallback> g_std_log_callback;
	void StdLogCallback::on_log(ILog* log, ILogger* sender)
	{
#if defined(LUNA_PLATFORM_WINDOWS)
		switch (log->type())
		{
		case ELogType::comment:
			::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
				FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case ELogType::error:
		case ELogType::fatal_error:
			::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
				FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case ELogType::message:
			::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
				FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case ELogType::warning:
			::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
				FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif
		DateTime dt;
		timestamp_to_data_time(log->timestamp(), dt);
		printf("[%hu-%hu-%hu %hu:%hu:%hu:%hu]", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second, dt.milliseconds);
		printf("[%s]", log->channel()->c_str());
		switch (log->type())
		{
		case ELogType::comment:
			printf("[COMMENT]");
			break;
		case ELogType::error:
			printf("[ERROR]");
			break;
		case ELogType::fatal_error:
			printf("[FATAL ERROR]");
			break;
		case ELogType::message:
			printf("[MESSAGE]");
			break;
		case ELogType::warning:
			printf("[WARNING]");
			break;
		}
		if ((log->flags() & ELogFlag::is_block) != ELogFlag::none)
		{
			printf("\n");
		}
		printf("%s\n", log->message()->c_str());
#if defined(LUNA_PLATFORM_WINDOWS)
		::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE),
			FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
	}
}