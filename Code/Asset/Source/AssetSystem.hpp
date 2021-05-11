// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file AssetSystem.hpp
* @author JXMaster
* @date 2020/2/15
*/
#pragma once
#include "AssetHeader.hpp"
#include <Core/Interface.hpp>
#include <Runtime/HashMap.hpp>
#include <Runtime/RingDeque.hpp>
#include <Runtime/Functional.hpp>
namespace Luna
{
	namespace Asset
	{
		class AssetMeta;

		extern Name g_name_type;
		extern Name g_name_guid;
		extern Name g_name_data_path;
		extern Name g_name_attachments;
		extern Name g_name_dependencies;

		//! The registered asset types and their corresponding managers.
		extern Unconstructed<HashMap<Name, P<IAssetType>>> g_types;

		//! the lock for the asset types.
		extern P<IMutex> g_type_lock;

		//! The global asset registry.
		extern Unconstructed<HashMap<Guid, P<IAsset>>> g_assets;

		//! The mapping from meta path to asset Guid.
		extern Unconstructed<HashMap<Path, Guid>> g_path_mapping;

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
		inline RP<IAssetType> route_mgr(const Name& type)
		{
			MutexGuard guard(g_type_lock);
			auto iter = g_types.get().find(type);
			if (iter != g_types.get().end())
			{
				return iter->second;
			}
			return BasicError::not_found();
		}

		void add_dependency(AssetMeta* from, const Guid& to);
		bool remove_dependency(AssetMeta* from, const Guid& to);

		R<Variant> load_asset_from_file(const Path& path, bool load_meta = true);

		void insert_asset(IAsset* ass, AssetMeta* meta);
	}
}