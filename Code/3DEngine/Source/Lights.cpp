// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Lights.cpp
* @author JXMaster
* @date 2020/5/20
*/
#include "Lights.hpp"

namespace Luna
{
	namespace E3D
	{
		Unconstructed<DirectionalLightType> g_directional_light_type;
		Unconstructed<PointLightType> g_point_light_type;
		Unconstructed<SpotlLightType> g_spot_light_type;

		R<Variant> DirectionalLight::serialize()
		{
			Variant var = Variant(EVariantType::table);
			auto intensity = Variant(EVariantType::f32, 3);
			intensity.to_f32_buf()[0] = m_intensity.r;
			intensity.to_f32_buf()[1] = m_intensity.g;
			intensity.to_f32_buf()[2] = m_intensity.b;
			var.set_field(0, Name("intensity"), intensity);
			auto intensity_multiplier = Variant(EVariantType::f32);
			intensity_multiplier.to_f32() = m_intensity_multiplier;
			var.set_field(0, Name("intensity_multiplier"), intensity_multiplier);
			return var;
		}

		RV DirectionalLight::deserialize(const Variant& obj)
		{
			lutry
			{
				auto& intensity = obj.field(0, Name("intensity"));
				lulet(intensity_buf, intensity.check_f32_buf());
				memcpy(m_intensity.m, intensity_buf, sizeof(f32) * 3);
				auto& intensity_multiplier = obj.field(0, Name("intensity_multiplier"));
				if (intensity_multiplier.type() != EVariantType::null)
				{
					luset(m_intensity_multiplier, intensity_multiplier.check_f32());
				}
				else
				{
					m_intensity_multiplier = 1.0f;
				}
			}
			lucatchret;
			return RV();
		}

		Scene::IComponentType* DirectionalLight::type_object()
		{
			return &g_directional_light_type.get();
		}


		R<Variant> PointLight::serialize()
		{
			Variant var = Variant(EVariantType::table);
			auto intensity = Variant(EVariantType::f32, 3);
			intensity.to_f32_buf()[0] = m_intensity.r;
			intensity.to_f32_buf()[1] = m_intensity.g;
			intensity.to_f32_buf()[2] = m_intensity.b;
			var.set_field(0, Name("intensity"), intensity);
			auto attenuation_power = Variant(EVariantType::f32);
			attenuation_power.to_f32() = m_attenuation_power;
			var.set_field(0, Name("attenuation_power"), attenuation_power);
			auto intensity_multiplier = Variant(EVariantType::f32);
			intensity_multiplier.to_f32() = m_intensity_multiplier;
			var.set_field(0, Name("intensity_multiplier"), intensity_multiplier);
			return var;
		}

		RV PointLight::deserialize(const Variant& obj)
		{
			lutry
			{
				auto& intensity = obj.field(0, Name("intensity"));
				lulet(intensity_buf, intensity.check_f32_buf());
				memcpy(m_intensity.m, intensity_buf, sizeof(f32) * 3);
				auto& attenuation_power = obj.field(0, Name("attenuation_power"));
				luset(m_attenuation_power, attenuation_power.check_f32());
				auto intensity_multiplier = obj.field(0, Name("intensity_multiplier"));
				if (intensity_multiplier.type() != EVariantType::null)
				{
					luset(m_intensity_multiplier, intensity_multiplier.check_f32());
				}
				else
				{
					m_intensity_multiplier = 1.0f;
				}
			}
			lucatchret;
			return RV();
		}

		Scene::IComponentType* PointLight::type_object()
		{
			return &g_point_light_type.get();
		}

		R<Variant> SpotLight::serialize()
		{
			Variant var = Variant(EVariantType::table);
			auto intensity = Variant(EVariantType::f32, 3);
			intensity.to_f32_buf()[0] = m_intensity.r;
			intensity.to_f32_buf()[1] = m_intensity.g;
			intensity.to_f32_buf()[2] = m_intensity.b;
			var.set_field(0, Name("intensity"), intensity);
			auto attenuation_power = Variant(EVariantType::f32);
			attenuation_power.to_f32() = m_attenuation_power;
			auto spot_power = Variant(EVariantType::f32);
			spot_power.to_f32() = m_spot_power;
			var.set_field(0, Name("attenuation_power"), attenuation_power);
			var.set_field(0, Name("spot_power"), spot_power);
			auto intensity_multiplier = Variant(EVariantType::f32);
			intensity_multiplier.to_f32() = m_intensity_multiplier;
			var.set_field(0, Name("intensity_multiplier"), intensity_multiplier);
			return var;
		}

		RV SpotLight::deserialize(const Variant& obj)
		{
			lutry
			{
				auto& intensity = obj.field(0, Name("intensity"));
				lulet(intensity_buf, intensity.check_f32_buf());
				memcpy(m_intensity.m, intensity_buf, sizeof(f32) * 3);
				auto& attenuation_power = obj.field(0, Name("attenuation_power"));
				auto& spot_power = obj.field(0, Name("spot_power"));
				luset(m_attenuation_power, attenuation_power.check_f32());
				luset(m_spot_power, spot_power.check_f32());
				auto& intensity_multiplier = obj.field(0, Name("intensity_multiplier"));
				if (intensity_multiplier.type() != EVariantType::null)
				{
					luset(m_intensity_multiplier, intensity_multiplier.check_f32());
				}
				else
				{
					m_intensity_multiplier = 1.0f;
				}
			}
			lucatchret;
			return RV();
		}

		Scene::IComponentType* SpotLight::type_object()
		{
			return &g_spot_light_type.get();
		}
	}
}