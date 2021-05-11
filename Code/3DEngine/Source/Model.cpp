// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Model.cpp
* @author JXMaster
* @date 2020/5/27
*/
#include "Model.hpp"

namespace Luna
{
	namespace E3D
	{
		Unconstructed<ModelType> g_model_type;
		RV ModelType::on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params)
		{
			P<Model> mdl = target_asset;
			lutry
			{
				auto& m = data.field(0, Name("mesh"));
				if (m.type() != EVariantType::null)
				{
					lulet(m_v, m.check_u64_buf());
					mdl->m_mesh = Guid(m_v[0], m_v[1]);
				}
				auto& mats = data.field(0, Name("materials"));
				if (mats.type() != EVariantType::null)
				{
					lulet(mats_data, mats.check_var_buf());
					usize num_mats = mats.length(1);
					mdl->m_materials.reserve(num_mats);
					for (usize i = 0; i < num_mats; ++i)
					{
						lulet(mat, mats_data[i].check_u64_buf());
						mdl->m_materials.push_back(Guid(mat[0], mat[1]));
					}
				}
			}
			lucatchret;
			return RV();
		}
		RV ModelType::on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params)
		{
			P<Model> mdl = target_asset;
			mdl->m_meta->internal_remove_all_dependencies();
			mdl->m_mesh = Guid(0, 0);
			mdl->m_materials.clear();
			if (params.type() == EVariantType::null)
			{
				return RV();
			}
			lutry
			{
				auto& m = params.field(0, Name("mesh"));
				if (m.type() != EVariantType::null)
				{
					lulet(m_v, m.check_u64_buf());
					mdl->m_mesh = Guid(m_v[0], m_v[1]);
					mdl->m_meta->internal_add_dependency(mdl->m_mesh.guid());
				}
				auto& mats = params.field(0, Name("materials"));
				if (mats.type() != EVariantType::null)
				{
					lulet(mats_data, mats.check_var_buf());
					usize num_mats = mats.length(1);
					mdl->m_materials.reserve(num_mats);
					for (usize i = 0; i < num_mats; ++i)
					{
						lulet(mat, mats_data[i].check_u64_buf());
						mdl->m_materials.push_back(Guid(mat[0], mat[1]));
						Asset::notify_asset_change(mdl->m_meta, Guid(0, 0), Guid(mat[0], mat[1]));
					}
				}
			}
			lucatchret;
			return RV();
		}
		void ModelType::on_unload_data(Asset::IAsset* target_asset)
		{
			P<Model> mdl = target_asset;
			mdl->m_mesh = Guid(0, 0);
			mdl->m_materials.clear();
		}
		R<Variant> ModelType::on_save_data(Asset::IAsset* target_asset, const Variant& params)
		{
			P<Model> mdl = target_asset;
			auto var = Variant(EVariantType::table);
			if (!mdl->m_mesh.null())
			{
				auto mesh = Variant(EVariantType::u64, 2);
				mesh.to_u64_buf()[0] = mdl->m_mesh.guid().high;
				mesh.to_u64_buf()[1] = mdl->m_mesh.guid().low;
				var.set_field(0, Name("mesh"), mesh);

				usize num_mats = mdl->m_materials.size();
				if (num_mats)
				{
					auto mats = Variant(EVariantType::variant, num_mats);
					auto mats_data = mats.to_var_buf();
					for (usize i = 0; i < num_mats; ++i)
					{
						auto mat = Variant(EVariantType::u64, 2);
						mat.to_u64_buf()[0] = mdl->m_materials[i].guid().high;
						mat.to_u64_buf()[1] = mdl->m_materials[i].guid().low;
						mats_data[i] = mat;
					}
					var.set_field(0, Name("materials"), mats);
				}
			}
			return var;
		}
		void Luna::E3D::ModelType::on_dependency_replace(Asset::IAsset* current_asset, const Guid& before, const Guid& after)
		{
			P<Model> mdl = current_asset;
			if (before == mdl->m_mesh.guid())
			{
				mdl->m_mesh = after;
			}
		}
	}
}