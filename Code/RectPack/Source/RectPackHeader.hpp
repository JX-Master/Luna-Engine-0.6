// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file RectPackHeader.hpp
* @author JXMaster
* @date 2020/3/8
*/
#pragma once
#include <Base/PlatformDefines.hpp>
#define LUNA_RECT_PACK_API LUNA_EXPORT

#include "../RectPack.hpp"
#include <Core/Core.hpp>

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}