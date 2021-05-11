// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ILight.hpp
* @author JXMaster
* @date 2020/5/14
*/
#pragma once
#include <Scene/IComponent.hpp>
#include <Runtime/Math.hpp>
namespace Luna
{
	namespace E3D
	{
		//! @interface ILight
		struct ILight : public Scene::IComponent
		{
			luiid("{8aba0010-86f7-4494-9121-747309f1643d}");

			virtual Float3 intensity() = 0;
			virtual void set_intensity(const Float3& intensity_value) = 0;
			virtual f32 intensity_multiplier() = 0;
			virtual void set_intensity_multiplier(f32 multiplier) = 0;
		};

		struct IDirectionalLight : public ILight
		{
			luiid("{d85615e9-5c38-438b-92a2-466f7fcbe0e2}");

			// No additional methods.
		};

		struct IPointLight : public ILight
		{
			luiid("{a941756e-bcb6-4cc0-a192-6a80d0ac3b4b}");

			virtual f32 attenuation_power() = 0;
			virtual void set_attenuation_power(f32 value) = 0;
		};

		struct ISpotLight : public ILight
		{
			luiid("{c58ffa5e-d3c8-4bfa-a096-53e74624a44b}");

			virtual f32 attenuation_power() = 0;
			virtual f32 spot_power() = 0;
			virtual void set_attenuation_power(f32 value) = 0;
			virtual void set_spot_power(f32 value) = 0;
		};
	}
}