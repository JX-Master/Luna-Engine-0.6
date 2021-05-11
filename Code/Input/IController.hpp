// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file IController.hpp
* @author JXMaster
* @date 2019/7/25
*/
#pragma once
#include "IInputDevice.hpp"
#include "KeyCode.hpp"

namespace Luna
{
	namespace Input
	{
		//! @interface IController
		//! Represents a general controller device.
		struct IController : public IInputDevice
		{
			luiid("{c4317d5e-766f-4103-b9d5-06d23c6b303d}");

			//! Checks if the controller at specified index is connected and available.
			//! @param[in] index The 0-based index of controllers to check.
			//! @return Returns `true` if the controller is connected, `false` otherwise.
			virtual bool connected(u32 index) = 0;

			//! Checks if the specified button is pressed.
			//! @param[in] index The 0-based index of controllers to check.
			//! @param[in] button The button to check.
			//! @return Returns `true` if the button is down, `false` otherwise. If the controller is not connected or the specified button
			//! does not exist on the controller, returns `false` always.
			virtual bool button_down(u32 index, EControllerButton button) = 0;

			//! Gets the axis value mapped to [-1, 1] for thumb pad, or [0, 1] for shoulder trigger.
			//! The returned value is already dead zone clamped.
			//! @param[in] index The 0-based index of controllers to check.
			//! @param[in] axis_to_check The axis to check.
			//! @return Returns the mapped axis value. If the controller is not connected or the specified axis does not exist on the controller, 
			//! returns `0.0f` always.
			virtual f32 axis(u32 index, EControllerAxis axis_to_check) = 0;

			//! Gets the vibration level. 
			//! @param[in] index The 0-based index of controllers to get vibration.
			//! @param[in] part The left or right vibration engine to operate.
			//! @return Returns the vibration level, 0.0f means no vibration and 1.0f means full vibration.
			virtual f32 vibration(u32 index, EControllerVibrationMotor part) = 0;

			//! Sets the vibration level.
			//! @param[in] index The 0-based index of controllers to set vibration.
			//! @param[in] part The left or right vibration engine to operate.
			//! @param[in] level The vibration level to set. 0.0f means no vibration and 1.0f means full vibration.
			//! @return Returns `RV()` if the vibration setting is succeed. Returns failure otherwise.
			virtual RV set_vibration(u32 index, EControllerVibrationMotor part, f32 level) = 0;
		};
	}
}