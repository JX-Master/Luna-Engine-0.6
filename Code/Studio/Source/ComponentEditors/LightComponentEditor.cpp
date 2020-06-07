// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file LightsComponentEditor.cpp
* @author JXMaster
* @date 2020/5/20
*/
#include "LightComponentEditor.hpp"

namespace luna
{
	namespace editor
	{
		void DirectionalLightComponentEditor::on_render(imgui::IContext* ctx)
		{
			P<e3d::IDirectionalLight> c = m_component.lock();

			Float3 intensity = c->intensity();
			ctx->color_edit3("Intensity", intensity.m, imgui::EColorEditFlag::hdr);
			c->set_intensity(intensity);
			float32 intensity_multiplier = c->intensity_multiplier();
			ctx->drag_float("Intensity Multiplier", &intensity_multiplier, 1.0f, 0.0f, FLT_MAX);
			c->set_intensity_multiplier(intensity_multiplier);
		}
		void PointLightComponentEditor::on_render(imgui::IContext* ctx)
		{
			P<e3d::IPointLight> c = m_component.lock();

			Float3 intensity = c->intensity();
			ctx->color_edit3("Intensity", intensity.m, imgui::EColorEditFlag::hdr);
			c->set_intensity(intensity);
			float32 intensity_multiplier = c->intensity_multiplier();
			ctx->drag_float("Intensity Multiplier", &intensity_multiplier, 1.0f, 0.0f, FLT_MAX);
			c->set_intensity_multiplier(intensity_multiplier);
			float32 attenuation_power = c->attenuation_power();
			ctx->drag_float("Attenuation Power", &attenuation_power, 0.1f, 0.0001f, FLT_MAX);
			c->set_attenuation_power(attenuation_power);
		}
		void SpotlLightComponentEditor::on_render(imgui::IContext* ctx)
		{
			P<e3d::ISpotLight> c = m_component.lock();

			Float3 intensity = c->intensity();
			ctx->color_edit3("Intensity", intensity.m, imgui::EColorEditFlag::hdr);
			c->set_intensity(intensity);
			float32 intensity_multiplier = c->intensity_multiplier();
			ctx->drag_float("Intensity Multiplier", &intensity_multiplier, 1.0f, 0.0f, FLT_MAX);
			c->set_intensity_multiplier(intensity_multiplier);
			float32 attenuation_power = c->attenuation_power();
			float32 spot_power = c->spot_power();
			ctx->drag_float("Attenuation Power", &attenuation_power, 0.1f, 0.0001f, FLT_MAX);
			ctx->drag_float("Spot Power", &spot_power, 0.1f, 0.0f, FLT_MAX);
			c->set_attenuation_power(attenuation_power);
			c->set_spot_power(spot_power);
		}
	}
}