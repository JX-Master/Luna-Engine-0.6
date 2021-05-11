// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file PlatformInit.hpp
* @author JXMaster
* @date 2019/7/27
*/
#pragma once
#include "../InputSystem.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "XBOXController.hpp"
#include <Core/Core.hpp>

namespace Luna
{
	namespace Input
	{
		namespace Win
		{
			// A simple object that manages the lifetime of the drivers.
			class WindowsInputDevices : public IObject
			{
			public:
				lucid("{6d997510-b632-46bb-bf3a-e8e408806775}");
				luiimpl_static(WindowsInputDevices, IObject);

				P<Mouse> m_mouse;
				P<Keyboard> m_keyboard;
				P<XBOXController> m_controller;
			};

			Unconstructed<WindowsInputDevices> g_input_devices;
			Unconstructed<Mouse> g_mouse;
			Unconstructed<Keyboard> g_keyboard;
			Unconstructed<XBOXController> g_xbox_controller;
		}

		RV platform_input_init()
		{
			Win::g_mouse.construct();
			Win::g_keyboard.construct();
			Win::g_xbox_controller.construct();
			Win::g_xbox_controller.get().m_vib_mtx = new_mutex();
			Win::g_input_devices.construct();
			Win::g_input_devices.get().m_controller = box_ptr(&(Win::g_xbox_controller.get()));
			Win::g_input_devices.get().m_mouse = box_ptr(&(Win::g_mouse.get()));
			Win::g_input_devices.get().m_keyboard = box_ptr(&(Win::g_keyboard.get()));
			// register devices.
			m_platform_devices = box_ptr<IObject>(&(Win::g_input_devices.get()));
			lutry
			{
				luexp(mount_device(Name("mouse"), &(Win::g_mouse.get())));
				luexp(mount_device(Name("keyboard"), &(Win::g_keyboard.get())));
				luexp(mount_device(Name("controller"), &(Win::g_xbox_controller.get())));
			}
			lucatchret;
			return RV();
		}
	}
}

#endif