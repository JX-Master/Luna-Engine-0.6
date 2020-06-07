// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CoreHeader.hpp
* @author JXMaster
* @date 2019/1/9
*/
#pragma once
#include <Base/PlatformDefines.hpp>
#define LUNA_CORE_API LUNA_EXPORT
#include "../Core.hpp"

#ifdef LUNA_PLATFORM_WINDOWS
#include <Base/Platform/Windows/MiniWin.hpp>
#endif

namespace luna
{
	namespace core
	{
		extern ILogger* g_ilogger;
	}

	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
	inline ILogger* get_logger()
	{
		return core::g_ilogger;
	}

	// Used internally by serialization system. Not exposed to user.
	constexpr result_t e_eof = e_user_failure - 1;

}