// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetSystem.hpp
* @author JXMaster
* @date 2020/2/15
*/
#pragma once
#include "AssetHeader.hpp"
#include <Base/Interface.hpp>
#include <Base/HashMap.hpp>
#include <Base/RingDeque.hpp>
#include <Base/Functional.hpp>
namespace luna
{
	namespace asset
	{
		class AssetMeta;

		extern P<IName> g_name_type;
		extern P<IName> g_name_guid;
		extern P<IName> g_name_data_path;
		extern P<IName> g_name_attachments;
		extern P<IName> g_name_dependencies;

		//! The registered asset types and their corresponding managers.
		extern Unconstructed<HashMap<P<IName>, P<IAssetType>>> g_types;

		//! the lock for the asset types.
		extern P<IReadWriteLock> g_type_lock;

		//! The global asset registry.
		extern Unconstructed<HashMap<Guid, P<IAsset>>> g_assets;

		//! The mapping from meta path to asset Guid.
		extern Unconstructed<HashMap<P<IPath>, Guid>> g_path_mapping;

		//! The lock for the asset registry.
		extern P<IMutex> g_lock;

		//! The streaming dispatch queue.
		extern P<IDispatchQueue> g_dispatch;

		//! Unresolved table.
		//! Key: The Guid of the unresolved asset.
		//! Value: Assets that depends on the unresolved asset.
		extern Unconstructed<HashMap<Guid, Vector<Guid>>> m_unresolved;

		void deinit();

		//! Gets the manager responsible for the specified extension.
		inline RP<IAssetType> route_mgr(IName* type)
		{
			ReadGuard guard(g_type_lock);
			auto iter = g_types.get().find(type);
			if (iter != g_types.get().end())
			{
				return iter->second;
			}
			return e_item_not_exist;
		}

		void add_dependency(AssetMeta* from, const Guid& to);
		bool remove_dependency(AssetMeta* from, const Guid& to);

		RP<IVariant> load_asset_from_file(IPath* path, bool load_meta = true);

		void insert_asset(IAsset* ass, AssetMeta* meta);
	}
}