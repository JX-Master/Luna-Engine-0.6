// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file FontHeader.hpp
* @author JXMaster
* @date 2020/3/16
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_FONT_API LUNA_EXPORT

#include "../Font.hpp"
#include <Image/Image.hpp>
#include <RectPack/RectPack.hpp>

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}