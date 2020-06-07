// Copyright 2018-2020 JXMaster. All rights reserved.
/*
* @file Asset.hpp
* @author JXMaster
* @date 2020/5/10
*/
#pragma once
#include "IAsset.hpp"
#include "IAssetType.hpp"

#ifndef LUNA_ASSET_API
#define LUNA_ASSET_API
#endif

namespace luna
{
	namespace asset
	{
		//constexpr Guid err_domain_asset{ "{7941556e-fd28-48ef-ba4c-a5adfc44ed46}" };

		//constexpr result_t e_asset_not_resolved = e_user_failure - 1;
		//constexpr result_t e_asset_not_loaded = e_user_failure - 2;

		LUNA_ASSET_API void init();

		//! Registers an asset type. 
		//! The asset system keeps a strong reference to the asset type object, all objects created for the 
		//! specified type will also keep a strong reference to the asset type object.
		LUNA_ASSET_API RV register_asset_type(IAssetType* type_obj);

		//! Unregisters an asset type.
		LUNA_ASSET_API RV unregister_asset_type(IName* type_name);

		//! Gets the registered asset type.
		LUNA_ASSET_API RP<IAssetType> get_asset_type(IName* type_name);

		//! Loads the asset metadata from the metadata file from the virtual file system and creates one 
		//! empty asset instance.
		//! @param[in] meta_path The path of the meta file, without extension. The system automatically
		//! searches files in the following order:
		//! * {meta_path}.asset.la
		//! * {meta_path}.asset.lb
		//! If `path` field is not set in the meta file, it will be set to the same path that contains 
		//! the meta file, which is one of the following two files:
		//! * {meta_path}.data.la
		//! * {meta_path}.data.lb
		//! @return Returns `s_ok` if the asset metadata is loaded from file; returns `s_already_done` if 
		//! one asset with the same GUID already exists in the registry (the original registry will not be 
		//! overwritten); returns failure on error.
		LUNA_ASSET_API RP<IAsset> load_asset_meta(IPath* meta_path);

		//! Creates a new empty asset instance for the specified asset type.
		//! 
		//! This call does not save the asset meta and data to file, the user should call `IAsset::save` to do that.
		//! 
		//! The data for the new asset is not loaded, and the meta & data path is not set as well. The user should set
		//! a valid path before loading or saving meta & data information.
		//! @param[in] asset_type The type of the new asset.
		//! @param[in] guid The guid of the new asset. If this is `nullptr`, the new guid will be randomly generated.
		LUNA_ASSET_API RP<IAsset> new_asset(IName* asset_type, const Guid* guid = nullptr);

		//! Fetches an asset by its GUID.
		LUNA_ASSET_API RP<IAsset> fetch_asset(const Guid& asset_id);

		//! Fetches an asset by its meta path. The path does not include extension.
		LUNA_ASSET_API RP<IAsset> fetch_asset(IPath* meta_path);

		//! Removes one asset from the asset registry. The asset system keeps strong references to the assets 
		//! until it is removed.
		//! 
		//! Usually you don't need to remove the asset itself, instead you just need to unload the asset data 
		//! when it is not used. You may call this if one asset needs to be is deleted completely.
		LUNA_ASSET_API RV remove_asset(const Guid& asset_id);

		//! Gets the streaming dispatch queue used for loading, saving and reloading assets. The asset implementation
		//! may also use this queue to do some loading-related works without the need to open another queue.
		LUNA_ASSET_API IDispatchQueue* get_streaming_queue();

		//! A smart pointer used for asset object. `PAsset` contains the Guid of the asset 
		//! so that it can reference the asset even when the asset object is not loaded.
		template <typename _Ty>
		class PAsset
		{
		private:
			Guid m_guid;
			mutable WP<_Ty> m_ass;
		public:
			PAsset() :
				m_guid(0, 0) {}
			PAsset(const Guid& rhs) :
				m_guid(rhs) {}
			PAsset(IAsset* ass)
			{
				if (ass && ass->meta()->is_valid())
				{
					m_guid = ass->meta()->guid();
					m_ass = ass;
				}
				else
				{
					m_guid = Guid(0, 0);
					m_ass = nullptr;
				}
			}
			PAsset(const PAsset&) = default;
			PAsset(PAsset&&) = default;
			PAsset& operator=(const PAsset&) = default;
			PAsset& operator=(PAsset&&) = default;

			PAsset& operator=(const Guid& rhs)
			{
				m_guid = rhs;
				m_ass = nullptr;	// Will be resolved in the next time `lock` is called.
				return *this;
			}

			PAsset& operator=(IAsset* rhs)
			{
				if (rhs && rhs->meta()->is_valid())
				{
					m_guid = rhs->meta()->guid();
					m_ass = rhs;
				}
				else
				{
					m_guid = Guid(0, 0);
					m_ass = nullptr;
				}
				return *this;
			}

			Guid guid() const
			{
				return m_guid;
			}

			bool null() const
			{
				return m_guid == Guid(0, 0);
			}

			P<_Ty> lock() const
			{
				auto ass = m_ass.lock();
				if (ass)
				{
					return ass;
				}
				if (m_guid == Guid(0, 0))
				{
					return nullptr;
				}
				// Try to resolve the asset.
				auto fetch = asset::fetch_asset(m_guid);
				if (succeeded(fetch))
				{
					ass = fetch.get();
					m_ass = ass;
					return ass;
				}
				return nullptr;
			}
		};

		inline void notify_asset_change(IAssetMeta* meta, const Guid& before, const Guid& after)
		{
			if (before == after)
			{
				return;
			}
			if (before != Guid(0, 0))
			{
				meta->internal_remove_dependency(before);
			}
			if (after != Guid(0, 0))
			{
				meta->internal_add_dependency(after);
			}
		}
	}
}