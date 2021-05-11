// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file LightsComponentEditor.hpp
* @author JXMaster
* @date 2020/5/20
*/
#pragma once
#include "../IComponentEditorType.hpp"
namespace Luna
{
	namespace editor
	{
		class DirectionalLightComponentEditor : public IComponentEditor
		{
		public:
			lucid("{c8b8721a-22ad-4ef3-8d32-87d6671989cf}");
			luiimpl(DirectionalLightComponentEditor, IComponentEditor, IObject);

			Name m_type_name;
			WP<Scene::IComponent> m_component;

			DirectionalLightComponentEditor() {}

			virtual Name type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(ImGui::IContext* ctx) override;
		};

		class PointLightComponentEditor : public IComponentEditor
		{
		public:
			lucid("{18bcac1e-b28a-46de-86b1-6a8e99c47b51}");
			luiimpl(PointLightComponentEditor, IComponentEditor, IObject);

			Name m_type_name;
			WP<Scene::IComponent> m_component;

			PointLightComponentEditor() {}

			virtual Name type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(ImGui::IContext* ctx) override;
		};

		class SpotlLightComponentEditor : public IComponentEditor
		{
		public:
			lucid("{c8b8721a-22ad-4ef3-8d32-87d6671989cf}");
			luiimpl(SpotlLightComponentEditor, IComponentEditor, IObject);

			Name m_type_name;
			WP<Scene::IComponent> m_component;

			SpotlLightComponentEditor() {}

			virtual Name type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(ImGui::IContext* ctx) override;
		};

		class DirectionalLightComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{41b820f7-fa84-453d-bebe-398a0cfeabee}");
			luiimpl(DirectionalLightComponentEditorType, IComponentEditorType, IObject);

			Name m_type_name;

			DirectionalLightComponentEditorType() :
				m_type_name(Name("Directional Light")) {}

			virtual Name type() override
			{
				return m_type_name;
			}
			virtual P<IComponentEditor> new_editor(Scene::IComponent* component) override
			{
				auto r = newobj<DirectionalLightComponentEditor>();
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};

		class PointLightComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{3ebfe846-89a3-439b-8d9c-e1b962b02589}");
			luiimpl(PointLightComponentEditorType, IComponentEditorType, IObject);

			Name m_type_name;

			PointLightComponentEditorType() :
				m_type_name(Name("Point Light")) {}

			virtual Name type() override
			{
				return m_type_name;
			}
			virtual P<IComponentEditor> new_editor(Scene::IComponent* component) override
			{
				auto r = newobj<PointLightComponentEditor>();
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};

		class SpotLightComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{86dfae1a-8786-4f83-83ef-15b410ead7cf}");
			luiimpl(SpotLightComponentEditorType, IComponentEditorType, IObject);

			Name m_type_name;

			SpotLightComponentEditorType() :
				m_type_name(Name("Spot Light")) {}

			virtual Name type() override
			{
				return m_type_name;
			}
			virtual P<IComponentEditor> new_editor(Scene::IComponent* component) override
			{
				auto r = newobj<SpotlLightComponentEditor>();
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}