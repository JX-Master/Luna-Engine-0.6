// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ImGuiHeader.hpp
* @author JXMaster
* @date 2020/4/5
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#define LUNA_IMGUI_API LUNA_EXPORT

#include "../ImGui.hpp"

namespace luna
{
	inline IAllocator* get_module_allocator()
	{
		return get_global_heap();
	}
}