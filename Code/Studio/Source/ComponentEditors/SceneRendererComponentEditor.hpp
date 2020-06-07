// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneRendererComponentEditor.hpp
* @author JXMaster
* @date 2020/5/15
*/
#pragma once

#include "../ISceneComponentEditorType.hpp"

namespace luna
{
	namespace editor
	{
		class SceneRendererComponentEditor : public ISceneComponentEditor
		{
		public:
			lucid("{740a5b6f-6409-427a-bcec-572601fde040}");
			luiimpl(SceneRendererComponentEditor, ISceneComponentEditor, IObject);

			P<IName> m_type_name;
			WP<scene::ISceneComponent> m_component;

			P<IStringBuffer> m_camera_name;
			P<IStringBuffer> m_skybox_name;

			SceneRendererComponentEditor(IAllocator* alloc) :
				luibind(alloc),
				m_camera_name(new_string_buffer()),
				m_skybox_name(new_string_buffer()) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}
			virtual void on_render(imgui::IContext* ctx) override;
		};

		class SceneRendererComponentEditorType : public ISceneComponentEditorType
		{
		public:
			lucid("{4673b2bd-57f1-4ec4-88e4-450291822ce6}");
			luiimpl(SceneRendererComponentEditorType, ISceneComponentEditorType, IObject);

			P<IName> m_type_name;

			SceneRendererComponentEditorType(IAllocator* alloc) :
				luibind(alloc),
				m_type_name(intern_name("Scene Renderer")) {}

			virtual IName* type() override
			{
				return m_type_name;
			}
			virtual P<ISceneComponentEditor> new_editor(scene::ISceneComponent* component) override
			{
				auto r = box_ptr(new_obj<SceneRendererComponentEditor>(get_module_allocator()));
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}