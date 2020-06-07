// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Lights.cpp
* @author JXMaster
* @date 2020/5/20
*/
#include "Lights.hpp"

namespace luna
{
	namespace e3d
	{
		Unconstructed<DirectionalLightType> g_directional_light_type;
		Unconstructed<PointLightType> g_point_light_type;
		Unconstructed<SpotlLightType> g_spot_light_type;

		RP<IVariant> DirectionalLight::serialize()
		{
			P<IVariant> var = new_var(EVariantType::table);
			auto intensity = new_var1(EVariantType::f32, 3);
			intensity->f32_buf()[0] = m_intensity.r;
			intensity->f32_buf()[1] = m_intensity.g;
			intensity->f32_buf()[2] = m_intensity.b;
			var->set_field(0, intern_name("intensity"), intensity);
			auto intensity_multiplier = new_var(EVariantType::f32);
			intensity_multiplier->f32() = m_intensity_multiplier;
			var->set_field(0, intern_name("intensity_multiplier"), intensity_multiplier);
			return var;
		}

		RV DirectionalLight::deserialize(IVariant* obj)
		{
			lutry
			{
				lulet(intensity, obj->field(0, intern_name("intensity")));
				lulet(intensity_buf, intensity->check_f32_buf());
				memcpy(m_intensity.m, intensity_buf, sizeof(float32) * 3);
				auto intensity_multiplier = obj->field(0, intern_name("intensity_multiplier"));
				if (succeeded(intensity_multiplier))
				{
					luset(m_intensity_multiplier, intensity_multiplier.get()->check_f32());
				}
				else
				{
					m_intensity_multiplier = 1.0f;
				}
			}
			lucatchret;
			return s_ok;
		}

		scene::IComponentType* DirectionalLight::type_object()
		{
			return &g_directional_light_type.get();
		}


		RP<IVariant> PointLight::serialize()
		{
			P<IVariant> var = new_var(EVariantType::table);
			auto intensity = new_var1(EVariantType::f32, 3);
			intensity->f32_buf()[0] = m_intensity.r;
			intensity->f32_buf()[1] = m_intensity.g;
			intensity->f32_buf()[2] = m_intensity.b;
			var->set_field(0, intern_name("intensity"), intensity);
			auto attenuation_power = new_var(EVariantType::f32);
			attenuation_power->f32() = m_attenuation_power;
			var->set_field(0, intern_name("attenuation_power"), attenuation_power);
			auto intensity_multiplier = new_var(EVariantType::f32);
			intensity_multiplier->f32() = m_intensity_multiplier;
			var->set_field(0, intern_name("intensity_multiplier"), intensity_multiplier);
			return var;
		}

		RV PointLight::deserialize(IVariant* obj)
		{
			lutry
			{
				lulet(intensity, obj->field(0, intern_name("intensity")));
				lulet(intensity_buf, intensity->check_f32_buf());
				memcpy(m_intensity.m, intensity_buf, sizeof(float32) * 3);
				lulet(attenuation_power, obj->field(0, intern_name("attenuation_power")));
				luset(m_attenuation_power, attenuation_power->check_f32());
				auto intensity_multiplier = obj->field(0, intern_name("intensity_multiplier"));
				if (succeeded(intensity_multiplier))
				{
					luset(m_intensity_multiplier, intensity_multiplier.get()->check_f32());
				}
				else
				{
					m_intensity_multiplier = 1.0f;
				}
			}
			lucatchret;
			return s_ok;
		}

		scene::IComponentType* PointLight::type_object()
		{
			return &g_point_light_type.get();
		}

		RP<IVariant> SpotLight::serialize()
		{
			P<IVariant> var = new_var(EVariantType::table);
			auto intensity = new_var1(EVariantType::f32, 3);
			intensity->f32_buf()[0] = m_intensity.r;
			intensity->f32_buf()[1] = m_intensity.g;
			intensity->f32_buf()[2] = m_intensity.b;
			var->set_field(0, intern_name("intensity"), intensity);
			auto attenuation_power = new_var(EVariantType::f32);
			attenuation_power->f32() = m_attenuation_power;
			auto spot_power = new_var(EVariantType::f32);
			spot_power->f32() = m_spot_power;
			var->set_field(0, intern_name("attenuation_power"), attenuation_power);
			var->set_field(0, intern_name("spot_power"), spot_power);
			auto intensity_multiplier = new_var(EVariantType::f32);
			intensity_multiplier->f32() = m_intensity_multiplier;
			var->set_field(0, intern_name("intensity_multiplier"), intensity_multiplier);
			return var;
		}

		RV SpotLight::deserialize(IVariant* obj)
		{
			lutry
			{
				lulet(intensity, obj->field(0, intern_name("intensity")));
				lulet(intensity_buf, intensity->check_f32_buf());
				memcpy(m_intensity.m, intensity_buf, sizeof(float32) * 3);
				lulet(attenuation_power, obj->field(0, intern_name("attenuation_power")));
				lulet(spot_power, obj->field(0, intern_name("spot_power")));
				luset(m_attenuation_power, attenuation_power->check_f32());
				luset(m_spot_power, spot_power->check_f32());
				auto intensity_multiplier = obj->field(0, intern_name("intensity_multiplier"));
				if (succeeded(intensity_multiplier))
				{
					luset(m_intensity_multiplier, intensity_multiplier.get()->check_f32());
				}
				else
				{
					m_intensity_multiplier = 1.0f;
				}
			}
			lucatchret;
			return s_ok;
		}

		scene::IComponentType* SpotLight::type_object()
		{
			return &g_spot_light_type.get();
		}
	}
}