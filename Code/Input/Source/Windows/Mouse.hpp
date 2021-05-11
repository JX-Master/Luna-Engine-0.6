// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mouse.hpp
* @author JXMaster
* @date 2019/7/26
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include "../InputHeader.hpp"
#include "../../IMouse.hpp"
#include <Core/Interface.hpp>

#include <Runtime/Source/Platform/Windows/MiniWin.hpp>

namespace Luna
{
	namespace Input
	{
		namespace Win
		{
			class Mouse : public IMouse
			{
			public:
				lucid("{956d13f9-1937-45ea-8bc6-51be5a68a9e0}");
				luiimpl_static(Mouse, IMouse, IInputDevice, IObject);

				virtual EInputDeviceFlag flags() override
				{
					return EInputDeviceFlag::none;
				}

				virtual void update() override {}

				virtual Int2U pos() override
				{
					POINT p;
					::GetCursorPos(&p);
					return Int2U(p.x, p.y);
				}
				virtual bool key_down(EMouseKey mouse_key) override;
				virtual RV set_pos(i32 x, i32 y) override;
			};
		}
	}
}

#endif