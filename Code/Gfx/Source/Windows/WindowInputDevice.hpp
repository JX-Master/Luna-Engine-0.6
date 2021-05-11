// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file WindowInputDevice.hpp
* @author JXMaster
* @date 2020/2/10
*/
#include "../../IWindowInputDevice.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <Runtime/Vector.hpp>
#include <Core/Interface.hpp>

namespace Luna
{
	namespace Gfx
	{
		namespace Win
		{
			class WindowInputDevice : public IWindowInputDevice
			{
			public:
				lucid("{10f9e666-98d2-4aa1-bd1b-ccbd494647e3}");
				luiimpl_static(WindowInputDevice, IWindowInputDevice, Input::IInputDevice, IObject);

				P<IMutex> m_mtx;
				Vector<WP<IWindowEventListener>> m_listeners;

				WindowInputDevice() {}

				void remove_invalid_listeners();

				bool dispatch_event(IWindow* window, EWindowEvent e, void* data);

				virtual Input::EInputDeviceFlag flags() override
				{
					return Input::EInputDeviceFlag::buffered;
				}
				virtual void update() override;
				virtual void add_listener(IWindowEventListener* listener) override;
				virtual RV remove_listener(IWindowEventListener* listener) override;
			};

			extern Unconstructed<WindowInputDevice> g_window_input;
		}
	}
}

#endif