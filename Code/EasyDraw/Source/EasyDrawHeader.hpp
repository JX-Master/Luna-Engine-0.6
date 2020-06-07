// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file EasyDrawHeader.hpp
* @author JXMaster
* @date 2020/3/18
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_EASY_DRAW_API LUNA_EXPORT
#include "../EasyDraw.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}