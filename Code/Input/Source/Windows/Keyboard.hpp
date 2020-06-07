// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Keyboard.hpp
* @author JXMaster
* @date 2019/7/26
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include "../InputHeader.hpp"
#include "../../IKeyboard.hpp"
#include <Base/Interface.hpp>

#include <Base/Platform/Windows/MiniWin.hpp>

namespace luna
{
	namespace input
	{
		namespace win
		{
			class Keyboard : public IKeyboard
			{
			public:
				lucid("{956d13f9-1937-45ea-8bc6-51be5a68a9e0}");
				luiimpl_static(Keyboard, IKeyboard, IInputDevice, IObject);

				virtual EInputDeviceFlag flags() override
				{
					return EInputDeviceFlag::none;
				}

				virtual void update() override {}

				virtual bool keydown(EKeyCode key) override
				{
					return (GetAsyncKeyState((int)key) & 0x8000) ? true : false;
				}
			};
		}
	}
}

#endif