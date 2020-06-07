// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRendererComponentEditor.hpp
* @author JXMaster
* @date 2020/5/28
*/
#pragma once

#include "../IComponentEditorType.hpp"

namespace luna
{
	namespace editor
	{
		class ModelRendererComponentEditor : public IComponentEditor
		{
		public:
			lucid("{991679a8-1354-415c-b0d6-74d25fc7476c}");
			luiimpl(ModelRendererComponentEditor, IComponentEditor, IObject);

			P<IName> m_type_name;
			WP<e3d::IModelRenderer> m_component;

			P<IStringBuffer> m_model_name;

			bool m_editing = false;

			ModelRendererComponentEditor(IAllocator* alloc) :
				luibind(alloc),
				m_model_name(new_string_buffer()) {}

			virtual IName* type_name() override
			{
				return m_type_name;
			}

			virtual void on_render(imgui::IContext* ctx) override;
		};

		class ModelRendererComponentEditorType : public IComponentEditorType
		{
		public:
			lucid("{df3ec6fd-36c4-4f27-8f72-47e318362b1b}");
			luiimpl(ModelRendererComponentEditorType, IComponentEditorType, IObject);

			P<IName> m_type_name;

			ModelRendererComponentEditorType(IAllocator* alloc) :
				luibind(alloc),
				m_type_name(intern_name("Model Renderer")) {}

			virtual IName* type() override
			{
				return m_type_name;
			}

			virtual P<IComponentEditor> new_editor(scene::IComponent* component) override
			{
				auto r = box_ptr(new_obj<ModelRendererComponentEditor>(get_module_allocator()));
				r->m_component = component;
				r->m_type_name = m_type_name;
				return r;
			}
		};
	}
}