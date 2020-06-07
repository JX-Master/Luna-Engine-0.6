// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file TransformComponentEditor.hpp
* @author JXMaster
* @date 2020/5/18
*/
#pragma once
#include "../IComponentEditorType.hpp"
namespace luna
{
	namespace editor
	{
		class TransformComponentEditor : public IComponentEditor
		{
		public:
			lucid("{7a2f49a1-a375-4936-a9cd-004f71442f33}");
			luiimpl(TransformComponentEditor, IComponentEditor, IObject);

			P<IName> m_type_name;
			WP<scene::IComponent> m_component;

			Float3U m_cached_rotation_eular = { 0.0f, 0.0f, 0.0f };
			
			TransformComponentEditor(IAllocator* alloc) :
				luibind(alloc) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(imgui::IContext* ctx) override;
		};

		class TransformComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{15a0a5a4-1eba-466e-b2cb-211ca97c173c}");
			luiimpl(TransformComponentEditorType, IComponentEditorType, IObject);

			P<IName> m_type_name;

			TransformComponentEditorType(IAllocator* alloc) :
				luibind(alloc),
				m_type_name(intern_name("Transform")) {}

			virtual IName* type() override
			{
				return m_type_name;
			}
			virtual P<IComponentEditor> new_editor(scene::IComponent* component) override
			{
				auto r = box_ptr(new_obj<TransformComponentEditor>(get_module_allocator()));
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}