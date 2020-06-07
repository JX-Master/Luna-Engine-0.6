// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneRendererComponentEditor.cpp
* @author JXMaster
* @date 2020/5/15
*/
#include "SceneRendererComponentEditor.hpp"

namespace luna
{
	namespace editor
	{
		void SceneRendererComponentEditor::on_render(imgui::IContext* ctx)
		{
			auto c = m_component.lock().as<e3d::ISceneRenderer>();

			auto camera = c->camera_entity();
			if (camera)
			{
				m_camera_name->assign(camera->name()->c_str());
			}
			ctx->input_text("Camera Entity", m_camera_name);
			if (!camera || strcmp(m_camera_name->c_str(), camera->name()->c_str()))
			{
				auto camera_entity = c->belonging_scene()->find_entity(intern_name(m_camera_name->c_str()));
				if (succeeded(camera_entity))
				{
					c->set_camera_entity(camera_entity.get());
				}
			}
			if (camera)
			{
				ctx->text("Main Camera is connected.");
			}
			else
			{
				ctx->text_colored(color::red, "Main Camera is not available.");
			}

			auto expo = c->exposure();
			ctx->drag_float("Exposure", &expo, 1.0f, 0.0f, FLT_MAX);
			c->set_exposure(expo);

			auto skybox = c->skybox_texture().guid();
			input_asset_entry(ctx, "Skybox", m_skybox_name, skybox, intern_name("Texture"));
			c->set_skybox_texture(skybox);

			auto env_color = c->environment_color();
			ctx->color_edit3("Environment Color", env_color.m, imgui::EColorEditFlag::hdr);
			c->set_environment_color(env_color);
		}
	}
}