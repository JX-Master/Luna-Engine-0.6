// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Model.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include "3DEngineHeader.hpp"
#include <Runtime/TSAssert.hpp>

namespace Luna
{
	namespace E3D
	{
		class Model : public IModel
		{
		public:
			lucid("{39acd068-d2ff-456c-a7eb-b647bddfc5f9}");
			luiimpl(Model, IModel, Asset::IAsset, IObject);
			lutsassert_lock();

			P<Asset::IAssetMeta> m_meta;
			Asset::PAsset<IMesh> m_mesh;

			Vector<Asset::PAsset<IMaterial>> m_materials;

			Model() {}

			virtual Asset::IAssetMeta* meta() override
			{
				return m_meta;
			}

			virtual Asset::PAsset<IMesh> mesh() override
			{
				lutsassert();
				return m_mesh;
			}

			virtual void set_mesh(Asset::PAsset<IMesh> mesh_asset) override
			{
				lutsassert();
				Asset::notify_asset_change(m_meta, m_mesh.guid(), mesh_asset.guid());
				m_mesh = mesh_asset;
			}

			virtual usize count_materials() override
			{
				return m_materials.size();
			}

			virtual void add_material(usize index) override
			{
				m_materials.insert(m_materials.begin() + index, Asset::PAsset<IMaterial>(Guid(0, 0)));
			}

			virtual void remove_material(usize index) override
			{
				Asset::notify_asset_change(m_meta, m_materials[index].guid(), Guid(0, 0));
				m_materials.erase(m_materials.begin() + index);
			}

			virtual Asset::PAsset<IMaterial> material(usize index) override
			{
				return m_materials[index];
			}

			virtual void set_material(usize index, Asset::PAsset<IMaterial> material) override
			{
				lucheck(index < m_materials.size());
				Asset::notify_asset_change(m_meta, m_materials[index].guid(), material.guid());
				m_materials[index] = material;
			}
		};

		class ModelType : public Asset::IAssetType
		{
		public:
			lucid("{30706f65-573c-429c-80ce-22b1287b11e5}");
			luiimpl_static(ModelType, Asset::IAssetType, IObject);

			Name m_name;

			ModelType() :
				m_name(Name("Model")) {}

			virtual Name type_name() override
			{
				return m_name;
			}
			virtual P<Asset::IAsset> on_new_asset(Asset::IAssetMeta* meta) override
			{
				auto r = newobj<Model>();
				r->m_meta = meta;
				return r;
			}
			virtual RV on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params) override;
			virtual RV on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_unload_data(Asset::IAsset* target_asset) override;
			virtual R<Variant> on_save_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_dependency_data_load(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(Asset::IAsset* current_asset, const Guid& before, const Guid& after) override;
		};

		extern Unconstructed<ModelType> g_model_type;
	}
}