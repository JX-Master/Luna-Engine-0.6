// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file ModelRenderer.cpp
* @author JXMaster
* @date 2020/5/27
*/
#include "ModelRenderer.hpp"

namespace luna
{
	namespace e3d
	{
		Unconstructed<ModelRendererType> g_model_renderer_type;
		RP<IVariant> ModelRenderer::serialize()
		{
			auto var = new_var(EVariantType::table);
			auto model = new_var1(EVariantType::u64, 2);
			model->u64_buf()[0] = m_model.guid().high;
			model->u64_buf()[1] = m_model.guid().low;
			var->set_field(0, intern_name("model"), model);
			return var;
		}
		RV ModelRenderer::deserialize(IVariant* obj)
		{
			lutry
			{
				lulet(m, obj->field(0, intern_name("model")));
				lulet(m_buf, m->check_u64_buf());
				m_model = Guid(m_buf[0], m_buf[1]);
			}
			lucatchret;
			return s_ok;
		}
		scene::IComponentType* ModelRenderer::type_object()
		{
			return &g_model_renderer_type.get();
		}
		P<scene::IEntity> ModelRenderer::belonging_entity()
		{
			return m_belonging_entity.lock();
		}
		Vector<Guid> ModelRenderer::referred_assets()
		{
			Vector<Guid> assets;
			if (!m_model.null())
			{
				assets.push_back(m_model.guid());
			}
			return assets;
		}
		asset::PAsset<IModel> ModelRenderer::model()
		{
			return m_model;
		}
		void ModelRenderer::set_model(asset::PAsset<IModel> model)
		{
			asset::notify_asset_change(m_belonging_entity.lock()->belonging_scene()->meta(), m_model.guid(), model.guid());
			m_model = model;
		}
		void ModelRendererType::on_dependency_replace(scene::IComponent* component, const Guid& before, const Guid& after)
		{
			P<ModelRenderer> renderer = component;
			if (renderer->m_model.guid() == before)
			{
				renderer->m_model = after;
			}
		}
	}
}