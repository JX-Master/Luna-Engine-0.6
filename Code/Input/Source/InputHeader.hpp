// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Input.hpp
* @author JXMaster
* @date 2020/2/8
*/
#pragma once
#include <Base/PlatformDefines.hpp>
#define LUNA_INPUT_API LUNA_EXPORT
#include "../Input.hpp"
#include <Core/Core.hpp>

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}