// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file InputSystem.hpp
* @author JXMaster
* @date 2020/2/8
*/
#pragma once
#include "InputHeader.hpp"
#include <Base/Interface.hpp>

namespace luna
{
	namespace input
	{
		extern Unconstructed<Vector<Pair<P<IName>, P<IInputDevice>>>> m_devices;
		extern P<IMutex> m_mtx;

		// This can be set by the platform to keep a strong
		// reference to the drivers.
		extern P<IObject> m_platform_devices;

		void remove_empty();

		LUNA_INPUT_API RV init();
		void deinit();
		LUNA_INPUT_API void update();
		LUNA_INPUT_API Vector<P<IName>> get_devices();
		LUNA_INPUT_API RP<IInputDevice> get_device(IName* device_name);
		LUNA_INPUT_API RV mount_device(IName* device_name, IInputDevice* device);
		LUNA_INPUT_API RV unmount_device(IName* device_name);

		// This can be implemented by the platform to add platform-specific drivers.
		RV platform_input_init();
	}
}