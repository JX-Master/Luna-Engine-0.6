// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GfxHeader.hpp
* @author JXMaster
* @date 2020/1/11
*/
#pragma once
#include <Core/Core.hpp>
#define LUNA_GFX_API LUNA_EXPORT
#include "../Gfx.hpp"

namespace luna
{
	namespace gfx
	{
		extern ILogger* g_logger;
	}

	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}

	inline ILogger* get_logger()
	{
		return gfx::g_logger;
	}
}