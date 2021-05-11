// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneRendererComponentEditor.hpp
* @author JXMaster
* @date 2020/5/15
*/
#pragma once

#include "../ISceneComponentEditorType.hpp"

namespace Luna
{
	namespace editor
	{
		class SceneRendererComponentEditor : public ISceneComponentEditor
		{
		public:
			lucid("{740a5b6f-6409-427a-bcec-572601fde040}");
			luiimpl(SceneRendererComponentEditor, ISceneComponentEditor, IObject);

			Name m_type_name;
			WP<Scene::ISceneComponent> m_component;

			String m_camera_name;
			String m_skybox_name;

			SceneRendererComponentEditor() :
				m_camera_name(String()),
				m_skybox_name(String()) {}

			virtual Name type_name() override
			{
				return m_type_name;
			}
			virtual void on_render(ImGui::IContext* ctx) override;
		};

		class SceneRendererComponentEditorType : public ISceneComponentEditorType
		{
		public:
			lucid("{4673b2bd-57f1-4ec4-88e4-450291822ce6}");
			luiimpl(SceneRendererComponentEditorType, ISceneComponentEditorType, IObject);

			Name m_type_name;

			SceneRendererComponentEditorType() :
				m_type_name(Name("Scene Renderer")) {}

			virtual Name type() override
			{
				return m_type_name;
			}
			virtual P<ISceneComponentEditor> new_editor(Scene::ISceneComponent* component) override
			{
				auto r = newobj<SceneRendererComponentEditor>();
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}