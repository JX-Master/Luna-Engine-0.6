// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRenderer.hpp
* @author JXMaster
* @date 2020/5/27
*/
#pragma once
#include "3DEngineHeader.hpp"
namespace luna
{
	namespace e3d
	{
		class ModelRenderer : public IModelRenderer
		{
		public:
			lucid("{6c2d4c3a-1d81-4ff2-9a24-dfcf6a439e6c}");
			luiimpl(ModelRenderer, IModelRenderer, scene::IComponent, ISerializable, IObject);

			WP<scene::IEntity> m_belonging_entity;
			asset::PAsset<IModel> m_model;

			ModelRenderer(IAllocator* alloc) :
				luibind(alloc) {}

			virtual RP<IVariant> serialize() override;
			virtual RV deserialize(IVariant* obj) override;
			virtual scene::IComponentType* type_object() override;
			virtual P<scene::IEntity> belonging_entity() override;
			virtual Vector<Guid> referred_assets() override;
			virtual asset::PAsset<IModel> model() override;
			virtual void set_model(asset::PAsset<IModel> model) override;
		};

		class ModelRendererType : public scene::IComponentType
		{
		public:
			lucid("{929d2a2f-2c22-474e-a677-d4169a85a8d4}");
			luiimpl_static(ModelRendererType, scene::IComponentType, IObject);

			P<IName> m_name;

			ModelRendererType() :
				m_name(intern_name("Model Renderer")) {}

			virtual IName* type_name() override
			{
				return m_name;
			}
			virtual RP<scene::IComponent> new_component(scene::IEntity* belonging_entity) override
			{
				auto r = box_ptr(new_obj<ModelRenderer>(get_module_allocator()));
				r->m_belonging_entity = belonging_entity;
				return r;
			}
			virtual void on_dependency_data_load(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(scene::IComponent* component, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after) override;
		};

		extern Unconstructed<ModelRendererType> g_model_renderer_type;
	}
}