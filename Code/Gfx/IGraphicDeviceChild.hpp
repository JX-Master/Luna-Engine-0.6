// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IDeviceChild.hpp
* @author JXMaster
* @date 2019/7/17
*/
#pragma once
#include <Core/Core.hpp>

namespace Luna
{
	namespace Gfx
	{
		struct IGraphicDevice;
		//! @interface IGraphicDeviceChild
		//! Represents a object that is managed by a graphic device (`IGraphicDevice`).
		struct IGraphicDeviceChild : public IObject
		{
			luiid("{324864f4-8489-4f6c-aa9b-2e96ea4c31ce}");

			//! Get the parent device that manages this object.
			virtual IGraphicDevice* get_device() = 0;
		};
	}
}