// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImageHeader.hpp
* @author JXMaster
* @date 2020/3/6
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_IMAGE_API LUNA_EXPORT

#include "../Image.hpp"
#include <Core/Core.hpp>

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}