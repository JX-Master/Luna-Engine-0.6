// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneAssetType.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "Scene.hpp"

namespace luna
{
	namespace scene
	{
		class SceneAssetType : public asset::IAssetType
		{
		public:
			lucid("{bda25fc8-f1df-4ebe-b294-4572fe3aa4d6}");
			luiimpl_static(SceneAssetType, asset::IAssetType, IObject);

			P<IName> m_scene_type_name;

			SceneAssetType()
			{
				m_scene_type_name = intern_name("Scene");
			}

			virtual IName* type_name() override
			{
				return m_scene_type_name;
			}

			virtual P<asset::IAsset> on_new_asset(asset::IAssetMeta* meta) override;
			virtual RV on_load_data(asset::IAsset* target_asset, IVariant* data, IVariant* params) override;
			virtual RV on_load_procedural_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_unload_data(asset::IAsset* target_asset) override;
			virtual RP<IVariant> on_save_data(asset::IAsset* target_asset, IVariant* params) override;
			virtual void on_dependency_data_load(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override;
			virtual void on_dependency_data_unload(asset::IAsset* current_asset, asset::IAsset* dependency_asset) override;
			virtual void on_dependency_replace(asset::IAsset* current_asset, const Guid& before, const Guid& after) override;
		};
	}
}