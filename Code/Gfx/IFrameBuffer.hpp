// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IFrameBuffer.hpp
* @author JXMaster
* @date 2020/3/10
*/
#pragma once
#include "IGraphicDeviceChild.hpp"
namespace luna
{
	namespace gfx
	{
		//! @interface IFrameBuffer
		//! The frame buffer object holds API-specific data to bind several render target views to one pass.
		struct IFrameBuffer : public IGraphicDeviceChild
		{
			luiid("{5b28409c-f0a6-4da3-80ef-54564384520a}");

			// No additional methods.
		};
	}
}