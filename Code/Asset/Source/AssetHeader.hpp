// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetHeader.hpp
* @author JXMaster
* @date 2020/2/26
*/
#pragma once
#include <Base/PlatformDefines.hpp>
#define LUNA_ASSET_API LUNA_EXPORT
#include "../Asset.hpp"
#include <Core/Core.hpp>
namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}