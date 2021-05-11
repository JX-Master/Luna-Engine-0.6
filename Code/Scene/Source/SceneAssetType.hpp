// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file SceneAssetType.hpp
* @author JXMaster
* @date 2020/5/7
*/
#pragma once
#include "Scene.hpp"

namespace Luna
{
	namespace Scene
	{
		class SceneAssetType : public Asset::IAssetType
		{
		public:
			lucid("{bda25fc8-f1df-4ebe-b294-4572fe3aa4d6}");
			luiimpl_static(SceneAssetType, Asset::IAssetType, IObject);

			Name m_scene_type_name;

			SceneAssetType()
			{
				m_scene_type_name = Name(u8"Scene");
			}

			virtual Name type_name() override
			{
				return m_scene_type_name;
			}

			virtual P<Asset::IAsset> on_new_asset(Asset::IAssetMeta* meta) override;
			virtual RV on_load_data(Asset::IAsset* target_asset, const Variant& data, const Variant& params) override;
			virtual RV on_load_procedural_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_unload_data(Asset::IAsset* target_asset) override;
			virtual R<Variant> on_save_data(Asset::IAsset* target_asset, const Variant& params) override;
			virtual void on_dependency_data_load(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override;
			virtual void on_dependency_data_unload(Asset::IAsset* current_asset, Asset::IAsset* dependency_asset) override;
			virtual void on_dependency_replace(Asset::IAsset* current_asset, const Guid& before, const Guid& after) override;
		};
	}
}