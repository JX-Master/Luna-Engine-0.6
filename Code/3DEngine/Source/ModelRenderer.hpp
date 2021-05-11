// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRenderer.hpp
* @author JXMaster
* @date 2020/5/27
*/
#pragma once
#include "3DEngineHeader.hpp"
namespace Luna
{
	namespace E3D
	{
		class ModelRenderer : public IModelRenderer
		{
		public:
			lucid("{6c2d4c3a-1d81-4ff2-9a24-dfcf6a439e6c}");
			luiimpl(ModelRenderer, IModelRenderer, Scene::IComponent, ISerializable, IObject);

			WP<Scene::IEntity> m_belonging_entity;
			Asset::PAsset<IModel> m_model;

			ModelRenderer() {}

			virtual R<Variant> serialize() override;
			virtual RV deserialize(const Variant& obj) override;
			virtual Scene::IComponentType* type_object() override;
			virtual P<Scene::IEntity> belonging_entity() override;
			virtual Vector<Guid> referred_assets() override;
			virtual Asset::PAsset<IModel> model() override;
			virtual void set_model(Asset::PAsset<IModel> model) override;
		};

		class ModelRendererType : public Scene::IComponentType
		{
		public:
			lucid("{929d2a2f-2c22-474e-a677-d4169a85a8d4}");
			luiimpl_static(ModelRendererType, Scene::IComponentType, IObject);

			Name m_name;

			ModelRendererType() :
				m_name(Name("Model Renderer")) {}

			virtual Name type_name() override
			{
				return m_name;
			}
			virtual RP<Scene::IComponent> new_component(Scene::IEntity* belonging_entity) override
			{
				auto r = newobj<ModelRenderer>();
				r->m_belonging_entity = belonging_entity;
				return r;
			}
			virtual void on_dependency_data_load(Scene::IComponent* component, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(Scene::IComponent* component, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(Scene::IComponent* component, const Guid& before, const Guid& after) override;
		};

		extern Unconstructed<ModelRendererType> g_model_renderer_type;
	}
}