// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IMouse.hpp
* @author JXMaster
* @date 2019/7/24
*/
#pragma once
#include "IInputDevice.hpp"
#include "KeyCode.hpp"
#include <Base/Math/Vector.hpp>

namespace luna
{
	namespace input
	{
		//! @interface IMouse
		//! Represents the mouse input device. Get this interface from `IInputSystem::get_device("mouse")`.
		struct IMouse : public IInputDevice
		{
			luiid("{7d3a9e2e-eff4-4341-99e0-78ef9c36f7dd}");

			//! Get the position of the mouse cursor in screen space.
			//! @return The position of the mouse cursor in screen space.
			virtual Int2U pos() = 0;

			//! Checks if the specified mouse key is pressed.
			//! @return `true` if the mouse button is down, returns `false` otherwise.
			virtual bool key_down(EMouseKey mouse_key) = 0;

			//! Sets the OS mouse cursor position. The position is based on the screen coordinates.
			//! This only works for Desktop-based system.
			virtual RV set_pos(int32 x, int32 y) = 0;
		};
	}
}