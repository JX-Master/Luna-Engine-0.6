// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IWindowInputDevice.hpp
* @author JXMaster
* @date 2020/2/10
*/
#pragma once
#include <Input/IInputDevice.hpp>
#include "IWindowEventListener.hpp"

namespace luna
{
	namespace gfx
	{
		struct IWindowInputDevice : public input::IInputDevice
		{
			luiid("{403d5af0-6755-4915-afbb-c289f194847c}");

			//! Adds one listener to listen window events. The window input device keeps a weak reference
			//! to the listener.
			virtual void add_listener(IWindowEventListener* listener) = 0;

			//! Removes one listener.
			virtual RV remove_listener(IWindowEventListener* listener) = 0;
		};
	}
}