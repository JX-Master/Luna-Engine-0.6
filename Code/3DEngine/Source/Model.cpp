// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Model.cpp
* @author JXMaster
* @date 2020/5/27
*/
#include "Model.hpp"

namespace luna
{
	namespace e3d
	{
		Unconstructed<ModelType> g_model_type;
		RV ModelType::on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params)
		{
			P<Model> mdl = target_asset;
			lutry
			{
				auto m = data->field(0, intern_name("mesh"));
				if (succeeded(m))
				{
					lulet(m_v, m.get()->check_u64_buf());
					mdl->m_mesh = Guid(m_v[0], m_v[1]);
				}
				auto mats = data->field(0, intern_name("materials"));
				if (succeeded(mats))
				{
					lulet(mats_data, mats.get()->check_var_buf());
					size_t num_mats = mats.get()->length(1);
					mdl->m_materials.reserve(num_mats);
					for (size_t i = 0; i < num_mats; ++i)
					{
						lulet(mat, mats_data[i]->check_u64_buf());
						mdl->m_materials.push_back(Guid(mat[0], mat[1]));
					}
				}
			}
			lucatchret;
			return s_ok;
		}
		RV ModelType::on_load_procedural_data(asset::IAsset* target_asset, IVariant* params)
		{
			P<Model> mdl = target_asset;
			mdl->m_meta->internal_remove_all_dependencies();
			mdl->m_mesh = Guid(0, 0);
			mdl->m_materials.clear();
			if (!params)
			{
				return s_ok;
			}
			lutry
			{
				auto m = params->field(0, intern_name("mesh"));
				if (succeeded(m))
				{
					lulet(m_v, m.get()->check_u64_buf());
					mdl->m_mesh = Guid(m_v[0], m_v[1]);
					mdl->m_meta->internal_add_dependency(mdl->m_mesh.guid());
				}
				auto mats = params->field(0, intern_name("materials"));
				if (succeeded(mats))
				{
					lulet(mats_data, mats.get()->check_var_buf());
					size_t num_mats = mats.get()->length(1);
					mdl->m_materials.reserve(num_mats);
					for (size_t i = 0; i < num_mats; ++i)
					{
						lulet(mat, mats_data[i]->check_u64_buf());
						mdl->m_materials.push_back(Guid(mat[0], mat[1]));
						asset::notify_asset_change(mdl->m_meta, Guid(0, 0), Guid(mat[0], mat[1]));
					}
				}
			}
			lucatchret;
			return s_ok;
		}
		void ModelType::on_unload_data(asset::IAsset* target_asset)
		{
			P<Model> mdl = target_asset;
			mdl->m_mesh = Guid(0, 0);
			mdl->m_materials.clear();
		}
		RP<IVariant> ModelType::on_save_data(asset::IAsset* target_asset, IVariant* params)
		{
			P<Model> mdl = target_asset;
			auto var = new_var(EVariantType::table);
			if (!mdl->m_mesh.null())
			{
				auto mesh = new_var1(EVariantType::u64, 2);
				mesh->u64_buf()[0] = mdl->m_mesh.guid().high;
				mesh->u64_buf()[1] = mdl->m_mesh.guid().low;
				var->set_field(0, intern_name("mesh"), mesh);

				size_t num_mats = mdl->m_materials.size();
				if (num_mats)
				{
					auto mats = new_var1(EVariantType::variant, num_mats);
					auto mats_data = mats->var_buf();
					for (size_t i = 0; i < num_mats; ++i)
					{
						auto mat = new_var1(EVariantType::u64, 2);
						mat->u64_buf()[0] = mdl->m_materials[i].guid().high;
						mat->u64_buf()[1] = mdl->m_materials[i].guid().low;
						mats_data[i] = mat;
					}
					var->set_field(0, intern_name("materials"), mats);
				}
			}
			return var;
		}
		void luna::e3d::ModelType::on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after)
		{
			P<Model> mdl = current_asset;
			if (before == mdl->m_mesh.guid())
			{
				mdl->m_mesh = after;
			}
		}
	}
}