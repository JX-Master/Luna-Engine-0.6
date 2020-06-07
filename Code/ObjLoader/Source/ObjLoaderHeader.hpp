// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ObjLoaderHeader.hpp
* @author JXMaster
* @date 2020/5/12
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_OBJ_LOADER_API LUNA_EXPORT
#include "../ObjLoader.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}