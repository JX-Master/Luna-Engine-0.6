// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformDebug.cpp
* @author JXMaster
* @date 2019/12/26
*/
#pragma once
#include "../../Platform.hpp"
#ifdef LUNA_PLATFORM_WINDOWS
namespace luna
{
	namespace platform
	{
		RV debug_printf(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			result_t r = debug_vprintf(fmt, args);
			va_end(args);
			return r;
		}

		RV debug_vprintf(const char* fmt, va_list args)
		{
			char buf[1024];
			vsnprintf_s(buf, 1024, fmt, args);
			::OutputDebugStringA(buf);
			return s_ok;
		}
	}
}
#endif