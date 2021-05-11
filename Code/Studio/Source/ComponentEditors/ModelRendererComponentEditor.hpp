// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRendererComponentEditor.hpp
* @author JXMaster
* @date 2020/5/28
*/
#pragma once

#include "../IComponentEditorType.hpp"

namespace Luna
{
	namespace editor
	{
		class ModelRendererComponentEditor : public IComponentEditor
		{
		public:
			lucid("{991679a8-1354-415c-b0d6-74d25fc7476c}");
			luiimpl(ModelRendererComponentEditor, IComponentEditor, IObject);

			Name m_type_name;
			WP<E3D::IModelRenderer> m_component;

			String m_model_name;

			bool m_editing = false;

			ModelRendererComponentEditor() :
				m_model_name(String()) {}

			virtual Name type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(ImGui::IContext* ctx) override;
		};

		class ModelRendererComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{df3ec6fd-36c4-4f27-8f72-47e318362b1b}");
			luiimpl(ModelRendererComponentEditorType, IComponentEditorType, IObject);

			Name m_type_name;

			ModelRendererComponentEditorType() :
				m_type_name(Name("Model Renderer")) {}

			virtual Name type() override
			{
				return m_type_name;
			}

			virtual P<IComponentEditor> new_editor(Scene::IComponent* component) override
			{
				auto r = newobj<ModelRendererComponentEditor>();
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}