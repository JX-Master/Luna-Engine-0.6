// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file XBOXController.hpp
* @author JXMaster
* @date 2019/7/27
*/
#pragma once
#include <Base/PlatformDefines.hpp>

#ifdef LUNA_PLATFORM_WINDOWS

#include "../InputHeader.hpp"
#include "../../IController.hpp"
#include <Base/Interface.hpp>
#include <Xinput.h>

namespace luna
{
	namespace input
	{
		namespace win
		{
			struct XBOXControllerVibration
			{
				float32 vibration_l;
				float32 vibration_r;
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
				virtual bool connected(uint32 index) override;
				virtual bool button_down(uint32 index, EControllerButton button) override;
				virtual float32 axis(uint32 index, EControllerAxis axis_to_check) override;
				virtual float32 vibration(uint32 index, EControllerVibrationMotor part) override;
				virtual RV set_vibration(uint32 index, EControllerVibrationMotor part, float32 level) override;
			};
		}
	}
}

#endif