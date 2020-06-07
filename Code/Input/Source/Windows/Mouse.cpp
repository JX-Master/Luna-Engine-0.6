// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Mouse.cpp
* @author JXMaster
* @date 2019/7/26
*/
#pragma once
#include "Mouse.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#include <Base/Platform/Windows/MiniWin.hpp>

namespace luna
{
	namespace input
	{
		namespace win
		{
			bool Mouse::key_down(EMouseKey mouse_key)
			{
				int key;
				switch (mouse_key)
				{
				case EMouseKey::lb:
					key = VK_LBUTTON;
					break;
				case EMouseKey::rb:
					key = VK_RBUTTON;
					break;
				case EMouseKey::mb:
					key = VK_MBUTTON;
					break;
				case EMouseKey::x1b:
					key = VK_XBUTTON1;
					break;
				case EMouseKey::x2b:
					key = VK_XBUTTON2;
					break;
				default:
					return false;
				}
				return (GetAsyncKeyState(key) & 0x8000) ? true : false;
			}
			RV Mouse::set_pos(int32 x, int32 y)
			{
				BOOL r = ::SetCursorPos(x, y);
				return r ? s_ok : e_failure;
			}
		}
	}
}

#endif