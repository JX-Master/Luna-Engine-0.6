// Copyright 2018-2021 JXMaster. All rights reserved.
/*
* @file Debug.hpp
* @author JXMaster
* @date 2020/12/26
*/
#include <Runtime/PlatformDefines.hpp>
#define LUNA_RUNTIME_API LUNA_EXPORT
#include "../Debug.hpp"
#include "OS.hpp"

namespace Luna
{
	LUNA_RUNTIME_API void debug_printf(const c8* fmt, ...)
	{
		VarList args;
		va_start(args, fmt);
		OS::debug_vprintf(fmt, args);
		va_end(args);
	}
	LUNA_RUNTIME_API void debug_vprintf(const c8* fmt, VarList args)
	{
		OS::debug_vprintf(fmt, args);
	}
}