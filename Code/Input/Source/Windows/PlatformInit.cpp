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

namespace luna
{
	namespace input
	{
		namespace win
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
			win::g_mouse.construct();
			win::g_keyboard.construct();
			win::g_xbox_controller.construct();
			win::g_xbox_controller.get().m_vib_mtx = new_mutex();
			win::g_input_devices.construct();
			win::g_input_devices.get().m_controller = box_ptr(&(win::g_xbox_controller.get()));
			win::g_input_devices.get().m_mouse = box_ptr(&(win::g_mouse.get()));
			win::g_input_devices.get().m_keyboard = box_ptr(&(win::g_keyboard.get()));
			// register devices.
			m_platform_devices = box_ptr<IObject>(&(win::g_input_devices.get()));
			lutry
			{
				luexp(mount_device(intern_name("mouse"), &(win::g_mouse.get())));
				luexp(mount_device(intern_name("keyboard"), &(win::g_keyboard.get())));
				luexp(mount_device(intern_name("controller"), &(win::g_xbox_controller.get())));
			}
			lucatchret;
			return s_ok;
		}
	}
}

#endif