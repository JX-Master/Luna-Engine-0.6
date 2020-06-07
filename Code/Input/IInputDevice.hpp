// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IInputDeviceContext.hpp
* @author JXMaster
* @date 2020/2/8
*/
#pragma once
#include <Base/IObject.hpp>

namespace luna
{
	namespace input
	{
		enum class EInputDeviceFlag : uint32
		{
			none = 0x00,
			//! The state of this device is buffered and only gets updated when `update` is called. This can be useful if 
			//! the input data size is very big and it is not efficient to update the data every time the data is queried.
			//! Input devices such as high resolution cameras may use this.
			buffered = 0x01,
		};

		//! @interface IInputDevice
		//! @threadsafe
		//! Represents one input handler for the input device.
		struct IInputDevice : public IObject
		{
			luiid("{cc05fd14-30ca-4a7a-b112-33bfe2190eae}");

			//! Gets the flags of this device.
			virtual EInputDeviceFlag flags() = 0;

			//! Updates the state of this device.
			virtual void update() = 0;
		};
	}
}