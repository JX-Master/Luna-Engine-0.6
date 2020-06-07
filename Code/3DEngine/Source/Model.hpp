// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Model.hpp
* @author JXMaster
* @date 2020/5/25
*/
#pragma once
#include "3DEngineHeader.hpp"
#include <Base/TSAssert.hpp>

namespace luna
{
	namespace e3d
	{
		class Model : public IModel
		{
		public:
			lucid("{39acd068-d2ff-456c-a7eb-b647bddfc5f9}");
			luiimpl(Model, IModel, asset::IAsset, IObject);
			lutsassert_lock();

			P<asset::IAssetMeta> m_meta;
			asset::PAsset<IMesh> m_mesh;

			Vector<asset::PAsset<IMaterial>> m_materials;

			Model(IAllocator* alloc) :
				luibind(alloc) {}

			virtual asset::IAssetMeta* meta() override
			{
				return m_meta;
			}

			virtual asset::PAsset<IMesh> mesh() override
			{
				lutsassert();
				return m_mesh;
			}

			virtual void set_mesh(asset::PAsset<IMesh> mesh_asset) override
			{
				lutsassert();
				asset::notify_asset_change(m_meta, m_mesh.guid(), mesh_asset.guid());
				m_mesh = mesh_asset;
			}

			virtual size_t count_materials() override
			{
				return m_materials.size();
			}

			virtual void add_material(size_t index) override
			{
				m_materials.insert(m_materials.begin() + index, asset::PAsset<IMaterial>(Guid(0, 0)));
			}

			virtual void remove_material(size_t index) override
			{
				asset::notify_asset_change(m_meta, m_materials[index].guid(), Guid(0, 0));
				m_materials.erase(m_materials.begin() + index);
			}

			virtual asset::PAsset<IMaterial> material(size_t index) override
			{
				return m_materials[index];
			}

			virtual void set_material(size_t index, asset::PAsset<IMaterial> material) override
			{
				luassert_usr(index < m_materials.size());
				asset::notify_asset_change(m_meta, m_materials[index].guid(), material.guid());
				m_materials[index] = material;
			}
		};

		class ModelType : public asset::IAssetType
		{
		public:
			lucid("{30706f65-573c-429c-80ce-22b1287b11e5}");
			luiimpl_static(ModelType, asset::IAssetType, IObject);

			P<IName> m_name;

			ModelType() :
				m_name(intern_name("Model")) {}

			virtual IName* type_name() override
			{
				return m_name;
			}
			virtual P<asset::IAsset> on_new_asset(asset::IAssetMeta* meta) override
			{
				auto r = box_ptr(new_obj<Model>(get_module_allocator()));
				r->m_meta = meta;
				return r;
			}
			virtual RV on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params) override;
			virtual RV on_load_procedural_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_unload_data(asset::IAsset* target_asset) override;
			virtual RP<IVariant> on_save_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_dependency_data_load(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_data_unload(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override {}
			virtual void on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after) override;
		};

		extern Unconstructed<ModelType> g_model_type;
	}
}