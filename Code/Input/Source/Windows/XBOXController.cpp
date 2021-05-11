// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file XBOXController.cpp
* @author JXMaster
* @date 2019/7/27
*/
#include "XBOXController.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#pragma comment(lib, "xinput.lib")

namespace Luna
{
	namespace Input
	{
		namespace Win
		{
			inline void normalize_thumb(f32 x, f32 y, f32 deadzone, f32& outx, f32& outy)
			{
				f32 l = sqrtf(x * x + y * y);
				l = max(l, 0.00001f);	// prevent dividing by 0.
				f32 lc = max(l, deadzone);
				lc -= deadzone;
				f32 scalef = lc / l / (32767.0f - deadzone);
				outx = x * scalef;
				outy = y * scalef;
			}

			inline void normalize_trigger(f32 inv, f32 deadzone, f32& outv)
			{
				constexpr f32 max_trigger = 255.0f;
				inv = max(inv, deadzone);
				outv = (inv - deadzone) / (max_trigger - deadzone);
			}

			bool XBOXController::connected(u32 index)
			{
				if (index >= XUSER_MAX_COUNT)
				{
					return false;
				}
				XINPUT_STATE state;
				DWORD dw = XInputGetState(index, &state);
				if (dw != ERROR_SUCCESS)
				{
					return false;
				}
				return true;
			}
			bool XBOXController::button_down(u32 index, EControllerButton button)
			{
				if (index >= XUSER_MAX_COUNT)
				{
					return false;
				}
				XINPUT_STATE state;
				DWORD dw = XInputGetState(index, &state);
				if (dw != ERROR_SUCCESS)
				{
					return false;
				}
				switch (button)
				{
				case EControllerButton::lthumb:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
				case EControllerButton::rthumb:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
				case EControllerButton::lup:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
				case EControllerButton::ldown:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
				case EControllerButton::lleft:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
				case EControllerButton::lright:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
				case EControllerButton::rup:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
				case EControllerButton::rdown:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
				case EControllerButton::rleft:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
				case EControllerButton::rright:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
				case EControllerButton::lb:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
				case EControllerButton::rb:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
				case EControllerButton::lt:
					return state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? true : false;
				case EControllerButton::rt:
					return state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? true : false;
				case EControllerButton::lspecial:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
				case EControllerButton::rspecial:
					return state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
				default:
					lupanic();
					break;
				}
				return false;
			}
			f32 XBOXController::axis(u32 index, EControllerAxis axis)
			{
				if (index >= XUSER_MAX_COUNT)
				{
					return 0.0f;
				}
				XINPUT_STATE state;
				DWORD dw = XInputGetState(index, &state);
				if (dw != ERROR_SUCCESS)
				{
					return 0.0f;
				}
				switch (axis)
				{
				case EControllerAxis::lx:
				{
					f32 x, y;
					normalize_thumb(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, x, y);
					return x;
				}
				case EControllerAxis::ly:
				{
					f32 x, y;
					normalize_thumb(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, x, y);
					return y;
				}
				case EControllerAxis::rx:
				{
					f32 x, y;
					normalize_thumb(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, x, y);
					return x;
				}
				case EControllerAxis::ry:
				{
					f32 x, y;
					normalize_thumb(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, x, y);
					return y;
				}
				case EControllerAxis::lt:
				{
					f32 ret;
					normalize_trigger(state.Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, ret);
					return ret;
				}
				case EControllerAxis::rt:
				{
					f32 ret;
					normalize_trigger(state.Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, ret);
					return ret;
				}
				default:
					lupanic();
					break;
				}
				return 0.0f;
			}
			f32 XBOXController::vibration(u32 index, EControllerVibrationMotor part)
			{
				MutexGuard g(m_vib_mtx);
				if (index >= XUSER_MAX_COUNT)
				{
					return 0.0f;
				}
				switch (part)
				{
				case EControllerVibrationMotor::left:
					return m_vibrations[index].vibration_l;
				case EControllerVibrationMotor::right:
					return m_vibrations[index].vibration_r;
				default:
					lupanic();
					break;
				}
				return 0.0f;
			}
			RV XBOXController::set_vibration(u32 index, EControllerVibrationMotor part, f32 level)
			{
				MutexGuard g(m_vib_mtx);
				if (index >= XUSER_MAX_COUNT)
				{
					return BasicError::bad_arguments();
				}
				switch (part)
				{
				case EControllerVibrationMotor::left:
					m_vibrations[index].vibration_l = level;
					break;
				case EControllerVibrationMotor::right:
					m_vibrations[index].vibration_r = level;
					break;
				default:
					lupanic();
					break;
				}
				XINPUT_VIBRATION vib;
				vib.wLeftMotorSpeed = (WORD)(m_vibrations[index].vibration_l * 65535.0f);
				vib.wRightMotorSpeed = (WORD)(m_vibrations[index].vibration_r * 65535.0f);
				DWORD r = XInputSetState(index, &vib);
				return (r == ERROR_SUCCESS) ? RV() : BasicError::bad_system_call();
			}
		}
	}
}

#endif