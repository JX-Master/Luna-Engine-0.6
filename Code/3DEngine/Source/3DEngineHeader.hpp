// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file 3DEngineHeader.hpp
* @author JXMaster
* @date 2020/5/10
*/
#pragma once
#include <Base/PlatformDefines.hpp>
#define LUNA_3DENGINE_API LUNA_EXPORT

#include "../3DEngine.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}