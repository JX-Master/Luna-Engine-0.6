// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file XBOXController.hpp
* @author JXMaster
* @date 2019/7/27
*/
#pragma once
#include <Runtime/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include "../InputHeader.hpp"
#include "../../IController.hpp"
#include <Core/Core.hpp>
#include <Core/Interface.hpp>
#include <Xinput.h>

namespace Luna
{
	namespace Input
	{
		namespace Win
		{
			struct XBOXControllerVibration
			{
				f32 vibration_l;
				f32 vibration_r;
			};
			
			class XBOXController : public IController
			{
			public:
				lucid("{bbe0f185-d5ea-4914-90b5-0b19d1db946c}");
				luiimpl_static(XBOXController, IController, IInputDevice, IObject);

				P<IMutex> m_vib_mtx;
				XBOXControllerVibration m_vibrations[XUSER_MAX_COUNT];

				XBOXController()
				{
					memzero(m_vibrations, sizeof(XBOXControllerVibration) * 4);
				}

				virtual EInputDeviceFlag flags() override
				{
					return EInputDeviceFlag::none;
				}
				virtual void update() override {}
				virtual bool connected(u32 index) override;
				virtual bool button_down(u32 index, EControllerButton button) override;
				virtual f32 axis(u32 index, EControllerAxis axis_to_check) override;
				virtual f32 vibration(u32 index, EControllerVibrationMotor part) override;
				virtual RV set_vibration(u32 index, EControllerVibrationMotor part, f32 level) override;
			};
		}
	}
}

#endif