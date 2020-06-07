// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TextureHeader.hpp
* @author JXMaster
* @date 2020/5/8
*/
#pragma once
#include <Base/PlatformDefines.hpp>
#include <Core/Core.hpp>

#define LUNA_TEXTURE_API LUNA_EXPORT

#include "../ITexture.hpp"
#include "../Texture.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}