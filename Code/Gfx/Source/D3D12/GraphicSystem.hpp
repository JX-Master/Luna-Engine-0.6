// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file GraphicSystem.hpp
* @author JXMaster
* @date 2019/7/10
* @brief D3D12 implementation of GraphicSystem
*/
#pragma once

#include "../GfxHeader.hpp"

#ifdef LUNA_GFX_D3D12

#include "../Windows/GraphicSystemWindows.hpp"

namespace luna
{
	namespace gfx
	{
		namespace d3d12
		{
			class GraphicDevice;
			class SwapChain;

			RV init();
		}
	}
}

#endif