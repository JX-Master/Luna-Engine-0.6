// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Debug.cpp
* @author JXMaster
* @date 2020/9/22
 */
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_POSIX

#include "../../OS.hpp"

namespace Luna
{
	namespace OS
	{
		void debug_printf(const c8* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			debug_vprintf(fmt, args);
			va_end(args);
		}
		void debug_vprintf(const c8* fmt, VarList args)
		{
			char buf[1024];
			vsnprintf(buf, 1024, fmt, args);
			printf("%s\n", buf);
		}
	}
}

#endif
