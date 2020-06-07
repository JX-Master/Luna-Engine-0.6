// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file XBOXController.cpp
* @author JXMaster
* @date 2019/7/27
*/
#include "XBOXController.hpp"

#ifdef LUNA_PLATFORM_WINDOWS

#pragma comment(lib, "xinput.lib")

namespace luna
{
	namespace input
	{
		namespace win
		{
			inline void normalize_thumb(float32 x, float32 y, float32 deadzone, float32& outx, float32& outy)
			{
				float32 l = sqrtf(x * x + y * y);
				l = max(l, 0.00001f);	// prevent dividing by 0.
				float32 lc = max(l, deadzone);
				lc -= deadzone;
				float32 scalef = lc / l / (32767.0f - deadzone);
				outx = x * scalef;
				outy = y * scalef;
			}

			inline void normalize_trigger(float32 inv, float32 deadzone, float32& outv)
			{
				constexpr float32 max_trigger = 255.0f;
				inv = max(inv, deadzone);
				outv = (inv - deadzone) / (max_trigger - deadzone);
			}

			bool XBOXController::connected(uint32 index)
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
			bool XBOXController::button_down(uint32 index, EControllerButton button)
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
			float32 XBOXController::axis(uint32 index, EControllerAxis axis)
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
					float32 x, y;
					normalize_thumb(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, x, y);
					return x;
				}
				case EControllerAxis::ly:
				{
					float32 x, y;
					normalize_thumb(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, x, y);
					return y;
				}
				case EControllerAxis::rx:
				{
					float32 x, y;
					normalize_thumb(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, x, y);
					return x;
				}
				case EControllerAxis::ry:
				{
					float32 x, y;
					normalize_thumb(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE, x, y);
					return y;
				}
				case EControllerAxis::lt:
				{
					float32 ret;
					normalize_trigger(state.Gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, ret);
					return ret;
				}
				case EControllerAxis::rt:
				{
					float32 ret;
					normalize_trigger(state.Gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, ret);
					return ret;
				}
				default:
					lupanic();
					break;
				}
				return 0.0f;
			}
			float32 XBOXController::vibration(uint32 index, EControllerVibrationMotor part)
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
			RV XBOXController::set_vibration(uint32 index, EControllerVibrationMotor part, float32 level)
			{
				MutexGuard g(m_vib_mtx);
				if (index >= XUSER_MAX_COUNT)
				{
					return e_failure;
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
				return (r == ERROR_SUCCESS) ? s_ok : e_failure;
			}
		}
	}
}

#endif