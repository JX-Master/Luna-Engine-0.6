// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file CameraComponentEditor.hpp
* @author JXMaster
* @date 2020/5/20
*/
#pragma once
#include "../IComponentEditorType.hpp"
namespace luna
{
	namespace editor
	{
		class CameraComponentEditor : public IComponentEditor
		{
		public:
			lucid("{e1357298-fac6-4f81-92c6-0903c818c8e3}");
			luiimpl(CameraComponentEditor, IComponentEditor, IObject);

			P<IName> m_type_name;
			WP<scene::IComponent> m_component;

			CameraComponentEditor(IAllocator* alloc) :
				luibind(alloc),
				m_type_name(intern_name("Camera")) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(imgui::IContext* ctx) override;
		};

		class CameraComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{c267e4d9-d29e-4e60-8d98-f87e0c6ac0fa}");
			luiimpl(CameraComponentEditorType, IComponentEditorType, IObject);

			P<IName> m_type_name;

			CameraComponentEditorType(IAllocator* alloc) :
				luibind(alloc),
				m_type_name(intern_name("Camera")) {}

			virtual IName* type() override
			{
				return m_type_name;
			}
			virtual P<IComponentEditor> new_editor(scene::IComponent* component) override
			{
				auto r = box_ptr(new_obj<CameraComponentEditor>(get_module_allocator()));
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}