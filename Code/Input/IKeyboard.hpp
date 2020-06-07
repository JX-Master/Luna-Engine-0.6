// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IKeyboard.hpp
* @author JXMaster
* @date 2019/7/25
*/
#pragma once
#include "IInputDevice.hpp"
#include "KeyCode.hpp"

namespace luna
{
	namespace input
	{
		//! @interface IKeyboard
		//! Represent keyboard input. Get this interface from `IInputSystem::get_device("keyboard")`.
		struct IKeyboard : public IInputDevice
		{
			luiid("{605cf4da-d0dd-4fcf-bfc1-0567f59b6f5f}");

			//! Checks the if the specified key is pressed.
			//! @param[in] key The key to check.
			//! @return	`true` if the key is pressed, `false` otherwise.
			virtual bool keydown(EKeyCode key) = 0;
		};
	}
}