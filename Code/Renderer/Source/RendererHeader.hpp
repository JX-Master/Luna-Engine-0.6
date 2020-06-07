// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RendererHeader.hpp
* @author JXMaster
* @date 2020/3/9
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_RENDERER_API LUNA_EXPORT

#include "../Renderer.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}