// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneHeader.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_SCENE_API LUNA_EXPORT
#include "../Scene.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}